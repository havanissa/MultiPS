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
#define INPUT_VOLTAGE 511 // Voltage x100 times
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

int FMSTATION = 9150;

Bounce btn1 = Bounce();
Bounce btn2 = Bounce();
Bounce btn3 = Bounce();

byte playmode = 0; // 0 normal repeat folder, 1 random

byte vol = 28;

int vsum = 0;
int vread = 0;
int vcurr = 0;
int song = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(UHF_PTT,OUTPUT);
  pinMode(UHF_PD,OUTPUT);
  pinMode(UHF_SQ,INPUT);
  digitalWrite(UHF_PTT, HIGH); // Low TX, high RX
  
  Serial.begin(115200);
  
  #if (SERIALDEBUG)
  Serial.println(F("Starting display"));
  #endif
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  
  #if (SERIALDEBUG)
  Serial.print(F("Si4713 Init..."));
  #endif
  if(!radio.begin())
  {
    #if (SERIALDEBUG)
    Serial.println(F("ERROR"));
    #endif
    display.print(F("VHF Radio error"));
    display.display();
    while(1); /* endless loop */
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
  UHFSerial.begin(9600);
  UHF_radio_start();
  
  MP3Serial.begin(9600);
  #if (SERIALDEBUG)
  Serial.println(F("MP3 Startup"));
  #endif
  mp3_start();
  play_random();


  #if (SERIALDEBUG)
  Serial.println(F("Init buttons"));
  #endif
  pinMode(11, INPUT_PULLUP); // Btn1
  pinMode(10, INPUT_PULLUP); // Btn3
  pinMode(9, INPUT_PULLUP); // Btn4

  btn1.attach(11); 
  btn1.interval(2000);
  btn2.attach(10);
  btn2.interval(0);
  btn3.attach(9);
  btn3.interval(0);
  
  display.clearDisplay();
}

boolean variance;
unsigned long timer_100hz;
unsigned long timer_10hz;
unsigned long timer_2hz;
unsigned long timer_1hz;
void loop() {
  
  if(millis() > timer_100hz)
  {
    // Buttons
    button_check();
    timer_100hz = millis() + 10;
  }
  if(millis() > timer_10hz)
  {
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
    update_display();
    timer_10hz = millis() + 100;
  }
  if(millis() > timer_2hz) 
  {
    #if (SERIALDEBUG)
    //Serial.println(F("4hz"));
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
    //Serial.println(F("1hz"));
    #endif
    // Meassure voltage
    update_voltage();
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




