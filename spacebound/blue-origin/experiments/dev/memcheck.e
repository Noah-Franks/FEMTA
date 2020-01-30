
define enter start;
define leave oscillator;

Sensor test 1Hz {
  
  where sine   is wave;
  where cosine is unused;
  
  [calibrate   | random, hart, raw, MB  | 0, 0, 0];    // intentionally unused
  [calibrate   | random, poly, raw, kPa | 0, 0, 0];    // --------------------
  [conversions | random, raw, raw       |        ];
  [conversions | sine, raw, raw         |        ];
  [print       | blue, sine             |        ];
}

Sensor arm6 1Hz {
  
  where Memory      is mem;
  where Temperature is cpu;
  
  [calibrate   | mem, poly, raw, KB  |     1, 0];
  [calibrate   | mem, poly, MB, %    | 0.231, 0];
  [conversions | mem, raw, KB, MB, % |         ];
  
  [calibrate   | cpu, poly, raw, C   | 0.001, 0];  
  [conversions | cpu, raw, C, F      |         ];
  
  [calibrate   | Load, poly, raw, %  |   100, 0];
  [conversions | Load, raw, %        |         ];
  
  [print       | orange, Load, mem   |     0, 0];
}

Sensor ds32 1Hz {
  
  where Time is clock;
  
  [calibrate   | clock, poly, raw, ms | 0.9765625, 0.0];
  [conversions | clock, raw, ms, s    |               ];
  [smooth      | clock                |            0.1];
  [print       | pink, clock          |               ];
  
  if (State start | clock < 1s | single) {
    leave start;
    leave start after 2s;
    enter oscillator after 6s;
  }
  
  if (State oscillator | clock > 4s | forever, reverse) {
    
    set pin 7 pos;
    set pin 7 neg after 3s;
    
    set pin 8 pos;
    set pin 8 neg after 2s;
    
    leave oscillator;
    enter oscillator after 4s;
  }
}

Sensor ad15_gnd 2/3Hz {
  
  where A0 is pressure;
  
  [calibrate   | A1, poly, raw, V       | 0.0001874287, -0.0009012627];
  [calibrate   | A2, poly, raw, V       | 0.0001874287, -0.0009012627];
  [calibrate   | A3, poly, raw, V       | 0.0001874287, -0.0009012627];
  [conversions | A1, raw, V             |                            ];
  [conversions | A2, raw, V             |                            ];
  [conversions | A3, raw, V             |                            ];
  
  [calibrate   | pressure, poly, raw, V | 0.0001874287, -0.0009012627];
  [conversions | pressure, raw, V       |                            ];
  
  [print       | mint, pressure         | 4                          ];
  [print       | purple, A1, A2, A3     | 4, 4, 4                    ];
}
