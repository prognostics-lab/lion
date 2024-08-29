#define GND_PIN A0

// User parameters
const int NUM_CELLS = 1;
const int PINS_PER_CELL = 4;
const int SENSOR_DELAY_MILS = 10;
const int LOOP_DELAY_MILS = 1000;

// Fixed parameters
const double RESISTOR_FIXED = 100000.0;
const double VOLTAGE_IN = 1023.0;
const double RT_LUT[] = {
    11093800.0, 10156100.0, 9311690.0, 8549810.0, 7861200.0, 7237730.0,
    6672310.0,  6158700.0,  5691440.0, 5265690.0, 4877200.0, 4522230.0,
    4197420.0,  3899830.0,  3626830.0, 3376080.0, 3145490.0, 2933200.0,
    2737530.0,  2556980.0,  2390220.0, 2236030.0, 2093330.0, 1961130.0,
    1838540.0,  1724780.0,  1619100.0, 1520850.0, 1429440.0, 1344320.0,
    1265000.0,  1191020.0,  1121980.0, 1057500.0, 997241.0,  940885.0,
    888148.0,   838764.0,   792494.0,  749115.0,  708422.0,  670228.0,
    634359.0,   600657.0,   568973.0,  539171.0,  511127.0,  484723.0,
    459851.0,   436413.0,   414316.0,  393473.0,  373806.0,  355239.0,
    337705.0,   321140.0,   305482.0,  290679.0,  276676.0,  263427.0,
    250886.0,   239012.0,   227764.0,  217106.0,  207005.0,  198530.0,
    188343.0,   179724.0,   171545.0,  163780.0,  156407.0,  149403.0,
    142748.0,   136423.0,   130410.0,  124692.0,  119253.0,  114078.0,
    109152.0,   104464.0,   100000.0,  95747.0,   91697.0,   87837.0,
    84157.0,    80650.0,    77305.0,   74115.0,   71072.0,   68167.0,
    65395.0,    62749.0,    60222.0,   57809.0,   55503.0,   53300.0,
    51195.0,    49183.0,    47259.0,   45419.0,   43659.0,   41975.0,
    40364.0,    38822.0,    37346.0,   35840.0,   34580.0,   33284.0,
    32043.0,    30853.0,    29713.0,   28620.0,   27573.0,   26568.0,
    25605.0,    24681.0,    23794.0,   22944.0,   22127.0,   21344.0,
    20592.0,    19869.0,    19175.0,   18509.0,   17869.0,   17253.0,
    16662.0,    16094.0,    15548.0,   15022.0,   14517.0,   14031.0,
    13564.0,    13114.0,    12681.0,   12265.0,   11864.0,   11478.0,
    11106.0,    10748.0,    10404.0,   10071.0,   9751.0,    9443.0,
    9146.0,     8859.0,     8583.0,    8316.0,    8060.0,    7812.0,
    7573.0,     7342.0,     7120.0,    6905.0,    6698.0,    6498.0,
    6304.0,     6118.0,     5937.0,    5763.0,    5595.0,    5433.0,
    5276.0,     5124.0,     4977.0,    4836.0,    4698.0,    4566.0,
    4438.0,     4314.0,     4194.0,    4077.0,    3965.0,    3856.0,
    3751.0,     3649.0,     3551.0,    3455.0,    3363.0,    3273.0,
    3186.0,
};
const int RT_LUT_LEN = 181;
// const int RT_LUT_MIN = -55 + 273;
const int RT_LUT_MIN = -55;

// Discharges capacitor inside ADC, should be used in between sequential reads
void reset_adc() {
  // The pin GND_PIN should be externally connected to ground
  (void)analogRead(GND_PIN);
  delay(SENSOR_DELAY_MILS);
}

unsigned int find_index(double resistance) {
  const double *lut = RT_LUT;
  int idx = 0;
  while (*lut++ > resistance && idx < RT_LUT_LEN - 1) {
    idx++;
  }
  return idx;
}

int calculate_temperature(double resistance) {
  int idx = (int)find_index(resistance);
  if (idx == 0)
    return RT_LUT_MIN;

  double threshold = (RT_LUT[idx - 1] + RT_LUT[idx]) / 2.0;
  if (resistance > threshold) {
    idx--;
  }
  return RT_LUT_MIN + idx;
}

double calculate_resistance(int analog_voltage) {
  return RESISTOR_FIXED / (VOLTAGE_IN / (double)analog_voltage - 1.0);
}

/* Data loop */

void setup() {
  Serial.begin(9600);
}

void loop() {
  int pin = A1;
  for (int i = 0; i < NUM_CELLS; i++) {
    Serial.print(i);
    Serial.print(",");
    for (int j = 0; j < PINS_PER_CELL; j++) {
      int analog_voltage = analogRead(pin);
      double resistance = calculate_resistance(analog_voltage);
      int temperature = calculate_temperature(resistance); // in celsius
      Serial.print(resistance);
      Serial.print(",");
      Serial.print(temperature);
      if (j != PINS_PER_CELL - 1) {
        Serial.print(",");
      }
      reset_adc();
      pin++;
    }
    Serial.println();
  }
  delay(LOOP_DELAY_MILS);
}
