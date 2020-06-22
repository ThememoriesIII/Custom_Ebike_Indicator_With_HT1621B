
//update segment ไปชิปไดฟ ht1621
void update32()
{
  HT1621_lcd.writeMem2(0, lcd_time_buffer[0]);
  HT1621_lcd.writeMem2(2, lcd_volt_buffer[2]);
  HT1621_lcd.writeMem2(4, lcd_volt_buffer[1]);

  HT1621_lcd.writeMem2(6, lcd_volt_buffer[0]);

  HT1621_lcd.writeMem2(8, lcd_time_buffer[1]);
  HT1621_lcd.writeMem2(10, lcd_time_buffer[2]);

  HT1621_lcd.writeMem2(12, lcd_dis_total_buffer[0]);
  HT1621_lcd.writeMem2(14, lcd_dis_total_buffer[1]);
  HT1621_lcd.writeMem2(16, lcd_dis_total_buffer[2]);

  HT1621_lcd.writeMem2(18, 0x0C);
  HT1621_lcd.writeMem(20, 0x0);
  HT1621_lcd.writeMem2(21, lcd_kmml_buffer[2]);
  HT1621_lcd.writeMem2(23, lcd_kmml_buffer[1]);
  HT1621_lcd.writeMem(25, 0xf);
  HT1621_lcd.writeMem2(26, lcd_temp_buffer[2]);
  HT1621_lcd.writeMem2(28, lcd_temp_buffer[1]);
  HT1621_lcd.writeMem2(30, lcd_vbat_buffer);
}

void set_temp(int temp_val)
{
  if (temp_val > 199.0)
    temp_val = 199;
  if (temp_val < 0.0)
    temp_val = 0;
  char localbuffer[4];
  snprintf(localbuffer, 4, "%3d", (unsigned int)temp_val);
  for (int i = 1; i < 3; i++)
  {
    if (localbuffer[i] == 0x20)
      lcd_temp_buffer[i] = seg21to24[0];
    else
      lcd_temp_buffer[i] = seg21to24[localbuffer[i] - 0x30];
  }
  if (temp_val < 100)
  {
    lcd_temp_buffer[1] = lcd_temp_buffer[1] & 0xfe;
    lcd_temp_buffer[2] = lcd_temp_buffer[2] & 0xfe;
  }
}

//set vbat segment
void set_vbat(int level)
{
  if (level > 5)
    level = 5;
  if (level < 0)
    level = 0;
  if (level >= 0 && level <= 5)
    lcd_vbat_buffer = seg_vbat[level];
}
//set volt segment
void set_volt(float volt)
{
  int presition = 1;
  if (volt > 199.0)
    volt = 199.0;
  if (volt < 0.0)
    volt = 0.0;
  char localbuffer[4];
  if (volt < 100.0)
  {
    snprintf(localbuffer, 4, "%3d", (unsigned int)(volt * 10));
  }
  else
  {
    //Serial.println("in un presition");
    presition = 0;
    snprintf(localbuffer, 4, "%3d", (unsigned int)volt);
  }
  for (int i = 0; i < 3; i++)
  {
    if (localbuffer[i] == 0x20)
      lcd_volt_buffer[i] = seg0to7[0];
    else
      lcd_volt_buffer[i] = seg0to7[localbuffer[i] - 0x30];
  }
  if (presition == 0)
    lcd_volt_buffer[2] = lcd_volt_buffer[2] & 0xfe;
}
// set distan km segment
void set_dis(float Speed_total)
{
  int presition = 0;
  if (Speed_total > 199.0)
    Speed_total = 199;
  if (Speed_total < 0.0)
    Speed_total = 0;
  char localbuffer[4];
  if (Speed_total < 100.0)
  {
    presition = 1;
    snprintf(localbuffer, 4, "%3d", (unsigned int)(Speed_total * 10));
  }
  else
    snprintf(localbuffer, 4, "%3d", (unsigned int)Speed_total);
  for (int i = 0; i < 3; i++)
  {
    if (localbuffer[i] == 0x20)
      lcd_dis_total_buffer[i] = seg8to17[0];
    else
      lcd_dis_total_buffer[i] = seg8to17[localbuffer[i] - 0x30];
  }
  if (presition == 0)
    lcd_dis_total_buffer[2] &= 0xef;
  //update32();
}

void set_km(float Speed)
{
  if (Speed > 199.0)
    Speed = 199;
  if (Speed < 0.0)
    Speed = 0;
  char localbuffer[4];
  snprintf(localbuffer, 4, "%3d", (unsigned int)Speed);
  for (int i = 1; i < 3; i++)
  {
    if (localbuffer[i] == 0x20)
      lcd_kmml_buffer[i] = seg21to24[0];
    else
      lcd_kmml_buffer[i] = seg21to24[localbuffer[i] - 0x30];
  }
  if (Speed < 100)
    lcd_kmml_buffer[1] = lcd_kmml_buffer[1] & 0xfe;
}

void set_time(tmElements_t tm, int Mode = 0)
{
  tm.Hour = hour12(tm.Hour);
  if (tm.Hour > 12)
    tm.Hour = 12;
  lcd_time_buffer[0] = (tm.Hour < 10) ? (0xfe & seg0to7[tm.Hour]) : seg0to7[tm.Hour - 10];
  if (tm.Minute >= 10)
  {
    lcd_time_buffer[1] = seg8to17[tm.Minute / 10];
    lcd_time_buffer[2] = seg8to17[tm.Minute % 10];
  }
  else
  {
    lcd_time_buffer[1] = seg8to17[0];
    lcd_time_buffer[2] = seg8to17[tm.Minute];
  }
}
