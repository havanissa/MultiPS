void mp3_start()
{
  mp3_send_cmd(0x0C); // Reset
  delay(1000);
  
  //mp3_send_cmd(0x18); // Random
  //delay(1000);
  mp3_send_cmd(0x06,vol); // set volume
  delay(1000);
  //mp3_send_cmd(0x01); // Next
  mp3_send_cmd(0x0D); // Play
  delay(1000);
}
void play_next()
{
  /*
  if(playmode==1)
    super_random_next();
  else
  */
    mp3_send_cmd(0x01);
}
void play_previous()
{
  /*
  if(playmode==1)
    super_random_previous();
  else
  */
    mp3_send_cmd(0x02);
}

void play_random()
{
  //super_random_rebuffer();
  mp3_send_cmd(0x0D);
}

void super_random_next()
{
  rnd_current++;
  if(rnd_current>rnd_songs)
    super_random_rebuffer();
  mp3_send_cmd(0x03,rnd_buffer[rnd_current]);
}
void super_random_previous()
{
  rnd_current--;
  if(rnd_current>rnd_songs)
    rnd_current = 0;
  mp3_send_cmd(0x03,rnd_buffer[rnd_current]);
}

void super_random_rebuffer()
{
  int i;
  byte rnd;
  randomSeed(analogRead(0));
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(1,0);
  display.print(F("Randomizing...")); 
  display.display();
  // Clear buffer
  for(i=0;i<200;i++)
    rnd_buffer[i] = 0;
  // Refill buffer
  for(i=1;i<=rnd_songs;i++)
  {
    rnd = random(1,rnd_songs+1);
    if(rnd_buffer[rnd]==0)
      rnd_buffer[rnd] = i;
  }
  rnd_current = 0;
}

