
ds32 1Hz {
  Time more_than 30m trigger {
    +24
  } singular;

  Time more_than 64s trigger {
    +23, -25
  } forever, reverses;
};
