
define enter start;

Sensor arm6 1Hz {
  
  where Temperature is cpu;
  
  [calibrate   | Temperature, poly, raw, C | 0.001, 0.000]
  [conversions | Temperature, raw, C, F    |             ]
  
  [calibrate   | Load, poly, raw, %        |       100, 0]
  [conversions | Load, raw, %              |             ]
  
  [print       | red, cpu                  |         1, 2];
}

Sensor ad15_vdd 20Hz {

  where A2 is water;
  
  [calibrate | A2, poly, V, kPa |
    -0.00000805518095144006,
     0.000179617025688186,
    -0.00149263466053365,
     0.0137083951784765,
     3.84928878749102,
    -0.389869854380195
   ];
  
  [calibrate   | water, poly, raw, V | 0.00041234314, -0.00198277794];
  [conversions | water, raw, V, kPa  |                              ];
  [print       | blue, water         | 4                            ];
}

Sensor ad15_sda 20Hz {
  
  where A0 is ambient;
  where A2 is novec;
  
  [calibrate | ambient, poly, V, kPa |
    -0.00000805518095144006,
     0.000179617025688186,
    -0.00149263466053365,
     0.0137083951784765,
     3.84928878749102,
    -0.389869854380195
   ];
  
  [calibrate | novec, poly, V, kPa |
    -0.00000805518095144006,
     0.000179617025688186,
    -0.00149263466053365,
     0.0137083951784765,
     3.84928878749102,
    -0.389869854380195
   ];
  
  [calibrate   | ambient, poly, raw, V | 0.00041234314, -0.00198277794];
  [calibrate   | novec  , poly, raw, V | 0.00041234314, -0.00198277794];
  [conversions | ambient, raw, V, kPa  |                              ];
  [conversions | novec  , raw, V, kPa  |                              ];
  [print       | white, ambient        | 4                            ];
  [print       | green, novec          | 4                            ];
  
  if (State start | ambient < 20kPa) {
    
    // close the vent valve
    set pin  4 pos;
    set pin 17 neg;
    set pin  4 neg after 350ms;
    
    // close solenoid 1
    set pin 19 pos;
    set pin 26 neg;
    set pin 19 neg after 350ms;
    
    leave start;
    enter rising after 350ms;
  }
  
  if (State rising | ambient > 25kPa) {
    
    // open the vent valve
    set pin 17 pos;
    set pin  4 neg;
    set pin 17 neg after 350ms;
    
    // open solenoid 1
    set pin 26 pos;
    set pin 19 neg;
    set pin 26 neg after 350ms;
    
    leave rising;
  }
}
