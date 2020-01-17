

ad15 100Hz {
  
  if (State pad : A0 < 10kPa : reverses) {
    set +0;
    set -1 [pulse :: 350ms];
    leave pad;
    enter apogee;
  }
  
  [calibrate  : A0, poly, V, kPa];
  [conversion : A0, raw, V, kPa];
};
