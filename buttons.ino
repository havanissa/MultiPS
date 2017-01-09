void button_check()
{
  btn1.update();
  btn2.update();
  btn3.update();

  if(btn1.fell())
  {
    if(playmode==0)
    {
      mp3_send_cmd(0x18); // random
      playmode = 1;
    }else{
      mp3_send_cmd(0x08,0x00); // Normal
      playmode = 0;
    }
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

