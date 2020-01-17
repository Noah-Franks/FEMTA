
define enter start;

ad15_gnd 1Hz {
  [print | green | ];
}
ad15_vdd 1Hz {}
ad15_sda 1Hz {}
ad15_scl 1Hz {}

//ds18 1Hz { [print | gray | ]; }

ds32 1Hz {
  
  if (State start | Time > 0.1s | single) {
      set pin 27 pos;
      leave start;
  }
  
  [calibrate   | Time, poly, raw, s | 0.0009765625, 0.0];
  [conversions | Time, raw, s       |                  ];
  //  [print : gray : 5Hz];
}
