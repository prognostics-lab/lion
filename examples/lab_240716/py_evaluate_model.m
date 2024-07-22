function out = py_evaluate_model(mdl, simin, params, initial_conditions)
simin = py_set_model_parameters(mdl, simin, params, initial_conditions);
out = sim(simin);

figure
plot(out.power);

simout = out.simout;
disp(simout.sf_temp);
disp(simout.terminal_voltage);
out = [
    simout.terminal_voltage.Time,
    simout.terminal_voltage.Data,
    simout.oc_voltage.Data,
    simout.current.Data,
    simout.true_soc.Data,
    simout.nominal_soc.Data,
    simout.usable_cap.Data,
    simout.q_gen.Data,
    simout.in_temp.Data,
    simout.air_temp.Data,
    simout.sf_temp.Data
];
end