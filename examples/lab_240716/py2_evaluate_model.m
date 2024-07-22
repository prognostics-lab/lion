cp = params.cp;
cair = params.cair;
rin = params.rin;
rout = params.rout;
rair = params.rair;
initial_soc = initial_conditions.initial_soc;
in_temp = initial_conditions.initial_in_temp;
simin = setBlockParameter(simin, ...
    strcat(mdl, "/Battery"), "th_cp", string(cp), ...
    strcat(mdl, "/Battery"), "th_cair", string(cair), ...
    strcat(mdl, "/Battery"), "th_rin", string(rin), ...
    strcat(mdl, "/Battery"), "th_rout", string(rout), ...
    strcat(mdl, "/Battery"), "th_rair", string(rair), ...
    strcat(mdl, "/Battery"), "initial_in_temp", string(in_temp), ...
    strcat(mdl, "/Battery"), "initial_soc", string(initial_soc));

out = sim(simin);

figure
plot(out.power)

simout = out.simout;
disp(simout.sf_temp);
disp(simout.terminal_voltage);
sf_temp = simout.sf_temp.Data;
air_temp = simout.air_temp.Data;
% out = [
%     simout.terminal_voltage.Time,
%     simout.terminal_voltage.Data,
%     simout.oc_voltage.Data,
%     simout.current.Data,
%     simout.true_soc.Data,
%     simout.nominal_soc.Data,
%     simout.usable_cap.Data,
%     simout.q_gen.Data,
%     simout.in_temp.Data,
%     simout.air_temp.Data,
%     simout.sf_temp.Data
% ];