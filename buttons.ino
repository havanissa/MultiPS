void button_check()
{
  btn1.update();
  btn2.update();
  btn3.update();
  switch(menu_state)
  {
    case 1:
      if(btn2.fell())
      {
        reset_menu_timout(); // reset menu timeout
        button_state--;
        if(button_state < 0)
          button_state = 0;
        if(button_state > 5)
          button_state = 5;
        #if (SERIALDEBUG)
        Serial.print(F("Buttonstate:"));
        Serial.println(button_state);
        #endif
      }
      if(btn3.fell())
      {
        reset_menu_timout(); // reset menu timeout
        button_state++;
        if(button_state < 0)
          button_state = 0;
        if(button_state > 5)
          button_state = 5;
        #if (SERIALDEBUG)
        Serial.print(F("Buttonstate:"));
        Serial.println(button_state);
        #endif
      }
      if(btn1.fell())
      {
        reset_menu_timout(); // reset menu timeout
        switch(button_state)
        {
          case 0:
            // Go back
            goback();
            break;
          case 1:
            // UHF on/off
            if(uhf_state)
            {
              digitalWrite(UHF_PD, LOW); // Turn off UHF Radio
              uhf_state = false;
            }else{
              digitalWrite(UHF_PD, HIGH); // Turn on UHF Radio
              uhf_state = true;
              UHF_radio_start();
            }
            break;
          case 2:
            // set FM frequency
            menu_state = 2;
            button_state = 0;
            btn1.interval(2000); // 2sec delay
            btn2.interval(100); // almost instant
            btn3.interval(100);
            break;
          case 3:
            // random on/off
            if(playmode==0)
            {
              //super_random_rebuffer();
              mp3_send_cmd(0x18); // random
              playmode = 1;
            }else{
              mp3_send_cmd(0x08,0x00); // Normal
              playmode = 0;
            }
            break;
          case 4:
            // MP3 volume
            btn1.interval(2000); // 2sec delay
            btn2.interval(100); // almost instant
            btn3.interval(100);
            menu_state = 3;
            button_state = 0;
            break;
          case 5:
            // UHF volume
            // currently disabled
            break;
          case 6:
            // Soft reset
            asm volatile ("jmp 0");  
            break;
        }
      }
      break;
    case 2:
      // FM Frequency
      // mp3 volume
      if(btn2.fell())
      {
        menu_state = 1;
        button_state = 4;
      }
      break;
    case 3:
      // mp3 volume
      if(btn1.fell())
      {
        menu_state = 1;
        button_state = 4;
      }
      if(btn2.fell())
      {
        reset_menu_timout(); // reset menu timeout
        vol--;
        if(vol < 0)
          vol = 0;
        if(vol > 30)
          vol = 30;
        mp3_send_cmd(0x06,vol); // set volume
      }
      if(btn3.fell())
      {
        reset_menu_timout(); // reset menu timeout
        vol++;
        if(vol < 0)
          vol = 0;
        if(vol > 30)
          vol = 30;
        mp3_send_cmd(0x06,vol); // set volume
      }
      break;  
    default:
      // Default button modes
      if(btn1.fell())
      {
        #if (SERIALDEBUG)
        Serial.println(F("Menu trigger"));
        #endif
        // Enter first menu level
        menu_state = 1;
        button_state = 0; // reset button
        reset_menu_timout(); // reset timeout timer
        btn1.interval(200); // 2sec delay
        btn2.interval(100); // Instant
        btn3.interval(100);
      }
      
      if(btn2.fell())
      {
        play_previous();
      }
      if(btn3.fell())
      {
        play_next();
      }
  }
  
}

