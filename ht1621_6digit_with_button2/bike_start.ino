
void read_block_card(byte block, byte len, char *text_t)
{
  static char buffer[18] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  MFRC522::StatusCode status;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    #ifdef DEBUG
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    #endif
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer, &len);
  if (status != MFRC522::STATUS_OK) {
    #ifdef DEBUG
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    #endif
    return;
  }
  #ifdef DEBUG
  Serial.println(buffer);
  #endif
  strcpy(text_t, buffer);
}

boolean check_key()
{
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  int check=0;
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  /*for(int i=0;i<4;i++)
  {
    if(access_id[i]==mfrc522.uid.uidByte[i])
      check++;
    #ifdef DEBUG
    Serial.print("mfrc ");
    Serial.println(mfrc522.uid.uidByte[i],HEX);
    #endif
  }*/
  
  read_block_card(1, 18, NAME);
  read_block_card(2, 18, LNAME);
  read_block_card(4, 18, ACCESS_CODE);
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  if (strcmp(ACCESS_CODE, "ACCESS") == 0&&ACCESS_CODE!=NULL)
  {
    check = 4;
  }
  #ifdef DEBUG
  Serial.println(check);
  #endif
  if(check==4)
    return true;
  else
    return false;
}

void bike_start()
{
 if(digitalRead(A1)==LOW)
{
  mod=1;
  refresh1=millis();
}
if(digitalRead(A2)==LOW){
  mod=2;
  refresh2=millis();
}
if(digitalRead(A3)==LOW)
{
  mod=3;
  refresh3=millis();
}
#ifdef DEBUG_in_refresh
Serial.print("rpm in refhresh ");
Serial.println(rpm);
Serial.print("kmhr in refhresh ");
Serial.println(kmhr);
Serial.print("dist in refhresh ");
Serial.println(dist_t);
#endif
tmElements_t tm;
RTC.read(tm);
tempbuffer=dht.readTemperature();
if(isnan(tempbuffer)||tempbuffer==0)
  temp=temp;
else
  temp=tempbuffer;
voltage = getVoltage();
batlevel = (voltage>=34)?(int)(voltage-33):0;

if(mod==0)
  print_km(kmhr);
if(mod==1&&millis()-refresh1<=5000)
{
  #ifdef DEBUG_mod
  Serial.println("mod1");
  #endif
    lcd16set_time(tm);setdecimalseparator(3);
}
else
{
  if(mod==2&&millis()-refresh2<=5000)
  { 
    #ifdef DEBUG_mod
    Serial.println("mod2");
    #endif
     print_dist(dist_t);
  }
  else
  {
    if(mod==3&&millis()-refresh3<=5000)
    {
      #ifdef DEBUG_mod
      Serial.println("mod3");
      #endif
      print_temp_volt(temp,voltage);
    }
    else
    {
      mod=0;
    }
  }
}


if(millis()-saverefresh>=500)
{
  setBatteryLevel(batlevel);
  update16();
  if(kmhr>0&&SD_state!=0)
  {
    wire_file(kmhr,tm,dist_t,temp,rpm,voltage);
    
  }saverefresh=millis();
}
}
