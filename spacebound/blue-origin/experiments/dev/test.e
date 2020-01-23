
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
  
  [print       | blue, sine       | 2, 3       ]
    //[print       | zero                     | 1          ]
  [print       | pink, cosine                   | 4          ]
    //[print       | green, identity          |            ]
  
    /*if (State start | cosine < 5mV | forever) {
      
      set pin 7 pos;
      set pin 7 neg after 6s;
      
      set pin 8 pos;
      set pin 8 neg after 4s;
      
      leave start;
      }*/
}

Sensor ds32 1Hz {
  [calibrate   | Time, poly, raw, s | 0.0009765625, 0.0]
  [conversions | Time, raw, s       |                  ]
  
  [print | maroon, Time      | 3 ]
    //[print | red, Temperature | 2 ]
}

Sensor ad15_gnd 25Hz {
  [calibrate   | A0, poly, raw, V   | 0.0001874287, -0.0009012627];
  [calibrate   | A1, poly, raw, V   | 0.0001874287, -0.0009012627];
  [calibrate   | A2, poly, raw, V   | 0.0001874287, -0.0009012627];
  [calibrate   | A3, poly, raw, V   | 0.0001874287, -0.0009012627];
  [conversions | A0, raw, V         |                            ];
  [conversions | A1, raw, V         |                            ];
  [conversions | A2, raw, V         |                            ];
  [conversions | A3, raw, V         |                            ];
  [print       | mint, A0           | 4                          ];
  //[print       | purple, A1, A2, A3 | 4, 4, 4                    ];
}

Sensor arm6 1Hz {
  
  [calibrate   | Temperature, poly, raw, C | 0.001, 0.000]
  [conversions | Temperature, raw, C       |             ]
  
  [calibrate   | Load, poly, raw, %        |       100, 0]
  [conversions | Load, raw, %              |             ]
  
  [print | salmon, Load        | 1 ];
  //[print |   pink, Memory      | 0 ];
  //[print | purple, Temperature | 2 ];
}
