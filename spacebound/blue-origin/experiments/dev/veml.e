
define enter start;

Sensor veml 1Hz {
  
  [calibrate   | UVA, poly, raw, % | 0.0015258789, 0];
  [calibrate   | UVB, poly, raw, % | 0.0015258789, 0];
  [conversions | UVA, raw, %       |                ];
  [conversions | UVB, raw, %       |                ];
  
  [print | pink, UVA | 2];
  [print | mint, UVB | 2];
}
