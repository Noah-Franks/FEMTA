
define enter start;

Sensor ds32 1Hz {
       where Time is clock;
       where Temperature is temp;
       
       [calibrate   | clock, poly, raw, ms | 0.9821424, 0 ];
       [calibrate   | temp, poly, raw, C   |       1.0, 0 ];
       [conversions | clock, raw, ms, s    |              ];
       [conversions | temp, raw, C, F      |              ];
       [print       | blue, clock          |            2 ];
       [print       | red, temp            |            2 ];
}

/*Sensor ad15_gnd 1Hz {
       [print | red, A0, A1, A2, A3 | 0, 0, 0, 0]
}

Sensor ad15_vdd 1Hz {
       [print | cyan, A0, A1, A2, A3 | 0, 0, 0, 0]
}

Sensor ad15_scl 1Hz {
       [print | cyan, A0, A1, A2, A3 | 0, 0, 0, 0]
}*/

Sensor mcp9 1Hz {
       where Temperature is heat;
       [calibrate   | heat, poly, raw, C | 1.0, 0.0]
       [conversions | heat, raw, C, F    |         ]
       [print       | red, heat          |       1 ]
}

Sensor arm6 1Hz {
       where Temperature is cpu;
       [calibrate   | cpu, poly, raw, C   | 0.001, 0];  
       [conversions | cpu, raw, C, F      |         ];
       [print       | red, cpu            |       2 ];
}
