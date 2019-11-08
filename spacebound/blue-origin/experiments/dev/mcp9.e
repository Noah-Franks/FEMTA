

define enter start;

mcp9 4/3Hz {
  [calibrate   | Temperature, poly, raw, C | 1.0, 0.0]
  [conversions | Temperature, raw, C, K    |         ]
  [print       | red                       |         ]
}
