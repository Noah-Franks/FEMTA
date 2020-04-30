
define enter start;
define leave descent;
define leave descent2;
define leave apogee;
define leave rising;

Sensor ad15_vdd 20Hz {
  
  where A2 is collection;
  
  [calibrate | A2, poly, V, kPa |
    -0.00000805518095144006,
     0.000179617025688186,
    -0.00149263466053365,
     0.0137083951784765,
     3.84928878749102,
    -0.389869854380195
   ];
  
  [calibrate   | collection, poly, raw, V | 0.00041234314, -0.00198277794];
  [conversions | collection, raw, V, kPa  |                              ];
  [print       | white, collection         | 4                            ];
  
  if (State start | collection > 0kPa) {
    
    // close solenoid 2
    set pin 21 pos;
    set pin 20 neg;
    set pin 21 neg after 350ms;
    
    // open the vent valve
    set pin 17 pos;
    set pin  4 neg;
    set pin 17 neg after 350ms;
    
    // open solenoid 1
    set pin 26 pos;
    set pin 19 neg;
    set pin 26 neg after 350ms;
    
    leave start;
    enter descent after 1s;
  }
  
  if (State descent | collection < 30kPa) {
    
    // close solenoid 1
    set pin 19 pos;
    set pin 26 neg;
    set pin 19 neg after 350ms;
    
    leave descent;
    enter descent2;
  };

  if (State descent2 | collection < 20kPa) {
    
    // close the vent valve
    set pin  4 pos;
    set pin 17 neg;
    set pin  4 neg after 350ms;
    
    leave descent2;
    enter apogee after 20s;
  }
  
  if (State apogee | collection > 0kPa) {
    
    // open solenoid 2
    set pin 20 pos;
    set pin 21 neg;
    set pin 20 neg after 350ms;
    
    leave apogee;
    enter rising after 60s;
  }
  
  if (State rising; | collection > 0kPa) {
    
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

Sensor ad15_sda 20Hz {
  
  where A0 is water;
  where A2 is novec;
  
  [calibrate | water, poly, V, kPa |
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

  [calibrate   | water, poly, raw, V | 0.00041234314, -0.00198277794];
  [calibrate   | novec  , poly, raw, V    | 0.00041234314, -0.00198277794];
  [conversions | water, raw, V, kPa  |                              ];
  [conversions | novec  , raw, V, kPa     |                              ];
  [print       | blue, water        | 4                            ];
  [print       | mint, novec             | 4                            ];  
}
