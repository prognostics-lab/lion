figure;
hold on

plot(out.simout.sf_temp - 273, "DisplayName", "Surface T°");
plot(out.air - 273, "DisplayName", "Air T°");

title("Surface and air temperature");
xlabel("Time (s)");
ylabel("Temperature (°C)");
grid("on");
legend;

set(gca,'units','centimeters')
pos = get(gca,'Position');
ti = get(gca,'TightInset');
set(gcf, 'PaperUnits','centimeters');
set(gcf, 'PaperSize', [pos(3)+ti(1)+ti(3) pos(4)+ti(2)+ti(4)]);
set(gcf, 'PaperPositionMode', 'manual');
set(gcf, 'PaperPosition',[0 0 pos(3)+ti(1)+ti(3) pos(4)+ti(2)+ti(4)]);
saveas(gcf, "img_raw/cell_sim_temps", "png");


figure;
hold on

plot(1e3 * out.simout.q_gen);

title("Generated heat");
xlabel("Time (s)");
ylabel("Heat (mW)");
grid("on");

set(gca,'units','centimeters')
pos = get(gca,'Position');
ti = get(gca,'TightInset');
set(gcf, 'PaperUnits','centimeters');
set(gcf, 'PaperSize', [pos(3)+ti(1)+ti(3) pos(4)+ti(2)+ti(4)]);
set(gcf, 'PaperPositionMode', 'manual');
set(gcf, 'PaperPosition',[0 0 pos(3)+ti(1)+ti(3) pos(4)+ti(2)+ti(4)]);
saveas(gcf, "img_raw/cell_sim_qgen", "png");


figure;
hold on

plot(out.input);

title("Power demand");
xlabel("Time (s)");
ylabel("Power (W)");
grid("on");

set(gca,'units','centimeters')
pos = get(gca,'Position');
ti = get(gca,'TightInset');
set(gcf, 'PaperUnits','centimeters');
set(gcf, 'PaperSize', [pos(3)+ti(1)+ti(3) pos(4)+ti(2)+ti(4)]);
set(gcf, 'PaperPositionMode', 'manual');
set(gcf, 'PaperPosition',[0 0 pos(3)+ti(1)+ti(3) pos(4)+ti(2)+ti(4)]);
saveas(gcf, "img_raw/cell_sim_power", "png");
