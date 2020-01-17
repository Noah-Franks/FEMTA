
ad15_sda 100Hz {
	 
    A01 less_than 1.335v trigger {     
      +23, -24
    } forever, reverses;    // (reverses -> do opposite)
        
} print;

ad15_vdd 100Hz {};   // no triggers
ad15_scl 100Hz {};   // -----------
ds32 1Hz {};

