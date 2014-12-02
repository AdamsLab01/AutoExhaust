void loop() { 
  switch(state) { 
  case S_Monitor:
    F_Monitor();
    break;

  case S_AutoInside:
    F_AutoInside();
    break;

  case S_AutoOutside:
    F_AutoOutside();
    break;

  case S_ManOut:
    F_ManOut();
    break;

  case S_ManIn:
    F_ManIn();
    break;

  case S_ManStop:
    F_ManStop();
    break;

  case S_Emergency:
    F_Emergency();
    break;
  }
}

