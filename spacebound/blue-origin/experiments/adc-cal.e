
define enter start;

Sensor ds32 20Hz {
  [calibrate   | Time, poly, raw, ms | 0.9821424, 0 ];
  [conversions | Time, raw, ms, s    |              ];
  [print       | grey, Time          |            3 ];
}

Sensor ad15_gnd 20Hz {
  
  where A0 is measure;
  
  [print | purple, measure | 0];
  [print | red, A2         | 0];
}
