#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Si4713.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <Bounce2.h>
#include <avr/wdt.h>
#include <MemoryFree.h>
#include <DS3232RTC.h>
#include <Streaming.h>
#include <TimeLib.h>
#include <Bounce2.h>

#define SERIALDEBUG false

#define UHF_PTT 6
#define UHF_PD  5
#define UHF_SQ  4
#define UHF_TX  2
#define UHF_RX  3
#define MP3_TX  7
#define MP3_RX  8
#define INPUT_VOLTAGE 511 // Meassured voltage x100 times
#define INPUT_MEASSURE 5
#define UHF_timeout_VALUE 3000 // 3sek

long UHF_FREQ = 457550;

SoftwareSerial UHFSerial(UHF_RX, UHF_TX); // RX, TX
SoftwareSerial MP3Serial(MP3_RX, MP3_TX); // RX, TX

#define RESETPIN 12

#define OLED 0x3C

#define INPUT_VOLTAGE 508 // Voltage x100 times

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);
Adafruit_SSD1306 display(0);

bool mp3playing = false;
byte menu_state = 0; // 0. mainscreen 1. first level menu ....
byte button_state = 0; // where is the button?
bool fancystart = false;
bool uhf_state = false;
 int playState = 1;

int FMSTATION = 9150;

Bounce btn1 = Bounce();
Bounce btn2 = Bounce();
Bounce btn3 = Bounce();

byte playmode = 0; // 0 normal repeat folder, 1 random

byte vol = 20;
/* // not functioning
int vsum = 0;
int vread = 0;
int vcurr = 0;
*/

byte song = 0;

byte rnd_current = 0;
byte rnd_songs = 0;
byte rnd_buffer[100]; // Buffer really low... need to optimize the shit out of the code

void setup() {
  // put your setup code here, to run once:
  pinMode(UHF_PTT,OUTPUT);
  pinMode(UHF_PD,OUTPUT);
  pinMode(UHF_SQ,INPUT);
  digitalWrite(UHF_PTT, HIGH); // Low TX, high RX
  
  Serial.begin(115200);


  pinMode(11, INPUT_PULLUP); // Btn1
  pinMode(10, INPUT_PULLUP); // Btn3
  pinMode(9, INPUT_PULLUP); // Btn4

  #if (SERIALDEBUG)
  Serial.println(F("Starting display"));
  #endif
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  if(digitalRead(11) == LOW)
    fancystart = true;

  delay(250);
  if(fancystart)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(1,0);
    display.println(F("Display init... OK"));
    display.display();
  }
  
  #if (SERIALDEBUG)
  Serial.print(F("Si4713 Init..."));
  #endif
  if(fancystart)
  {
    display.print(F("VHF Radio..."));
    display.display();
  }
  if(!radio.begin())
  {
    #if (SERIALDEBUG)
    Serial.println(F("ERROR"));
    #endif
    display.print(F("VHF Radio error"));
    display.display();
    while(1); /* endless loop */
  }
  if(fancystart)
  {
    display.println(F("OK"));
    display.display();
  }
  VHF_radio_start();
  delay(1000);
  VHF_radio_start();
  #if (SERIALDEBUG)
  Serial.println(F("OK"));
  #endif

  #if (SERIALDEBUG)
  Serial.println(F("Starting RTC"));
  #endif
  setSyncProvider(RTC.get);
  if(fancystart)
  {
    display.print(F("RTC..."));
    if(timeStatus() != timeSet) 
      display.println(F("Failed"));
    else
      display.println(F("OK"));
    display.display();
  }
  #if (SERIALDEBUG)
  if(timeStatus() != timeSet) 
      Serial.println(F("Unable to sync with the RTC"));
  else
      Serial.println(F("RTC has set the system time"));
  #endif
  
  
  #if (SERIALDEBUG)
  Serial.println(F("Starting UHF radio"));
  #endif
  digitalWrite(UHF_PD, HIGH); // Turn on UHF Radio
  uhf_state = true;
  UHFSerial.begin(9600);
  UHF_radio_start();
  if(fancystart)
  {
    display.println(F("UHF radio... OK"));
    display.display();
  }
    if(fancystart)
  {
    display.println(F("Starting MP3..."));
    display.display();
  }
  MP3Serial.begin(9600);
  #if (SERIALDEBUG)
  Serial.println(F("MP3 Startup"));
  #endif
  mp3_start();
  play_random();

  if(fancystart)
  {
    display.println(F("Starting buttons..."));
    display.display();
  }
  #if (SERIALDEBUG)
  Serial.println(F("Init buttons"));
  #endif
  btn1.attach(11); 
  btn1.interval(2000);
  btn2.attach(10);
  btn2.interval(0);
  btn3.attach(9);
  btn3.interval(0);
  
  delay(1000);
  display.clearDisplay();
  #if (SERIALDEBUG)
  Serial.println(F("Setup complete"));
  #endif
}

boolean variance;
unsigned long timer_10hz;
unsigned long timer_2hz;
unsigned long timer_1hz;
unsigned long menu_timeout;
void loop() {

  if(millis() > menu_timeout)
  {
    goback();
    menu_timeout = millis() + 5000; // 5 second timeout
  }
  
  if(millis() > timer_10hz)
  {
    // Buttons
    button_check();
    #if (SERIALDEBUG)
    //Serial.println(F("10hz"));
    #endif
    // Debug tool
    write2uhf();
    // MP3 Check
    mp3_rec();
    // UHF Check
    UHF_check_radio();
    // Update display
    display_menu();
    timer_10hz = millis() + 100;
  }
  if(millis() > timer_2hz) 
  {
    #if (SERIALDEBUG)
    //Serial.println(F("2hz"));
    #endif
    if(variance)
      mp3_send_cmd(0x42); // Current status
    else
      mp3_send_cmd(0x4B); // Current track
    variance = !variance;
    
    timer_2hz = millis() + 500;
  }
  if(millis() > timer_1hz) 
  {
    #if (SERIALDEBUG)
    Serial.println(F("1hz"));
    #endif
    // Meassure voltage
    //update_voltage();
    // UHF Status
    UHF_radio_status_request();
    UHF_radio_status();
    
    timer_1hz = millis() + 1000;
  }
}

void write2uhf()
{
  if(Serial.available())
    UHFSerial.write(Serial.read());
}




