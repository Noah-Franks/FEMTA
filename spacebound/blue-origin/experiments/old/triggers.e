/* This file covers basically everything you can do.
 * The spacing does not matter, but it's best to keep
 * things readable so others can understand.
 */

ad15_sda 100Hz {
    /* in the context of the adc whose address pin is
     * connected to the i2c's sda line. Logged at 100Hz.
     * Best if all other ad15s log at the same frequency */
    
    /* Set gpio 23 hot and 24 cold when the difference
     * between channels 0 and 1 is less than 1.335 volts. 
     * When the difference is greater than 1.335 volts, do
     * the opposite by setting 23 cold and 24 hot, as 
     * indicated by the "reverses" flag. This trigger
     * persists throughout the whole experiment due to the 
     * "forever" option */
    A01 less_than 1.335v trigger {     
      +23, -24
    } forever, reverses;    // (reverses -> do opposite)
    
    /* Set gpio 25 hot when more than 4880 counts are observed 
     * on the difference between channels 0 and 1. This trigger
     * fires a singular time due to the "singular" flag. Once 
     * fired, 25 will remain hot until a different trigger 
     * changes its value. If the condition becomes false,
     * nothing happens since the "reverses" flag isn't present. */
    A01 more_than 4880c trigger {
      +25
    } singular;

    /* Print at 5Hz to console. If another sensor has the "print"
     * statement set, then the results are interleaved rather than
     * set in seperate columns. Not adding column support any time
     * soon due to the edge cases and complexity of doing that.
     */
} print;

ad15_vdd 100Hz {};   // no triggers
ad15_scl 100Hz {};   // -----------

ad15_gnd 100Hz {
    /* This ad15 is special. Unlike the others, which have been
     * set up to read a pair of differentials over A01 and A23,
     * this one reads 4 single channels over A0, A1, A2, and A3.
     * This means it does it measures against its internal GND,
     * which might differ from other grounds on the system. */
  
    A2 more_than 4880c trigger { +13, -23, +25 } reverses, singular;
};

adxl 50Hz {
  
    Z less_than -1.4g trigger {    // trigger when z-axis observes less than 1.4g
      +12,
      +13
    } singular;
  
    X more_than 1.0g trigger { +8 } forever , reverses;
    Y more_than 1.0g trigger { +9 } singular, reverses;
};

ds32 1Hz {
  
    Time more_than 90s trigger {    // make gpio 12 cold after 90 seconds
        -12
    } singular;

    Time more_than 30m trigger {    // make gpio 11 and 13 cold after 30 minutes
        -11, -13
    } singular;
};

