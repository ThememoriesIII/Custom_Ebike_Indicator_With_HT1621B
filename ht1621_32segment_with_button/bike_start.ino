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
  int check = 0;
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  /*for (int i = 0; i < 4; i++)
    {
    if (access_id[i] == mfrc522.uid.uidByte[i])
      check++;
    #ifdef DEBUG
    Serial.print("mfrc ");
    Serial.println(mfrc522.uid.uidByte[i], HEX);
    #endif
    }
    #ifdef DEBUG
    Serial.println(check);
    #endif
  */

  read_block_card(1, 18, NAME);
  read_block_card(2, 18, LNAME);
  read_block_card(4, 18, ACCESS_CODE);
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  if (strcmp(ACCESS_CODE, "ACCESS") == 0&&ACCESS_CODE!=NULL)
  {
    check = 4;
  }

  if (check == 4)
    return true;
  else
    return false;
}

void bike_start()
{
#ifdef RUNING_36PIN
  RTC.read(tm);
  voltage = getVoltage();
  batlevel = (voltage >= 31) ? (int)(voltage - 31) : 0;
  tempbuffer = dht.readTemperature();
  if (isnan(tempbuffer) || tempbuffer == 0)
    temp = temp;
  else
    temp = tempbuffer;

  set_time(tm);
  set_volt(voltage);
  set_dis(dist_t);
  set_km(kmhr);
  set_vbat(batlevel);
  set_temp((int)temp);
#endif
#ifdef SAVE_SD
  if (millis() - saverefresh >= 500)
  {
    set_vbat(batlevel);
    update32();
    if (kmhr > 0 && SD_state != 0)
    {
      wire_file(kmhr, tm, dist_t, temp, rpm, voltage);

    }
    saverefresh = millis();
  }
#endif
}
