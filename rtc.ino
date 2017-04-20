void write_time()
{
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setTextSize(4);
  display.setCursor(5,10);
  if(hour() < 10)
    display.print(0);
  display.print(hour());
  displayDigits(minute());
}
void write_time_sec()
{
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(20,10);
  if(hour() < 10)
    display.print(0);
  display.print(hour());
  displayDigits(minute());
  displayDigits(second());
}
void displayDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    display.print(F(":"));
    if(digits < 10)
        display.print(F("0"));
    display.print(digits);
}
