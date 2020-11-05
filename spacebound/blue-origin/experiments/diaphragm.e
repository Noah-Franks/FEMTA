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
define leave descent;
define leave ascent;
define leave end;

ad15_vdd 100Hz {};

ad15_sda 100Hz {

  if (State start | A2 > 22kPa) {
    set pin 19 pos;
    set pin 26 neg;
    set pin 26 pos after 350ms;

    leave start;
    enter descent after 1s;
  }
  
  if (State descent | A2 < 10kPa) {
    set pin 26 pos;
    set pin 19 neg;
    set pin 19 pos after 350ms;

    leave descent;
    enter ascent after 1s;
  }

  if (State ascent | A2 > 10kPa) {
    set pin 19 pos;
    set pin 26 neg;
    set pin 26 pos after 350ms;

    leave ascent;
    enter end after 1s;
  }
  
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
  [smooth      | A2               | 0.1                          ];
  
  [calibrate   | A0, poly, raw, V | 0.00041234314, -0.00198277794];
  [conversions | A0, raw, V, kPa  |                              ];
  [smooth      | A0               | 0.1                          ];
  
  [print       | gray             |                            ]
}

ds32 1Hz {
  [calibrate   | Time, poly, raw, s | 0.0009765625, 0.0]
  [conversions | Time, raw, s       |                  ]
}
