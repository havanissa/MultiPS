unsigned long UHF_timeout;
void UHF_check_radio()
{
  if(!uhf_state)
  {
    display.invertDisplay(false);
    return;
  }
  if(millis() > UHF_timeout)
  {
    if(digitalRead(UHF_SQ)==HIGH && !mp3playing)
    {
      mp3_send_cmd(0x0D); // Resume playing
    }
    UHF_timeout = millis() + UHF_timeout_VALUE;
  }
  if(digitalRead(UHF_SQ)==LOW)
  {
    UHF_timeout = millis() + UHF_timeout_VALUE;
    display.invertDisplay(true);
    if(mp3playing)
      mp3_send_cmd(0x0E); // Pause
  }else{
    display.invertDisplay(false);
  }
  
}
void UHF_radio_status_request()
{
  UHFSerial.println(F("AT+DMOCONNECT"));
  UHFSerial.println(F("RSSI?"));
}
void UHF_radio_status()
{
  #if (SERIALDEBUG)
  if(UHFSerial.available())
    Serial.write(UHFSerial.read());
  #endif
    
}
void UHF_radio_start()
{
  UHFSerial.println(F("AT+DMOSETGROUP=0,457.5500,457.5500,0000,3,0000"));
  delay(500);
  UHFSerial.println(F("AT+DMOSETVOLUME=8"));
  delay(500);
}
