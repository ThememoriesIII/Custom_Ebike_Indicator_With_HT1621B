void update16()
{
  //HT1621_lcd.memset(0, 0, 32);
  HT1621_lcd.writeMem2(0, lcd6digitBuffer[5]);
  HT1621_lcd.writeMem2(2, lcd6digitBuffer[4]);
  HT1621_lcd.writeMem2(4, lcd6digitBuffer[3]);
  HT1621_lcd.writeMem2(6, lcd6digitBuffer[2]);  
  HT1621_lcd.writeMem2(8, lcd6digitBuffer[1]);
  HT1621_lcd.writeMem2(10,lcd6digitBuffer[0]);
  #ifdef RAMDEBUG
  for(int i= 0;i<12;i++)
  {
    //Serial.print(i);
    //Serial.print(" ");
    //Serial.println(HT1621_lcd.readMem(i));
  }
  #endif
}

void print_temp_volt(float temp,float volte)
{
  char localbuffer[7];
  snprintf(localbuffer,7,"%2d %2d%1d",(int)temp,(int)volte,(int)(unsigned int)(volte*10)%10);
  for(int i=0;i<6;i++)
  {
    lcd6digitBuffer[i] &= 0x80;
    #ifdef DEBUG_temp_volt
    Serial.print(i);
    Serial.print(" ");
    Serial.println(localbuffer[i],HEX);
    #endif
    if(localbuffer[i]==' ')
      lcd6digitBuffer[i]=lcd6digitSegment[0];
    else
      if(localbuffer[i]=='-')
        lcd6digitBuffer[i]=lcd6digitSegment[10];
      else
        lcd6digitBuffer[i]=lcd6digitSegment[localbuffer[i]-0x30];
  }
  lcd6digitBuffer[2]=0x00;
  setdecimalseparator(1);
  //update32();
}


void lcd16set_time(tmElements_t tm)
{
  char localbuffer[7];
  snprintf(localbuffer,7," %2d%2d",tm.Hour,tm.Minute);
  for(int i=0;i<6;i++)
  {
    lcd6digitBuffer[i] &= 0x80;
    #ifdef DEBUG_time
    Serial.print(i);
    Serial.print(" ");
    Serial.println(localbuffer[i],HEX);
    #endif
    if(localbuffer[i]==' ')
      lcd6digitBuffer[i]=lcd6digitSegment[0];
    else
      if(localbuffer[i]=='-')
        lcd6digitBuffer[i]=lcd6digitSegment[10];
      else
        lcd6digitBuffer[i]=lcd6digitSegment[localbuffer[i]-0x30];
  }
  lcd6digitBuffer[0]=0x00;
  lcd6digitBuffer[5]=0x00;
  setdecimalseparator(3);
  //update32();
}

void print_km(float km){
  if(km>999.9)
    km=999.9;
  if(km<0)
    km=0;
  long ingegerpart = ((long)(km*pow(10,1)));
  char localbuffer[7];
  snprintf(localbuffer,7,"%6li",ingegerpart);
  for(int i=0;i<6;i++)
  {
    lcd6digitBuffer[i] &= 0x80;
    if(localbuffer[i]==0x20)
      lcd6digitBuffer[i]=lcd6digitSegment[0];
    else
      lcd6digitBuffer[i]=lcd6digitSegment[localbuffer[i]-0x30];
  } 
  setdecimalseparator(1);
  //setzero
  lcd6digitBuffer[0] &= 0x80;
  lcd6digitBuffer[1] &= 0x80;
  //km
  lcd6digitBuffer[0] |= 0x2F;
  lcd6digitBuffer[1] |= 0x46;
}

void print_dist(float dist){
  if(dist>999.9)
    dist=999.9;
  if(dist<0)
    dist=0;
  long ingegerpart = ((long)(dist*pow(10,3)));
  char localbuffer[7];
  snprintf(localbuffer,7,"%6li",ingegerpart);
  for(int i=0;i<6;i++)
  {
    lcd6digitBuffer[i] &= 0x80;
    if(localbuffer[i]==0x20)
      lcd6digitBuffer[i]=lcd6digitSegment[0];
    else
      lcd6digitBuffer[i]=lcd6digitSegment[localbuffer[i]-0x30];
  } 
  setdecimalseparator(3);
  //setzero
  lcd6digitBuffer[0] &= 0x80;
  //km
  lcd6digitBuffer[0] |= 0x6e;
}

void setdecimalseparator(int decimaldigits) 
{
   // zero out the eight bit
    lcd6digitBuffer[3] &= 0x7F;
    lcd6digitBuffer[4] &= 0x7F;
    lcd6digitBuffer[5] &= 0x7F;
  if( decimaldigits <= 0 || decimaldigits > 3){
    return;
  }
  lcd6digitBuffer[6-decimaldigits] |= 0x80;
}
void setBatteryLevel(int level) {
  // zero out the previous (otherwise the or couldn't be possible)
  if(level>3)
  level=3;
  if(level<0)
  level=0;
    lcd6digitBuffer[0] &= 0x7F;
    lcd6digitBuffer[1] &= 0x7F;
    lcd6digitBuffer[2] &= 0x7F;

  switch(level){
    case 3: // battery on and all 3 segments
      lcd6digitBuffer[0] |= 0x80;
    case 2: // battery on and 2 segments
      lcd6digitBuffer[1] |= 0x80;
    case 1: // battery on and 1 segment
      lcd6digitBuffer[2] |= 0x80;
    case 0: // battery indication off
    default:
      break;
  }
}
