
// Notes
//   GPIO numbers are specified by their Broadcom IDs
//   once a trigger condition is achieved, it fires forever

ad15_vdd {
  
    hertz = 50                         // log at 100Hz
    
    A01 less_than 1.0v trigger 22      // triggers GPIO 22 when less than 1V is read on diff 01
    A23 more_than 4.0v trigger 23      // triggers GPIO 23 when more than 4V is read on diff 23
    
    print                              // print at 5Hz to console
}

adxl {
    hertz = 50
    
    Z less_than -1.4g trigger 4        // multiple triggers can be attached
    Z less_than -4.0g trigger 5        // ---------------------------------
}

ds32 {
    hertz = 1
    
    time more_than 32s trigger 8
}

