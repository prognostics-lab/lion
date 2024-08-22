function result = py_evaluate_model(mdl, simin, params, initial_conditions, constant_parameters)
simin = py_set_model_parameters(mdl, simin, params, initial_conditions, constant_parameters);
out = sim(simin);
result = [out.simout.in_temp.Time, out.simout.in_temp.Data, out.simout.sf_temp.Data, out.simout.true_soc.Data];
end