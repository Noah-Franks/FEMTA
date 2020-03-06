
define enter start;

Sensor test 1Hz {
  [calibrate   | sine,   poly, raw, C     |  1,    0];
  [calibrate   | cosine, poly, raw, C     |  1,    0];
  [calibrate   | sine,   poly, C, kPa     | 20, 22.5];
  [calibrate   | cosine, poly, C, kPa     | 20, 22.5];
  
  [conversions | sine,   raw, C, kPa      |         ];
  [conversions | cosine, raw, C, kPa      |         ];
  
  [calibrate   | identity, poly, raw, kPa |  1, 0.02];
  [conversions | identity, raw, kPa       |         ];
  
  [print       | mint, sine               |         ];
  [print       | orange, cosine           |         ];
  [print       | pink, identity           |         ];
}
