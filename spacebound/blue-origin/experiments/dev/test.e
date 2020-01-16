
define enter start;

test 1Hz {
  [calibrate   | zero    , poly, raw, K   |    1.0, 8.0];
  [calibrate   | identity, poly, raw, min |    1.0, 1.0];
  [calibrate   | sine    , poly, raw, kPa |    8.0, 0.0];
  [calibrate   | cosine  , poly, raw, V   |  0.008, 0.0];
  [calibrate   | random  , poly, raw, s   | 0.0001, 0.0];
  
  // sanity check: no 999 should show up
  [calibrate   | random, poly, raw, min   |         999];
  
  [conversions | zero    , raw, K         |            ];
  [conversions | identity, raw, min       |            ];
  [conversions | sine    , raw, kPa       |            ];
  [conversions | cosine  , raw, V, mV     |            ];
  [conversions | random  , raw, s, min    |            ];
  
  [print       | green                    |            ];
  
  if (State start | zero > 0K | forever) {
      leave start;
      enter start after 4s;
  }
}


/*ds32 1Hz {
  [calibrate   | Time, poly, raw, s | 0.0009765625, 0.0]
    [conversions | Time, raw, s       |                  ]
    }*/
