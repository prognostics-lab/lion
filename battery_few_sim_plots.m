figure;
hold on

plot(out.bat1.sf_temp - 273, "DisplayName", "Battery 1");
plot(out.bat2.sf_temp - 273, "DisplayName", "Battery 2", "LineStyle", ":");
plot(out.bat3.sf_temp - 273, "DisplayName", "Battery 3", "LineStyle", "--");

title("Surface temperature for all cells");
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
saveas(gcf, "img_raw/battery_few_sim_temps", "png");


figure;
hold on

plot(out.air1 - 273, "DisplayName", "Battery 1");
plot(out.air2 - 273, "DisplayName", "Battery 2", "LineStyle", ":");
plot(out.air3 - 273, "DisplayName", "Battery 3", "LineStyle", "--");

title("Air temperature for all cells");
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
saveas(gcf, "img_raw/battery_few_sim_air_temps", "png");