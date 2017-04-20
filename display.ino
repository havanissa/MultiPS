void front_display()
{
    display.clearDisplay();
    write_time(); // Update time on screen
    // Placeholders
    display.setTextSize(1);
  display.setCursor(1,45);
  display.print(F("V    "));
  display.print(FMSTATION/100);
  display.print(F("."));
  display.print((FMSTATION%100)/10);
  display.print(F("M"));
  display.setCursor(1,55);
  display.print(F("U "));
  if(uhf_state)
  {
    display.print(UHF_FREQ/1000);
    display.print(F("."));
    display.print(UHF_FREQ%1000);
    display.print(F("M"));
  }else{
    display.print(F("disabled"));
  }
  
  display.setCursor(70,45);
  display.print(F("BAT "));
  display.print(F(" 0.00"));
  /*
  display.print(vcurr/100);
  display.print(F("."));
  display.print(vcurr%100);
  */
  display.print(F("v"));
  display.setCursor(70,55);
  if(mp3playing){
    display.print(F("MP3 P"));
    if(song < 100)
      display.print(F("0"));
    if(song < 10)
      display.print(F("0"));
    display.print(song);
    if(playmode==0)
      display.print(F("N"));
    else
      display.print(F("R"));
    
  }else{
    display.print(F("MP3 PAUS"));
  }
    
  
}
/*
void update_voltage()
{
  int voltage;
  if(vread>INPUT_MEASSURE)
  {
    vcurr = map(vsum/vread,0,1024,0,INPUT_VOLTAGE);
    vread = 0;
    vsum = 0;
  }
  voltage = analogRead(A6);
  vsum += voltage;
  vread++;
}*/
