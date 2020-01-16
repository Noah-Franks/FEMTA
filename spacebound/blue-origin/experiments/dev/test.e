
define enter start;

test 1Hz {
  [calibrate   | zero    , poly, raw, K   |    1.0, 8.0];
  [calibrate   | identity, poly, raw, s   |    1.0, 1.0];
  [calibrate   | sine    , poly, raw, kPa |    8.0, 0.0];
  [calibrate   | cosine  , poly, raw, V   |  0.008, 0.0];
  [calibrate   | random  , poly, raw, s   | 0.0001, 0.0];
  
  [conversions | zero    , raw, K         |            ];
  [conversions | identity, raw, s         |            ];
  [conversions | sine    , raw, kPa       |            ];
  [conversions | cosine  , raw, V, mV     |            ];
  [conversions | random  , raw, s, min    |            ];
  
  [print       | cyan                     |            ];
  
  
  //if (zero > 
}


/*ds32 1Hz {
  [calibrate   | Time, poly, raw, s | 0.0009765625, 0.0]
    [conversions | Time, raw, s       |                  ]
    }*/
