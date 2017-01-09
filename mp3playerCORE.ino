byte error_count = 0;
uint8_t send_buf[10] = {
  0x7E, 0xFF, 06, 00, 00, 00, 00, 00, 00, 0xEF};

uint16_t mp3_get_checksum (uint8_t *thebuf) {
  uint16_t sum = 0;
  for (int i=1; i<7; i++) {
    sum += thebuf[i];
  }
  return -sum;
}
void mp3_fill_checksum () {
  uint16_t checksum = mp3_get_checksum (send_buf);
  fill_uint16_bigend (send_buf+7, checksum);
}
static void fill_uint16_bigend (uint8_t *thebuf, uint16_t data) {
  *thebuf = (uint8_t)(data>>8);
  *(thebuf+1) = (uint8_t)data;
}
void send_func () {
  for (int i=0; i<10; i++) {
    MP3Serial.write(send_buf[i]);
  }
}
//
void mp3_send_cmd (uint8_t cmd, uint16_t arg) {
  send_buf[3] = cmd;
  fill_uint16_bigend ((send_buf+5), arg);
  mp3_fill_checksum ();
  send_func ();
}

//
void mp3_send_cmd (uint8_t cmd) {
  send_buf[3] = cmd;
  fill_uint16_bigend ((send_buf+5), 0);
  mp3_fill_checksum ();
  send_func ();
}
void mp3_rec()
{
  byte cmd;
  byte buffer[10];
  int i;
  if(MP3Serial.available()>9)
  {
    cmd = MP3Serial.read();
    buffer[0] = cmd;
    if(cmd != 0x7E)
      return ;
    for(i=1;i<10;i++)
      buffer[i] = MP3Serial.read();
    switch(buffer[3])
    {
      case 0x42:
        switch(buffer[6])
        {
          case 0x00:
            #if (SERIALDEBUG)
            Serial.println(F("Stopped"));
            #endif
            if(digitalRead(UHF_SQ)==HIGH)
            {
              error_count++;
              if(error_count > 50)
              {
                error_count = 0;
                play_next(); // Next!
              }
            }
            mp3playing = false;
            delay(0);
            break;
          case 0x01:
            #if (SERIALDEBUG)
            Serial.println(F("Playing"));
            #endif
            error_count = 0;
            delay(0);
            mp3playing = true;
            break;
          case 0x02:
            #if (SERIALDEBUG)
            Serial.println(F("Paused during playback"));
            #endif
            delay(0);
            mp3playing = false;
            break;
          default:
            #if (SERIALDEBUG)
            Serial.println(F("No device online or sleeping"));
            #endif
            delay(0);
        }
        break;
      case 0x48:
        #if (SERIALDEBUG)
        Serial.print(F("Total files"));
        Serial.println(buffer[6], DEC);
        #endif
        //rnd_songs = buffer[6];
        break;
      case 0x3D:
        #if (SERIALDEBUG)
        Serial.print(F("Song finished, track "));
        Serial.println(buffer[6], DEC);
        #endif
        play_next(); // Next!
        break;
      case 0x4B:
        #if (SERIALDEBUG)
        Serial.print(F("Playing track "));
        Serial.println(buffer[6], DEC);
        #endif
        song = buffer[6];
        break;
      case 0x43:
        #if (SERIALDEBUG)
        Serial.print(F("Volume set to "));
        Serial.println(buffer[6], DEC);
        #endif
        delay(0);
        break;
      default:
        #if (SERIALDEBUG)
        Serial.print(F("Unknown command: "));
        Serial.println(buffer[3], HEX);
        #endif
        delay(0);
    }
    #if (SERIALDEBUG)
    Serial.print("Buffer: ");
    Serial.print(buffer[0], HEX);
    Serial.print(" ");
    Serial.print(buffer[1], HEX);
    Serial.print(" ");
    Serial.print(buffer[2], HEX);
    Serial.print(" ");
    Serial.print(buffer[3], HEX);
    Serial.print(" ");
    Serial.print(buffer[4], HEX);
    Serial.print(" ");
    Serial.print(buffer[5], HEX);
    Serial.print(" ");
    Serial.print(buffer[6], HEX);
    Serial.print(" ");
    Serial.print(buffer[7], HEX);
    Serial.print(" ");
    Serial.print(buffer[8], HEX);
    Serial.print(" ");
    Serial.println(buffer[9], HEX);
    #endif
  }
}


/*
void mp3_rec()
{
  byte cmd;
  byte buffer[10];
  int i;
  if(serial.available()>9)
  {
    cmd = serial.read();
    buffer[0] = cmd;
    if(cmd != 0x7E)
      return ;
    for(i=1;i<10;i++)
      buffer[i] = serial.read();
    switch(buffer[3])
    {
      case 0x42:
        mp3_state = buffer[6];
        switch(buffer[6])
        {
          case 0x00:
            #if (SERIALDEBUG)
            Serial.println(F("Stopped"));
            #endif
            error_count++;
            if(error_count > 3)
            {
              error_count = 0;
              play_next(); // Next!
            }
            delay(0);
            break;
          case 0x01:
            #if (SERIALDEBUG)
            Serial.println(F("Playing"));
            #endif
            error_count = 0;
            delay(0);
            break;
          case 0x02:
            #if (SERIALDEBUG)
            Serial.println(F("Paused during playback"));
            #endif
            delay(0);
            break;
          default:
            #if (SERIALDEBUG)
            Serial.println(F("No device online or sleeping"));
            #endif
            delay(0);
        }
        break;
      case 0x48:
        #if (SERIALDEBUG)
        Serial.print(F("Total files"));
        Serial.println(buffer[6], DEC);
        #endif
        rnd_songs = buffer[6];
        break;
      case 0x3D:
        #if (SERIALDEBUG)
        Serial.print(F("Song finished, track "));
        Serial.println(buffer[6], DEC);
        #endif
        play_next(); // Next!
        break;
      case 0x4B:
        #if (SERIALDEBUG)
        Serial.print(F("Playing track "));
        Serial.println(buffer[6], DEC);
        #endif
        song = buffer[6];
        break;
      case 0x43:
        #if (SERIALDEBUG)
        Serial.print(F("Volume set to "));
        Serial.println(buffer[6], DEC);
        #endif
        delay(0);
        break;
      default:
        #if (SERIALDEBUG)
        Serial.print(F("Unknown command: "));
        Serial.println(buffer[3], HEX);
        #endif
        delay(0);
    }
    #if (SERIALDEBUG)
    Serial.print("Buffer: ");
    Serial.print(buffer[0], HEX);
    Serial.print(" ");
    Serial.print(buffer[1], HEX);
    Serial.print(" ");
    Serial.print(buffer[2], HEX);
    Serial.print(" ");
    Serial.print(buffer[3], HEX);
    Serial.print(" ");
    Serial.print(buffer[4], HEX);
    Serial.print(" ");
    Serial.print(buffer[5], HEX);
    Serial.print(" ");
    Serial.print(buffer[6], HEX);
    Serial.print(" ");
    Serial.print(buffer[7], HEX);
    Serial.print(" ");
    Serial.print(buffer[8], HEX);
    Serial.print(" ");
    Serial.println(buffer[9], HEX);
    #endif
  }
}
*/
