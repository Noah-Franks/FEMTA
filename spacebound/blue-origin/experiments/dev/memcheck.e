

define enter start;

Sensor test 1Hz {
  
  where sine is wave;
  
  [conversions | sine, raw, raw | ];
  [print       | blue, sine     | ];
}
