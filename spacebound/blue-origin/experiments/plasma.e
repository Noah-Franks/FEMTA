
define enter start;
define leave close;

Sensor test 10Hz {

  if (State start | zero > -1raw) {
    
    // open the vent valve
    set pin 17 pos;
    set pin  4 neg;
    set pin 17 neg after 350ms;    
    
    leave start;
    enter close after 10s;
  }
  
  if (State close | zero > -1raw) {
    
    // close the vent valve
    set pin  4 pos;
    set pin 17 neg;
    set pin  4 neg after 350ms;
    
    leave close;
    enter start after 10s;
  }
}
