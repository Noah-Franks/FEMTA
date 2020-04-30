
define enter start
define leave mission_complete

Sensor test 1Hz {
       
       [print | mint, sine | 2]
       
       if (State start | sine > 0.5raw) {
          leave start;
          enter mission_complete after 5s;
       }
}
