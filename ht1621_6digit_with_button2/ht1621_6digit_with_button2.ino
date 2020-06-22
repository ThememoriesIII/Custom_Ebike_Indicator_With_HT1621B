#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Time.h>
#include <ht1621.h>
#include <DS1307RTC.h>
#include <DHT.h>
#include <MFRC522.h>

//spi select chip
#define cs_sd 10
#define cs_lcd 3
//datacomunicate
#define rd 6
#define wr 4
#define data 5
//hall sensor pin
#define hallpin 2
//voltage diider pin
#define devidor_pin A0
//relay pin
#define RELAY 6
//tepmperatur pin
#define DHTPIN 7
//rfid comunication pin
#define RST_PIN 8          // Configurable, see typical pin layout above
#define SS_PIN 9
//temoperatur module type
#define DHTTYPE DHT22

HT1621 HT1621_lcd(data,wr,cs_lcd); // data,wr,rd,cs
DHT dht(DHTPIN, DHTTYPE);
File Data_Logger;
MFRC522 mfrc522(SS_PIN, RST_PIN);

boolean check_key();
void bike_start();
void read_block_card(byte block,byte len,char *text_t);
void update16();
void lcd16set_time(tmElements_t tm);
void print_km(float km);
void print_dist(float dist);
void setdecimalseparator(int decimaldigits);
void setBatteryLevel(int level);
void print_temp_volt(float temp,float volte);

// wirefile and time
uint8_t hour12(uint8_t hour24);
char *strAmPm(uint8_t hour);
void wire_file(float kmhr_,tmElements_t tm,float dist_t,float temp_t,float rpm_t,float v_t);
float getVoltage();

// 0 1 2 3 4 5 6 7 8 9 -
unsigned char lcd6digitSegment[11]={0x7D,0x60,0x3e,0x7a,0x63,0x5b,0x5f,0x70,0x7f,0x7b,0x02};
unsigned char lcd6digitBuffer[7] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//use for messure distan 
// rpm kmhr totaldistan
unsigned long prevmillis=0;
unsigned long prevmillis_whel_stop=0; 
unsigned long duration;
float rpm=0.0;
float kmhr=0.0;
float dist_t=0.0;
//for temperature
float temp = 0;
float tempbuffer=0;
//for voltage
float voltage = 0;
int batlevel=0;
//deboud state
boolean currentstate; // Current state of IR input scan
boolean prevstate=LOW;
//mod show
int mod = 0;
//refresh time mod
unsigned long refresh1=0;
unsigned long refresh2=0;
unsigned long refresh3=0;
unsigned long saverefresh=0;

boolean start_bike=0;
boolean SD_state=0;
byte access_id[4]={0x77,0x32,0xBD,0x5F};
char ACCESS_CODE[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char NAME[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char LNAME[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
/////////// debug to serial
//#define DEBUG
//////////get data debug
//DEBUG_GetVolt

////////// sd card debug
//define DEBUG_SD

///////////lcd debug
//#define DEBUG_temp_volt
//#define DEBUG_time
//#define DEBUG_mod
//#define RAMDEBUG

/////////// debug data in loop
//#define DEBUG_in_refresh

void setup() 
{
  // put your setup code here, to run once:
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  pinMode(hallpin, INPUT);
  pinMode(cs_sd, OUTPUT);
  pinMode(SS_PIN,OUTPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(RELAY,OUTPUT);
  SPI.begin();
  digitalWrite(SS_PIN, LOW);
  digitalWrite(cs_sd, HIGH);
  digitalWrite(RELAY, HIGH);
  delay(50);
  mfrc522.PCD_Init();
  delay(1000);
  HT1621_lcd.begin(true);
  HT1621_lcd.sendCommand(HT1621::BIAS_THIRD_4_COM);  // FOR TS119
  HT1621_lcd.sendCommand(HT1621::SYS_EN);
  HT1621_lcd.sendCommand(HT1621::LCD_OFF);
  digitalWrite(RELAY, HIGH);
  while(start_bike==0)
  {
    start_bike=check_key();
     #ifdef DEBUG
     Serial.print("BIKE KEY");
     Serial.println(start_bike);
     #endif
  }
  digitalWrite(RELAY, LOW);
  if(!HT1621_lcd.begin())
  {
    #ifdef DEBUG
    Serial.println("lcd FAIL");
    #endif
  }
  else
  {
    HT1621_lcd.sendCommand(HT1621::LCD_ON);
  }

  digitalWrite(SS_PIN, HIGH);
  digitalWrite(cs_sd,LOW );
  delay(50);
 if (!SD.begin(cs_sd))
  {
    SD_state=0;
    #ifdef DEBUG
    Serial.println("CARD FAILL");
    delay(1000);
    #endif
  }
  else
    SD_state=1;
  Wire.begin();
  dht.begin();
}

void loop() 
{
  currentstate = digitalRead(hallpin); // Read hall sensor state
  if( prevstate != currentstate) // If there is change in input
  {
    if( currentstate == HIGH ) // If input only changes from LOW to HIGH
    {
      duration = ( micros() - prevmillis ); // Time difference between revolution in microsecond
      rpm = (60000000/duration); // rpm = (1/ time millis)*1000*1000*60;
      prevmillis = micros(); // store time for nect revolution calculation
      prevmillis_whel_stop=millis();
      //v =3.14*0.0006069*(rpm*60);//speed(km/hr) = 3.14159*(D/100000)*(rpm*60) ; D (cm.) 
      //k = d × r × 0.001885
      //1885/1000000
      kmhr= (60.69*rpm*0.001885);
      //Serial.println(kmhr);
      dist_t+=0.0006969;
      #ifdef DEBUG
      Serial.print("dist in cal ");
      Serial.println(dist_t);
      Serial.print("rpm in cal ");
      Serial.println(rpm);
      Serial.print("kmhr in cal ");
      Serial.println(kmhr);
      Serial.print("dist in cal ");
      Serial.println(dist_t);
      #endif
    }
  }
  else
  {
    if(millis()-prevmillis_whel_stop>=5000)
      {
        rpm=0;
        kmhr=0;
        dist_t=0;
      }
  }
  prevstate = currentstate;
  bike_start();
}
