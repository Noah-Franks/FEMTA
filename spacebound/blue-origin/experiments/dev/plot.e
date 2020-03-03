
define enter start;

Sensor test 1Hz {
  [calibrate   | sine, poly, raw, C   | 8, 8];
  [conversions | sine, raw, C | ];
  
  [calibrate   | cosine, poly, raw, C | 4, 10];
  [conversions | cosine, raw, C | ];
  
  [print | red, sine    | ];
  [print | blue, cosine | ];
}
