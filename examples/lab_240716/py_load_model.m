function simin = py_load_model(mdl, time_delta, end_time, time, power, amb_temp)
disp("[DEBUG] Loading Simulink system");
load_system(mdl);
simin = Simulink.SimulationInput(mdl);

disp("[DEBUG] Assembling timeseries");
power_profile = timeseries(power', time');
amb_profile = timeseries(amb_temp', time');
ws = get_param(mdl, "ModelWorkspace");
assignin(ws, "power_profile", power_profile);
assignin(ws, "amb_profile", amb_profile);

disp("[DEBUG] Setting simulation command");
set_param(mdl, "SimulationCommand", "Update");
disp("[DEBUG] Setting model parameters");
disp("[DEBUG] end_time = " + end_time);
disp("[DEBUG] time_delta = " + time_delta);
simin = setModelParameter(simin, ...
    Solver="ode14x", ...
    StopTime=string(end_time), ...
    FixedStep=string(time_delta));
end
