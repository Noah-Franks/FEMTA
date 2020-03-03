
define enter start;

Sensor ad15_gnd 25Hz {
    
  [calibrate   | A0   , poly, raw, V | 0.0001874287, -0.0009012627];
  [conversions | A0   , raw, V, mV       |                 ];
  
  [print       | mint, A0            | 4                          ];
}
