

ad15_sda {
    hertz = 100

    A01 less_than 7120 trigger {       // 1.335v 20kpa trigger the white wire
      +23, -24,
    } forever, reverses;
    
    A01 less_than 4880 trigger {       // 0.915v 10kpa trigger the green wire
      +25,
    } singular;
      
    //print                               // print at 5Hz to console
}


ad15_vdd {
    hertz = 100                          // log at 100Hz

    print
}

ds32 {
    hertz = 1

    y = 0.4x^2 + 0.5x + 3
}

