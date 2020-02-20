
define enter start;

  Sensor test 4Hz {
      [ print | blue, sine | 2 ];
  }

  Sensor arm6 12/8Hz {
      [ calibrate   | Load, poly, raw, % | 100, 0 ]
      [ conversions | Load, raw, %       |        ]
      [ print       | pink, Load         |      0 ]
  }

