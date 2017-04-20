void display_menu()
{
  switch(menu_state)
  {
    case 1:
        main_menu();
      break;
    case 2:
        menu_vhf_freq();
      break;
    case 3:
        menu_vhf_volume();
      break;  
    default:
      front_display();
  }
  display.display();
}
void goback()
{
    button_state = 0;
    menu_state = 0;
    btn1.interval(2000); // 2sec delay
    btn2.interval(0); // Instant
    btn3.interval(0);
}
void reset_menu_timout()
{
  menu_timeout = millis() + 5000;
}
void main_menu()
{
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15,0);
  display.println(F("Back"));
  display.setCursor(15,10);
  if(uhf_state)
    display.println(F("UHF            ON"));
  else
    display.println(F("UHF           OFF"));

  display.setCursor(15,20);
  display.println(F("Set FM freq"));
  display.setCursor(15,30);
  if(playmode==1)
    display.println(F("RANDOM          ON"));
  else
    display.println(F("RANDOM         OFF"));
  
  display.setCursor(15,40);
  display.println(F("MP3 audio level"));
  display.setCursor(15,50);
  display.println(F("UHF audio level"));
  display.setCursor(15,60);
  display.println(F("Soft reset"));
  
  // Display selection thingy
  display.setCursor(0,button_state*10);
  display.println(F(">"));
}

void menu_vhf_freq()
{
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(1,0);
  if(FMSTATION/100 < 100)
    display.print(F(" ")); 
  display.print(FMSTATION/100); 
  display.print(F("."));
  display.print((FMSTATION % 100)/10);
  display.setTextSize(2);
  display.print(F("MHz")); 
}
void menu_vhf_volume()
{
  display.clearDisplay();
  radio.readASQ();
  // Clear bar
  display.fillRect(0,0,display.width(),30,0);
  int w = radio.currInLevel;
  if(w < -30)
    w = -30;
  w = map(w,-30,0,0,128);
  display.fillRect(0,0,w,30,1);
  display.fillRect(0,32,display.width(),10,0);
  w = map(vol,0,30,0,128);
  display.fillRect(0,32,w,10,1);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(60,47);
  if (vol<10) display.print(F("0"));
  display.print(vol);
}

