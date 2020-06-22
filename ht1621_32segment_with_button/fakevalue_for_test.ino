#ifdef DEBUG_FAKE_VALUE_36
void test_value()
{
  set_time(tm);
  set_volt(speedk);
  set_dis(speedk);
  set_km(speedk);
  set_temp(20);
  update32();
  speedk = speedk + 0.1;
  batlevel += 1;
  tm.Hour += 1;
  tm.Minute += 1;
  tm.Second += 1;

  if (speedk > 199)
    speedk = 0;
  if (batlevel > 5)
    batlevel = 0;
  if (tm.Second > 59)
  {
    tm.Hour = 0;
    tm.Minute = 0;
    tm.Second = 0;
  }
}
#endif

#ifdef TESTSEGMENT_36PIN
void testallsegment36pin()
{
  for (int i = 0 ; i < 10 ; i++)
  {
    HT1621_lcd.writeMem2(0, seg0to7[i]);
    HT1621_lcd.writeMem2(2, seg0to7[i]);
    HT1621_lcd.writeMem2(4, seg0to7[i]);

    HT1621_lcd.writeMem2(6, seg0to7[i]);

    HT1621_lcd.writeMem2(8, seg8to17[i]);
    HT1621_lcd.writeMem2(10, seg8to17[i]);

    HT1621_lcd.writeMem2(12, seg8to17[i]);
    HT1621_lcd.writeMem2(14, seg8to17[i]);
    HT1621_lcd.writeMem2(16, seg8to17[i]);

    HT1621_lcd.writeMem2(18, 0x0C);
    HT1621_lcd.writeMem(20, 0x0);
    HT1621_lcd.writeMem2(21, seg21to24[i]);
    HT1621_lcd.writeMem2(23, seg21to24[i]);
    HT1621_lcd.writeMem(25, 0xf);
    HT1621_lcd.writeMem2(26, seg21to24[i]);
    HT1621_lcd.writeMem2(28, seg21to24[i]);
    HT1621_lcd.writeMem2(30, seg_vbat[5]);
    delay(1000);
  }
}
#endif
