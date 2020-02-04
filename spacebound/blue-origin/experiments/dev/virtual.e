
define enter start;

Sensor test 1Hz {
  [calibrate   | sine, poly, raw, C | 2.0, 0]
  [conversions | sine, raw, C       |       ]
  [print       | orange, sine       |       ]
}

Sensor arm6 1Hz {
  [calibrate   | Load, poly, raw, % | 100, 0]
  [conversions | Load, raw, %       |       ]
  [print       | pink, Load         |       ]
}
