
define enter start;

Sensor test 10Hz {
  [calibrate   | sine,   poly, raw, C   |  1, 0];
  [calibrate   | cosine, poly, raw, C   |  1, 0];
  [calibrate   | sine,   poly, C, kPa   |  8, 8];
  [calibrate   | cosine, poly, C, kPa   |  6, 8];
               
  [conversions | sine,   raw, C, kPa         |     ];
  [conversions | cosine, raw, C, kPa         |     ];
  
  [calibrate   | identity, poly, raw, kPa | 1, 0.02];
  [conversions | identity, raw, kPa       |        ];
  
  [print       | mint, sine               |     ];
  [print       | blue, cosine             |     ];
  [print       | pink, identity           |     ];
}
