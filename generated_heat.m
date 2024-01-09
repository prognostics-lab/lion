figure;

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

set(gca,'units','centimeters')
pos = get(gca,'Position');
ti = get(gca,'TightInset');
set(gcf, 'PaperUnits','centimeters');
set(gcf, 'PaperSize', [pos(3)+ti(1)+ti(3) pos(4)+ti(2)+ti(4)]);
set(gcf, 'PaperPositionMode', 'manual');
set(gcf, 'PaperPosition',[0 0 pos(3)+ti(1)+ti(3) pos(4)+ti(2)+ti(4)]);
saveas(gcf, "img_raw/q_gen_experiment", "png");