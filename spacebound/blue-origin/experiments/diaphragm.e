/**
 * @Authors Noah
 * @Experimenters Sam, Adi, Noah
 * @Experiment_Date 10//19
 * @Compiler_Version Beta 0.9
 * @Licence GPLv3
 * 
 * @Description Tests a diaphragm, determining if it holds pressure, leaks, or bursts
 **/

ad15_gnd 5Hz {
  
  if (A0 < 10Kpa : singular) {     
    set -17;
    set +18 [pulse :: 350ms];         // we take power off so as to avoid burning the solenoid
  };
  
  [calibrate : A0, poly, V, kPa :    // we expect 11.3v at 1atm : V->kPa
     -0.00000805518095144006,        // degrees are from high to low
      0.000179617025688186,
     -0.00149263466053365,
      0.0137083951784765,
      3.84928878749102,
     -0.389869854380195
   ];
  
  [calibrate   : A0, poly, raw, V : 0.0001874287, -0.0009012627];
  [conversions : A0, raw, V, kPa  :                           0];
//[print       : gray             :                         5Hz];
};

ds32 1Hz {
  [calibrate   : Time, poly, raw, s : 0.0009765625, 0.0];
  [conversions : Time, raw, s       :                 0];
  [print       : gray               :               5Hz];
};