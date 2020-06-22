/// check sd card one time in set up
void wire_file(float kmhr_t, tmElements_t tm, float dist_t, float temp_t, float rpm_t, float v_t)
{
#ifdef DEBUG_SD
  Serial.println("Connec Card Done");
#endif
  char name_File[20];
  sprintf(name_File, "%d_%d_%d.csv", tm.Day, tm.Month, (tmYearToCalendar(tm.Year) + 543) - 2500);
  //int o = Data_Logger.open(name_File,O_RDWR | O_CREAT | O_AT_END);
  Data_Logger = SD.open(name_File, FILE_WRITE);
#ifdef DEBUG_SD
  Serial.println(name_File);
#endif
  //SD.exists(name_File);
  if (Data_Logger)
  {
    //String datelog;
    char datelog[80];
#ifdef DEBUG_SD
    Serial.println("Open File Done");

    Serial.print("k ");
    Serial.print(kmhr_t);
    Serial.print(" d ");
    Serial.print(dist_t);
    Serial.print(" t ");
    Serial.println(temp_t);
#endif
    sprintf(datelog, "%d/%d/%d,%d:%d:%d,%d.%2dKm/Hr,%d.%3.3dKm,%d.%1d'C,%d.%2dRPM,%dV,%s %s",
            tm.Day,
            tm.Month,
            tmYearToCalendar(tm.Year) + 543,
            tm.Hour, tm.Minute,
            tm.Second,
            (int)kmhr_t, ((unsigned int)((kmhr_t * 100)) % 100),
            (int)dist_t, ((unsigned int)((dist_t * 1000)) % 1000),
            (int)temp_t, ((unsigned int)((temp_t * 10)) % 10),
            (int)rpm_t, ((unsigned int)((rpm_t * 100)) % 100),
            (int)v_t, NAME, LNAME);
#ifdef DEBUG_SD
    Serial.println(datelog);
#endif
    Data_Logger.println(datelog);
    Data_Logger.close();
  }
  else
  {
#ifdef DEBUG_SD
    Serial.print("cant open file ");
    Serial.println(name_File);
#endif
  }
}


////// check sed card every time
/*void wire_file(float kmhr_t,tmElements_t tm,float dist_t,float temp_t,float rpm_t,float v_t)
  {
  if (!SD.begin(cs_sd))
  {
    #ifdef DEBUG_SD
      Serial.println("Card Fail");
    #endif
    return;
  }
  else
  {
    #ifdef DEBUG_SD
      Serial.println("Connec Card Done");
    #endif
    char name_File[20];
    sprintf(name_File, "%d_%d_%d.csv",tm.Day,tm.Month,(tmYearToCalendar(tm.Year)+543)-2500);
    //int o = Data_Logger.open(name_File,O_RDWR | O_CREAT | O_AT_END);
    Data_Logger = SD.open(name_File,FILE_WRITE);
    #ifdef DEBUG_SD
      Serial.println(name_File);
    #endif
    //SD.exists(name_File);
    if (Data_Logger)
    {
      //String datelog;
      char datelog[80];
      #ifdef DEBUG_SD
        Serial.println("Open File Done");

      Serial.print("k ");
      Serial.print(kmhr_t);
      Serial.print(" d ");
      Serial.print(dist_t);
      Serial.print(" t ");
      Serial.println(temp_t);
      #endif
      sprintf(datelog,"%d/%d/%d,%d:%d:%d,%d.%2dKm/Hr,%d.%3.3dKm,%d.%1d'C,%d.%2dRPM,%dV",
      tm.Day,
      tm.Month,
      tmYearToCalendar(tm.Year)+543,
      tm.Hour,tm.Minute,
      tm.Second,
      (int)kmhr_t,((unsigned int)((kmhr_t*100))%100),
      (int)dist_t,((unsigned int)((dist_t*1000))%1000),
      (int)temp_t,((unsigned int)((temp_t*10))%10),
      (int)rpm_t,((unsigned int)((rpm_t*100))%100),
      (int)v_t);
      #ifdef DEBUG_SD
      Serial.println(datelog);
      #endif
      Data_Logger.println(datelog);
      Data_Logger.close();
    }
    else
    {
      #ifdef DEBUG_SD
        Serial.print("cant open file ");
        Serial.println(name_File);
      #endif
    }
  }
  }*/
