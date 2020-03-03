
define enter start;
define leave other;
define leave never;

Sensor test 1/1Hz {
  [calibrate   | zero    , poly, raw, K   |     1.0, 8.0];
  [calibrate   | identity, poly, raw, min |   -1.05, 1.0];
  [calibrate   | sine    , poly, raw, kPa |     8.0, 0.0];
  [calibrate   | cosine  , poly, raw, V   |   0.008, 0.0];
//[calibrate   | random  , poly, raw, s   |  0.0001, 0.0];
  
  [calibrate   | sine    , poly, kPa, F |  2.0, 1.0];
  
  // sanity check: no 999 should show up
//[calibrate   | random, poly, raw, min   |         999];
  
  [conversions | zero    , raw, K         |            ];
  [conversions | identity, raw, min       |            ];
  [conversions | sine    , raw, kPa, F, C |            ];
  [conversions | cosine  , raw, V, mV     |            ];
//[conversions | random  , raw, s, min    |            ];
  
  [calibrate   | zero    , poly, raw, kPa |    1.0, 8.0];
  //[conversions | sine    , raw, raw       |            ];
  
  [smooth      | random                   | 0.01       ]
  
  [print       | blue, sine               | 2, 3       ]
//[print       | zero                     | 1          ]
//[print       | pink, cosine             | 4          ]
//[print       | green, identity          |            ]
  
  where zero is dope;
}

Sensor ds32 1Hz {
  
  where Time is Layer;
  //where Temperature is Temp;
  
  [calibrate   | Layer, poly, raw, s       | 0.0009765625, 0.0]
  [conversions | Layer, raw, s             |                  ]
  
  [print | maroon, Layer      | 3 ]
  //[print | red, Temperature | 2 ]
  
  if (State start | Layer > 0.1min | forever) {
    
    set pin 7 pos;
    set pin 7 neg after 3s;
    
    set pin 8 pos;
    set pin 8 neg after 2s;
    
    leave start;
    enter start after 4s;
  }
}

Sensor ad15_gnd 25Hz {
  
  // alias channels with better names
  where A0 is water_pressure;
  where A1 is novec_pressure;
  where A2 is water_temperature;
  where A3 is novec_temperature;
  
  // alternatively, use the 'alias' tag
  /*[alias       | A0, water_pressure              |                            ];
  [alias       | A1, novec_pressure              |                            ];
  [alias       | A2, water_temperature           |                            ];
  [alias       | A3, novec_temperature           |                            ];*/
  
    [calibrate   | water_pressure   , poly, raw, V | 0.0001874287, -0.0009012627];
  [calibrate   | novec_pressure   , poly, raw, V | 0.0001874287, -0.0009012627];
  [calibrate   | water_temperature, poly, raw, V | 0.0001874287, -0.0009012627];
  [calibrate   | novec_temperature, poly, raw, V | 0.0001874287, -0.0009012627];
  [conversions | water_pressure   , raw, V       |                            ];
  [conversions | novec_pressure   , raw, V       |                            ];
  [conversions | water_temperature, raw, V       |                            ];
  [conversions | novec_temperature, raw, V       |                            ];
  [print       | mint, water_pressure            | 4                          ];
  [print       | purple, A1, A2, A3              | 4, 4, 4                    ];
}

Sensor arm6 1Hz {
  
  where Load is cpu;
  
  [calibrate   | Temperature, poly, raw, C | 0.001, 0.000]
  [conversions | Temperature, raw, C, F    |             ]
  
  [calibrate   | Load, poly, raw, %        |       100, 0]
  [conversions | Load, raw, %              |             ]
  
  [print | salmon, Load                    |         1, 2];
//[print |   pink, Memory      | 0 ];
//[print | purple, Temperature | 2 ];
}
