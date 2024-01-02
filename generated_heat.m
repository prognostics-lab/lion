currs = out.simout.current.Data;
heats = out.simout.q_gen.Data;

scatter(1e3 * currs, 1e3 * heats);
grid("on");

cutoff = -100;
for i = 1:size(currs, 1)
    if currs(i) > cutoff && heats(i) <= 0
        cutoff = currs(i);
    end
end

temperature = 298;
resistance = 0.12;
ehc = 4.4780091370741066e-05;

current_theory = temperature * ehc / resistance;
xline([1e3 * current_theory 1e3 * cutoff], "--", {"Theoretical (" + 1e3 * current_theory + " mA)", "Simulation (" + 1e3 * cutoff + " mA)"});
yline(0);

xlabel("Current (mA)");
ylabel("q_{gen} (mW)");