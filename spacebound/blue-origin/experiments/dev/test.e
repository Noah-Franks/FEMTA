
define enter start;
define leave other;
define leave never;

Sensor test 1/1Hz {
  [calibrate   | zero    , poly, raw, K   |    1.0, 8.0];
/*[calibrate   | identity, poly, raw, min |    1.0, 1.0];
  [calibrate   | sine    , poly, raw, kPa |    8.0, 0.0];
  [calibrate   | cosine  , poly, raw, V   |  0.008, 0.0];*/
//[calibrate   | random  , poly, raw, s   | 0.0001, 0.0];
  
  // sanity check: no 999 should show up
//[calibrate   | random, poly, raw, min   |         999];
  
  [conversions | zero    , raw, K         |            ];
/*[conversions | identity, raw, min       |            ];
  [conversions | sine    , raw, kPa       |            ];
  [conversions | cosine  , raw, V, mV     |            ];*/
//[conversions | random  , raw, s, min    |            ];
  
  [calibrate   | zero    , poly, raw, kPa |    1.0, 8.0];
  [conversions | sine    , raw, raw       |            ];
  
  [smooth      | random                   | 0.01       ]
  
  [print       | blue, sine, cosine, zero | 2, 4       ]
  [print       | zero                     | 1          ]
  [print       | green, identity          |            ]
  [print       | purple, random           | 0          ]
    
  /*if (State start | zero > 0K | forever) {
      leave start;
      
      enter start after 4s;
      enter other after 4s;
      enter never after 6s;    // never happens due to resetting
      
      set pin 7 neg;
      set pin 7 pos after 2s;
  }*/
}


/*Sensor ds32 1Hz {
  [calibrate   | Time, poly, raw, s | 0.0009765625, 0.0]
  [conversions | Time, raw, s       |                  ]
}*/
