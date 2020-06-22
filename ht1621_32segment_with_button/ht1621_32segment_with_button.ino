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

HT1621 HT1621_lcd(data, wr, cs_lcd); // data,wr,rd,cs
DHT dht(DHTPIN, DHTTYPE);
File Data_Logger;
MFRC522 mfrc522(SS_PIN, RST_PIN);
//decrade function

////////open serial port //////////////
//#define DEBUG //debug to serial
//#define RAMFILL //SHOW ALL LCD RAM
//#define TESTSEGMENT_36PIN


#define RUNING_36PIN
#define GETSPEED // bike speed
#define SAVE_SD // sdcard code

//#define DEBUG_SPEED // debug bike speed to serial
//#define DEBUG_FAKE_VALUE_36
//#define DEBUG_Analog //Bat analog cal
//#define DEBUG_SD // debug sdcard to serial
/////////////////////////////////////////////////////
void bike_start();
boolean check_key();
void read_block_card(byte block,byte len,char *text_t);
/////////////////////////////////////////////////////
void update32();
void set_temp(int temp_val);
void set_vbat(int level);
void set_volt(float volt);
void set_dis(float Speed_total);
void set_km(float Speed);
void set_time(tmElements_t tm, int Mode = 0);

//buffer lcd charactor
unsigned char lcd_time_buffer[3] = {0x00, 0x00, 0x00};
unsigned char lcd_volt_buffer[3] = {0x00, 0x00, 0x00};
unsigned char lcd_dis_total_buffer[3] = {0x00, 0x00, 0x00};
unsigned char lcd_kmml_buffer[3] = {0x00, 0x00, 0x00};
unsigned char lcd_temp_buffer[3] = {0x00, 0x00, 0x00};
unsigned char lcd_vbat_buffer = 0x00;
//segment table
// รหัสแสดงผลสำหรับเวลาหลังนาทีและไมล์สะสม
unsigned char seg8to17[10] = {0xbf, 0x1c, 0x7b, 0x5F, 0xdc, 0xd7, 0xf7, 0x5d, 0xff, 0xdf};
//รหัสแสดงผลสำหรับแรงดันไฟปัจจุบันและเวลาหลักชั่วโมง
unsigned char seg0to7[10] = {0xfb, 0xc1, 0xB7, 0xf5, 0xcd, 0x7d, 0x7f, 0xd5, 0xff, 0xfd};
//21-24  26-29 - รหัสแสดงผลสำหรับไมล์และอุณหภูมิ
unsigned char seg21to24[10] = {0xfb, 0x61, 0xbd, 0xf5, 0x67, 0xd7, 0xdf, 0x75, 0xff, 0xf7};
//vbat 30-31 - รหัสแสดงผลสำหรับสถานะแบต
unsigned char seg_vbat[6] = {0x10, 0x12, 0x16, 0x1e, 0x1f, 0x9f};

// สำหรับเขียนไฟล์และแปลงเวลา
uint8_t hour12(uint8_t hour24);
char *strAmPm(uint8_t hour);
void wire_file(float kmhr_, tmElements_t tm, float dist_t, float temp_t, float rpm_t, float v_t);
float getVoltage();

//เก็บเวลาไว้เป็น global
tmElements_t tm;
// เก็บเวลาสำหรับคำนวณสถานะต่างๆ
unsigned long prevmillis = 0; //สำหรับเก็บเวลาที่ Hall เป็น HIGH ครั้งแรก
unsigned long prevmillis_whel_stop = 0; //เก็บเวลาว่า hall หยุดเปลี่ยนค่านานเท่าไหร่
unsigned long duration; // เก็บระยะเวลาทีเปลี่ยนสถานะจากครั้งแรกใช้เวลานานเท่าไหร่
unsigned long refresh; // refresh rate สำหรับการ update ค่า

//สำหรับวัดค่าสถานะต่างๆ
// rpm kmhr totaldistan
float rpm = 0.0; //สำหรับเก็บ rpm จากบันหยัดไตรยาง
float kmhr = 0.0; //สำหรับเก็บค่า kmhr จากการแปลง rpm
float dist_t = 0.0; //สำหรับเก็บระยะทางสะสม
//for temperature สำหรับอ่านค่า อุณหภูมิ
float temp = 0;
float tempbuffer = 0;
//for voltage สำหรับอ่าค่า voltage
float voltage = 0;
int batlevel = 0;

