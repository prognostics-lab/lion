%% Set simulation parameters
% Parameters
REAL_PARAMS = dictionary("cp", 100, ...
    "cair", 1e-3, ...
    "rin", 3, ...
    "rout", 9, ...
    "rair", 1e-3, ...
    "in_temp", 298, ...
    "air_temp", 298);

% Write parameters to file
encoded = jsonencode(struct("cp", REAL_PARAMS("cp"), ...
    "cair", REAL_PARAMS("cair"), ...
    "rin", REAL_PARAMS("rin"), ...
    "rout", REAL_PARAMS("rout"), ...
    "rair", REAL_PARAMS("rair"), ...
    "in_temp", REAL_PARAMS("in_temp"), ...
    "air_temp", REAL_PARAMS("air_temp")));
fid = fopen("tests/sanity_check/params.json", "w");
fprintf(fid, "%s", encoded);
fclose(fid);

%% Data loading
% % Load csv file
% csv_dir = "data/240209_temptest_C6B2/TestData.csv";
% opts = detectImportOptions(csv_dir);
% opts = setvaropts(opts, "Seconds", "InputFormat", "MM/dd/uuuu HH:mm:ss.SSS");
% csv_table = readtable(csv_dir, opts);
% 
% % Determine relevant parameters
% SEGMENT = 4;
% TIME_LIMIT = 50;
% 
% % Extract data
% time = csv_table(:, "Seconds") - csv_table(1, "Seconds");
% time = seconds(time.(1));
% current = csv_table(:, "Current").(1);
% voltage = csv_table(:, "Voltage").(1);
% [time, power] = get_segment(time, current, voltage, SEGMENT, TIME_LIMIT);
% amb_temp = 298 + 0 * sin(2 * pi * 0.0001 * time);


% % Artificially generate data
% time = linspace(0, 100000, 1000000)';
% end_time = time(end);
% SEGMENTS = 10;
% time_mod = (time(end) - time(1)) / SEGMENTS;
% power = 20 * sin(2 * pi * (0.000005 .* mod(time, time_mod) + 0) .* mod(time, time_mod));
% % power = 20 * chirp(time, 0, time(end), 0.1);
% % power = 0.1;
% % amb_temp = 298 + 0 * sin(2 * pi * 0.0001 * time);
% AMB_TEMP_DELTA = SEGMENTS / 2;
% amb_temp = 298 + floor(2 * AMB_TEMP_DELTA * time / time(end)) - AMB_TEMP_DELTA;
% % amb_temp = 298;


% Experiment for Rout estimation
exp1_time = linspace(0, 10000, 100000)';
exp1_power = 0.2 * ones(size(exp1_time));
exp1_amb_temp = 273 * ones(size(exp1_time));

exp2_time = linspace(0, 100000, 100000)';
exp2_power = 3 * (square(2 * pi * (5 / (exp2_time(end) - exp2_time(1))) * exp2_time));
exp2_amb_temp = 273 * ones(size(exp2_time));

%% Generate timeseries
exp1_power_profile = timeseries(exp1_power, exp1_time);
exp1_amb_profile = timeseries(exp1_amb_temp, exp1_time);
exp1_time_delta = exp1_time(2) - exp1_time(1);
exp1_end_time = exp1_time(end);

exp2_power_profile = timeseries(exp2_power, exp2_time);
exp2_amb_profile = timeseries(exp2_amb_temp, exp2_time);
exp2_time_delta = exp2_time(2) - exp2_time(1);
exp2_end_time = exp2_time(end);

%% Set simulation parameters and run simulation
mdl = "sanity_check_sim";

% Experiment 1 (Rout estimation)
disp("Running experiment 1");
power_profile = exp1_power_profile;
amb_profile = exp1_amb_profile;
out1 = evaluate_model(exp1_time_delta, exp1_end_time, mdl, REAL_PARAMS, 0.1);
out_table1 = table(out1.tout, out1.simout.sf_temp.Data, out1.simout.air_temp.Data, ...
    out1.simout.q_gen.Data, out1.ambient.Data, ...
    VariableNames=["time", "sf_temp", "air_temp", "q_gen", "amb_temp"]);
writetable(out_table1, "tests/sanity_check/exp1_sim.csv");

% Experiment 2 (Rin, Cp)
disp("Running experiment 2");
power_profile = exp2_power_profile;
amb_profile = exp2_amb_profile;
out2 = evaluate_model(exp2_time_delta, exp2_end_time, mdl, REAL_PARAMS, 0.8);
out_table2 = table(out2.tout, out2.simout.sf_temp.Data, out2.simout.air_temp.Data, ...
    out2.simout.q_gen.Data, out2.ambient.Data, ...
    VariableNames=["time", "sf_temp", "air_temp", "q_gen", "amb_temp"]);
writetable(out_table2, "tests/sanity_check/exp2_sim.csv");

%% Helper functions
function [time, power] = get_segment(table_time, table_current, table_voltage, segment, time_limit)
% Initialize helper variables
curr_seg = 1;
prev_time = 0;
segment_id = zeros(length(table_time), 1);

% Identify segments
for i = 1:length(table_time)
    t = table_time(i);
    if abs(t - prev_time) >= time_limit
        curr_seg = curr_seg + 1;
    end
    segment_id(i) = curr_seg;
    prev_time = t;
end

time = table_time(segment_id == segment);
time = time - time(1);
current = table_current(segment_id == segment);
voltage = table_voltage(segment_id == segment);
power = current .* voltage;
end


function out = evaluate_model(time_delta, end_time, mdl, params, initial_soc)
cp = params("cp");
cair = params("cair");
rin = params("rin");
rout = params("rout");
rair = params("rair");
in_temp = params("in_temp");
air_temp = params("air_temp");
handle = load_system(mdl);
simin = Simulink.SimulationInput(mdl);
set_param(mdl, "SimulationCommand", "update");
simin = setModelParameter(simin, ...
    Solver="ode14x", ...
    StopTime=string(end_time), ...
    FixedStep=string(time_delta));
simin = setBlockParameter(simin, ...
    strcat(mdl, "/Battery"), "th_cp", string(cp), ...
    strcat(mdl, "/Battery"), "th_cair", string(cair), ...
    strcat(mdl, "/Battery"), "th_rin", string(rin), ...
    strcat(mdl, "/Battery"), "th_rout", string(rout), ...
    strcat(mdl, "/Battery"), "th_rair", string(rair), ...
    strcat(mdl, "/Battery"), "initial_in_temp", string(in_temp), ...
    strcat(mdl, "/Battery"), "initial_air_temp", string(air_temp), ...
    strcat(mdl, "/Battery"), "initial_soc", string(initial_soc));

% Run simulation
out = sim(simin);
end


function rout = find_rout(out)
rout = (out.simout.sf_temp.Data(end) - out.ambient.Data(end)) / out.simout.q_gen.Data(end);
end
