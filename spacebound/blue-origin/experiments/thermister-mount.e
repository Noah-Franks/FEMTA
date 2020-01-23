
define enter start;

Sensor test 1/1Hz {
  [calibrate   | sine    , poly, raw, kPa | 8.0, 0.0];
  [conversions | sine    , raw, kPa       |         ];
  [print       | pink, sine               | 2       ];
}

Sensor ad15_vdd 20Hz {  
  
  [calibrate | A2, poly, V, kPa |
    -0.00000805518095144006,
     0.000179617025688186,
    -0.00149263466053365,
     0.0137083951784765,
     3.84928878749102,
    -0.389869854380195
   ];
  
  [calibrate   | A2, poly, raw, V | 0.00041234314, -0.00198277794];
  [conversions | A2, raw, V, kPa  |                              ];
  [print       | blue, A2         | 4                            ];
}

Sensor ad15_sda 20Hz {
  
  [calibrate | A0, poly, V, kPa |
    -0.00000805518095144006,
     0.000179617025688186,
    -0.00149263466053365,
     0.0137083951784765,
     3.84928878749102,
    -0.389869854380195
   ];
  
  [calibrate | A2, poly, V, kPa |
    -0.00000805518095144006,
     0.000179617025688186,
    -0.00149263466053365,
     0.0137083951784765,
     3.84928878749102,
    -0.389869854380195
   ];
  
  [calibrate   | A0, poly, raw, V | 0.00041234314, -0.00198277794];
  [calibrate   | A2, poly, raw, V | 0.00041234314, -0.00198277794];
  [conversions | A0, raw, V, kPa  |                              ];
  [conversions | A2, raw, V, kPa  |                              ];
  [print       | white, A0        | 4                            ];
  [print       | green, A2        | 4                            ];

  if (State start | A0 < 20kPa | forever) {
    
    // close the vent valve
    set pin  4 pos;
    set pin 17 neg;
    set pin  4 neg after 350ms;
    
    // close solenoid 1
    set pin 19 pos;
    set pin 26 neg;
    set pin 19 neg after 350ms;
    
    leave start;
  }
}