//deboud state ดีเบาค่าhallsensor
boolean currentstate; // Current state of IR input scan อ่าค่าปุจจุบันของเซ็นเซอร์
boolean prevstate = LOW; //จyบสถานะความแตกต่างของ hall

int mod = 0;
//refresh time mod
unsigned long saverefresh = 0;

boolean start_bike = 0;
boolean SD_state = 0;
byte access_id[4] = {0x77, 0x32, 0xBD, 0x5F};
char ACCESS_CODE[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char NAME[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char LNAME[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

#ifdef DEBUG_FAKE_VALUE_36
//for test สำหรับ fake value
float speedk = 0;
#endif

//สำหรับเทส lcd segment ทั้งหมด
#ifdef RAMFILL
void ramfill(int f, int l)
{
  for (int i = f ; i < l ; i++)
  {
    HT1621_lcd.writeMem(i, 0xf);
  }
}
#endif
//สำหรับค่าที่ออกจากจอจากสูงสุดถึงต่ำสุด
#ifdef DEBUG_FAKE_VALUE_36
void test_value();
#endif
void setup()
{
  // put your setup code here, to run once:
#ifdef DEBUG
  Serial.begin(9600);
#endif
  pinMode(hallpin, INPUT);
  pinMode(cs_sd, OUTPUT);
  pinMode(SS_PIN, OUTPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);
  
  SPI.begin();
  digitalWrite(SS_PIN, LOW);
  digitalWrite(cs_sd, HIGH);
  
  delay(50);
  mfrc522.PCD_Init();
  delay(1000);
  HT1621_lcd.begin(true);
  HT1621_lcd.sendCommand(HT1621::BIAS_THIRD_4_COM);  // FOR TS119
  HT1621_lcd.sendCommand(HT1621::SYS_EN);
  HT1621_lcd.sendCommand(HT1621::LCD_OFF);
  digitalWrite(RELAY, HIGH);
  while (start_bike == 0)
  {
    start_bike = check_key();
    #ifdef DEBUG
        Serial.print("BIKE KEY");
        Serial.println(start_bike);
    #endif
  }
  digitalWrite(RELAY, LOW);
  HT1621_lcd.sendCommand(HT1621::LCD_ON);

  digitalWrite(SS_PIN, HIGH);
  digitalWrite(cs_sd, LOW );
  delay(50);
  if (!SD.begin(cs_sd))
  {
    SD_state = 0;
    #ifdef DEBUG
      Serial.println("CARD FAILL");
      delay(1000);
    #endif
  }
  else
    SD_state = 1;
  Wire.begin();
  dht.begin();
}
void loop()
{
  //test_value();
#ifdef GETSPEED
  currentstate = digitalRead(hallpin); // Read hall sensor state
  if ( prevstate != currentstate) // If there is change in input
  {
    if ( currentstate == HIGH ) // If input only changes from LOW to HIGH
    {
      duration = ( micros() - prevmillis ); // Time difference between revolution in microsecond
      rpm = (60000000 / duration); // rpm = (1/ time millis)*1000*1000*60;
      prevmillis = micros(); // store time for nect revolution calculation
      prevmillis_whel_stop = millis();
      //v =3.14*0.0006069*(rpm*60);//speed(km/hr) = 3.14159*(D/100000)*(rpm*60) ; D (cm.)
      //k = d × r × 0.001885
      //1885/1000000
      kmhr = (60.69 * rpm * 0.001885);
      //Serial.println(kmhr);
      dist_t += 0.0006969;
      #ifdef DEBUG_SPEED
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
    if (millis() - prevmillis_whel_stop >= 5000)
    {
      rpm = 0;
      kmhr = 0;
      dist_t = 0;
      #ifdef DEBUG_SPEED
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
  prevstate = currentstate;
#endif
  //Serial.print(NAME);
  //Serial.println(LNAME);
  bike_start();
}
