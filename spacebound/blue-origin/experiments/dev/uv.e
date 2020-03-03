
define enter start;

Sensor ad15_gnd 1Hz {
  
  where A0 is photo;
  
  [calibrate   | photo, poly, raw, V | 0.0001874287, -0.0009012627];    // raw -> adc
  //  [calibrate   | photo, poly, V, %   |                       10, 0];    // adc -> photodiode
  [conversions | photo, raw, V    |                            ];
  [print       | magenta, photo, A1, A2, A3      |                           4];
}

Sensor ds32 1Hz {
  
  [calibrate   | Time, poly, raw, s       | 0.0009765625, 0.0];
  [conversions | Time, raw, s             |                  ];
  [print       | grey, Time               |                2 ];
}
