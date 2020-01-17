
ad15_gnd 100Hz {
  
  A01 more_than 1.04v {
    
    +24 [pulse = 40ms],
    -12 [pulse = 20ms]
    
  } singular;

  calibrate {

  };
  
} print;

// singular - happens once, after which trigger is never considered
// forever  - happens whenever condition is true
// reverses - create the opposite trigger as well
// resets   - after firing, stop being considered until falsehood, in which case reset
