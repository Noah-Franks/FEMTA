

define enter start;
define leave oscillator;

/*Sensor ad15_gnd 1Hz {
       [print | A0, A1, A2, A3 | ]
}*/

/*Sensor ad15_sda 1Hz {
       [print | pink, A0, A1, A2, A3 | ]
}*/

/*Sensor ad15_scl 1Hz {
       [print | A0, A1, A2, A3 | ]
}*/

/*Sensor ad15_vdd 1Hz {
       [print | green, A0, A1, A2, A3 | ]
}*/

Sensor arm6 1Hz {
       where Temperature is cpu;
       [calibrate   | cpu, poly, raw, C   | 0.001, 0];  
       [conversions | cpu, raw, C, F      |         ];
       [print | red, Temperature | ];
}

Sensor ds32 1Hz {
       
  where Time is clock;
  
  [calibrate   | clock, poly, raw, ms | 0.9765625, 0.0];
  [conversions | clock, raw, ms, s    |               ];
  [print       | blue, clock          |               ];
  
  if (State start | clock < 1s | single) {
    leave start;
    leave start after 2s;
    enter oscillator after 6s;
  }
  
  if (State oscillator | clock > 4s | forever) {
    
    set pin 19 pos;
    set pin 19 neg after 3s;
    
    set pin 21 pos;
    set pin 21 neg after 2s;
    
    leave oscillator;
    enter oscillator after 4s;
  }

}
