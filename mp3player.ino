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
  if(playState==2)
    super_random_next();
  else
  */
    mp3_send_cmd(0x01);
}
void play_previous()
{
  /*
  if(playState==2)
    super_random_previous();
  else
  */
    mp3_send_cmd(0x02);
}

void play_random()
{
  mp3_send_cmd(0x0D);
}
