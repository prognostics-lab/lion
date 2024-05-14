%% Data loading
% Load csv file
csv_dir = "data/240209_temptest_C6B2/TestData.csv";
opts = detectImportOptions(csv_dir);
opts = setvaropts(opts, "Seconds", "InputFormat", "MM/dd/uuuu HH:mm:ss.SSS");
csv_table = readtable(csv_dir, opts);

% Determine relevant parameters
SEGMENT = 4;
TIME_LIMIT = 50;

% Extract data
time = csv_table(:, "Seconds") - csv_table(1, "Seconds");
time = seconds(time.(1));
current = csv_table(:, "Current").(1);
voltage = csv_table(:, "Voltage").(1);
[time, power] = get_segment(time, current, voltage, SEGMENT, TIME_LIMIT);
amb_temp = 298 + 0 * sin(2 * pi * 0.0001 * time);

% Generate timeseries
power_profile = timeseries(power, time);
amb_profile = timeseries(amb_temp, time);
% time_delta = time(2) - time(1);
time_delta = 1;
end_time = time(end);

%% Set simulation parameters and run simulation
% Parameters
mdl = "sanity_check_sim";
REAL_PARAMS = dictionary("cp", 40, ...
    "cair", 100, ...
    "rin", 0.1, ...
    "rout", 0.1, ...
    "rair", 0.001, ...
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

out = evaluate_model(time_delta, end_time, mdl, REAL_PARAMS, 0.2);

out_table = table(out.tout, out.simout.sf_temp.Data, out.simout.air_temp.Data, ...
    out.simout.q_gen.Data, out.ambient.Data, ...
    VariableNames=["time", "sf_temp", "air_temp", "q_gen", "amb_temp"]);
writetable(out_table, "tests/sanity_check/sim.csv");

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
