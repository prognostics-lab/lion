%% Parameters
% Simulation parameters
T_AMB = 2.98e2; % K, 25 °C
T_REF = 2.73e2; % K
ALPHA = -5.738; % K
BETA = 2.099e2; % K

% Open-circuit voltage curve parameters
% OCV_1 = -5.863e-1;
% OCV_2 = 21.9;
% OCV_3 = 3.414;
% OCV_4 = 1.102e-1;
% OCV_5 = -1.718e-1;
% OCV_6 = 8.0e-3;
OCV_V0 = 26.792;
OCV_VL = 3.971;
OCV_ALPHA = -2.631;
OCV_BETA = 0.508;
OCV_GAMMA = 0.556;

% Battery parameters
HEAT_CAPACITY = 105.3; % J K-1
R_IN = 1.8; % K W-1
R_OUT = 15.8; % K W-1
INTERNAL_RESISTANCE = 55.3e-3; % Ohm
% t_constant_ = HEAT_CAPACITY * (R_IN + R_OUT);
NOMINAL_CAPACITY = 4; % A h
UPPER_VOLT_CF = 4.2; % V
LOWER_VOLT_CF = 2.75; % V
