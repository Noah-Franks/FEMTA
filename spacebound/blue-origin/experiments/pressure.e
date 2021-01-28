/**
 * @Authors Noah, Mark
 * @Experimenters Tristan, Mark, Ani, Tony
 * @Experiment_Date 11/05/19
 * @Compiler_Version Release 1.0
 * @Licence GPLv3
 * 
 * @Description Tests a diaphragm, determining if it holds pressure, leaks,
 *              or bursts under vacuum conditions
 **/

define enter start;

ad15_gnd 100Hz {}

ad15_vdd 100Hz {

  [calibrate | A2, poly, V, kPa |    // we expect 11.3v at 1atm | V->kPa
     -0.00000805518095144006,        // degrees are from high to low
      0.000179617025688186,
     -0.00149263466053365,
      0.0137083951784765,
      3.84928878749102,
     -0.389869854380195
   ];
  
  [calibrate   | A2, poly, raw, V | 0.00041234314, -0.00198277794];
  [conversions | A2, raw, V, kPa  |                              ];
  
  [print]
}

ad15_sda 100Hz {

  /*
  if (A2 < 21kPa | single) {
    set pin 26 neg;
    set pin 19 pos;
    set pin 17 neg;
    set pin 4 pos;
    set pin 19 neg after 350ms;
    set pin 4 neg after 350ms;
    }*/

  [calibrate | A2, poly, V, kPa |    // we expect 11.3v at 1atm | V->kPa
     -0.00000805518095144006,        // degrees are from high to low
      0.000179617025688186,
     -0.00149263466053365,
      0.0137083951784765,
      3.84928878749102,
     -0.389869854380195
   ];
  
  [calibrate   | A2, poly, raw, V | 0.00041234314, -0.00198277794];
  [conversions | A2, raw, V, kPa  |                              ];
  //[smooth      | A2               | 0.1                          ];

  [calibrate | A0, poly, V, kPa |    // we expect 11.3v at 1atm | V->kPa
     -0.00000805518095144006,        // degrees are from high to low
      0.000179617025688186,
     -0.00149263466053365,
      0.0137083951784765,
      3.84928878749102,
     -0.389869854380195
   ];

  [calibrate   | A0, poly, raw, V | 0.00041234314, -0.00198277794];
  [conversions | A0, raw, V, kPa  |                              ];
  // [smooth      | A0               | 0.1                          ];

  /*
  [calibrate | A1, poly, V, kPa |    // we expect 11.3v at 1atm | V->kPa
     -0.00000805518095144006,        // degrees are from high to low
      0.000179617025688186,
     -0.00149263466053365,
      0.0137083951784765,
      3.84928878749102,
     -0.389869854380195
   ];

  [calibrate   | A1, poly, raw, V | 0.00041234314, -0.00198277794];
  [conversions | A1, raw, V, kPa  |                              ];
  // [smooth      | A1               | 0.1                          ];
  */
  
  [print]
}

ds32 1Hz {
  [calibrate   | Time, poly, raw, s | 0.0009765625, 0.0]
  [conversions | Time, raw, s       |                  ]
}
