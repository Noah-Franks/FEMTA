/**
 * @Authors Noah
 * @Experimenters Sam, Noah, Tony
 * @Experiment_Date 10//19
 * @Compiler_Version Release 1.0
 * @Licence GPLv3
 * 
 * @Description Tests a diaphragm, determining if it holds pressure, leaks, or bursts
 **/

define enter start;

ad15_vdd 25Hz {};
ad15_gnd 25Hz {};
ad15_scl 25Hz {};

ad15_sda 25Hz {
  
  /*if (A2 < 10kPa | single) {
    set pin 19 neg;
    set pin 26 pos;
    set pin 26 neg after 350ms;
    }*/
  
  [calibrate | A2, poly, V, kPa |    // we expect 11.3v at 1atm | V->kPa
     -0.00000805518095144006,        // degrees are from high to low
      0.000179617025688186,
     -0.00149263466053365,
      0.0137083951784765,
      3.84928878749102,
     -0.389869854380195
   ];
  
  [calibrate | A0, poly, V, kPa |    // we expect 11.3v at 1atm | V->kPa
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
  
  [calibrate   | A0, poly, raw, V | 0.00041234314, -0.00198277794];
  [conversions | A0, raw, V, kPa  |                              ];
  //[smooth      | A0               | 0.1                          ];
  
  [print       | gray             |                            ]
}

ds32 1Hz {
  [calibrate   | Time, poly, raw, s | 0.0009765625, 0.0]
  [conversions | Time, raw, s       |                  ]
}
