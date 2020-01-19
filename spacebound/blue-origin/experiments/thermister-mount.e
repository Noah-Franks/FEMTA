
define enter start;

Sensor ad15_gnd 20Hz {

  /*if (State start | A0 < 20kPa) {
    
    // water valve
    set pin 26 neg;
    set pin 19 pos;
    set pin 19 neg after 350ms;
    
    // hfe valve
    set pin 17 neg;
    set pin  4 pos;
    set pin  4 neg after 350ms;
    
    leave start;
  }*/


  [calibrate | A0, poly, V, kPa |
      -0.00000805518095144006,
       0.000179617025688186,
      -0.00149263466053365,
       0.0137083951784765,
       3.84928878749102,
      -0.389869854380195
  ];
  
  [calibrate | A2, poly, V, kPa |
      -0.00000805518095144006,
       0.000179617025688186,
      -0.00149263466053365,
       0.0137083951784765,
       3.84928878749102,
      -0.389869854380195
  ];
  
  
  [calibrate   | A0, poly, raw, V | 0.00041234314, -0.00198277794];
  [calibrate   | A2, poly, raw, V | 0.00041234314, -0.00198277794];
  
  [conversions | A0, raw, V, kPa  |                              ];
  [conversions | A2, raw, V, kPa  |                              ];
}

Sensor ad15_vdd 20Hz {

  

  [calibrate | A2, poly, V, kPa |
      -0.00000805518095144006,
       0.000179617025688186,
      -0.00149263466053365,
       0.0137083951784765,
       3.84928878749102,
       -0.389869854380195
  ];
  
  [calibrate   | A2, poly, raw, V | 0.00041234314, -0.00198277794];
  [conversions | A2, raw, V, kPa  |                              ];
}

Sensor ad15_scl 20Hz {}
Sensor ad15_sda 20Hz {}
