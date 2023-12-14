%% Power input
figure;
plot(out.input);
title("Power demand for the cell");
xlabel("Time (s)");
ylabel("Power (W)");
% ylim([-0.01, 0.11]);
grid("on");

set(gca,'units','centimeters')
pos = get(gca,'Position');
ti = get(gca,'TightInset');
set(gcf, 'PaperUnits','centimeters');
set(gcf, 'PaperSize', [pos(3)+ti(1)+ti(3) pos(4)+ti(2)+ti(4)]);
set(gcf, 'PaperPositionMode', 'manual');
set(gcf, 'PaperPosition',[0 0 pos(3)+ti(1)+ti(3) pos(4)+ti(2)+ti(4)]);
saveas(gcf, "img_raw/power", "pdf");

%% Simulation results
figure("Position", [0, 0, 10000, 2000]);
subplot(2, 4, 1);
plot(out.simout.terminal_voltage);
title("Terminal voltage");
xlabel("Time (s)");
ylabel("Voltage (V)");
grid("on");

subplot(2, 4, 2);
plot(out.simout.current);
title("Current");
xlabel("Time (s)");
ylabel("Current (A)");
grid("on");

subplot(2, 4, 3);
plot(100 * out.simout.nominal_soc);
title("Nominal SoC");
xlabel("Time (s)");
ylabel("SoC_0 (%)");
grid("on");

subplot(2, 4, 4);
plot(out.simout.usable_cap / 3.6);
title("Usable capacity");
xlabel("Time (s)");
ylabel("Capacity (mAh)");
grid("on");

subplot(2, 4, 5);
plot(out.simout.oc_voltage);
title("Open circuit voltage");
xlabel("Time (s)");
ylabel("Voltage (V)");
grid("on");

subplot(2, 4, 6);
plot(100 * out.simout.true_soc);
title("Usable SoC");
xlabel("Time (s)");
ylabel("SoC (%)");
grid("on");

subplot(2, 4, 7);
plot(out.simout.sf_temp - 273);
title("Surface temperature");
xlabel("Time (s)");
ylabel("Temperature (°C)");
grid("on");

subplot(2, 4, 8);
plot(out.simout.q_gen);
title("Generated heat");
xlabel("Time (s)");
ylabel("Heat (W)");
grid("on");

set(gca,'units','centimeters')
pos = get(gca,'Position');
ti = get(gca,'TightInset');
set(gcf, 'PaperUnits','centimeters');
set(gcf, 'PaperSize', [pos(3)+ti(1)+ti(3) pos(4)+ti(2)+ti(4)]);
set(gcf, 'PaperPositionMode', 'manual');
set(gcf, 'PaperPosition',[0 0 pos(3)+ti(1)+ti(3) pos(4)+ti(2)+ti(4)]);
saveas(gcf, "img_raw/output", "pdf");
