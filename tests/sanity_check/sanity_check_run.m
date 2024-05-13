%% Data loading
% Load csv file
csv_dir = "data/240209_temptest_C6B2/TestData.csv";
opts = detectImportOptions(csv_dir);
opts = setvaropts(opts, "Seconds", "InputFormat", "MM/dd/uuuu HH:mm:ss.SSS");
table = readtable(csv_dir, opts);

% Determine relevant parameters
SEGMENT = 3;
TIME_LIMIT = 50;

% Extract data
time = table(:, "Seconds") - table(1, "Seconds");
time = seconds(time.(1));
current = table(:, "Current").(1);
voltage = table(:, "Voltage").(1);
[time, power] = get_segment(time, current, voltage, SEGMENT, TIME_LIMIT);

%- Generated data -%
% end_time = 10000;
% SAMPLES_PER_SECOND = 100;
% time = linspace(0, end_time, SAMPLES_PER_SECOND * end_time);
% power = 100 * sin(2 * pi * 0.001 * time);

%% Generate power profile timeseries
power_profile = timeseries(power, time);
end_time = time(end);

%% Set simulation parameters and run simulation
% Parameters
mdl = "sanity_check_sim";
% open_system(mdl);
simin = Simulink.SimulationInput(mdl);
simin = setModelParameter(simin, ...
    StopTime=string(end_time));

% Run simulation
out = sim(simin);

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