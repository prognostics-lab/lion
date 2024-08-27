function resistance = polarization_resistance_deffuzify(memberships, soc)
coeffs = [
    0, 0, 0, 0;
    0, 0, 0, 0;
    0, 0, 0, 0;
    0, 0, 0, 0;
    0.3084, -0.2579, -0.05083, 0.1317;
    0.1141, -0.07709, -0.05706, 0.0958;
    0.04612, -0.008633, -0.05782, 0.07868;
    0.0061, 0.04202, -0.07066, 0.07218;
];
num = 0;
for i = 1:length(memberships)
    num = num + memberships(i) * polyval(coeffs(i, :), soc);
end
resistance = num / sum(memberships);
end
