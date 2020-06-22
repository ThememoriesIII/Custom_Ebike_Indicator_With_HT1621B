float getVoltage()
{
  int value = analogRead(devidor_pin);
#ifdef DEBUG_Analog
  Serial.print("Analog");
  Serial.println(value);
#endif
  float caribate = 1.5;
  float vout = (value * 3.3) / 1023.0; // see text
  float vin = (vout / 0.104167) - caribate;
#ifdef DEBUG_Analog
  Serial.print("vout");
  Serial.println(vout);
  Serial.print("vin");
  Serial.println(vin);
#endif
  if (vin < 0.09) vin = 0.0;
  return vin;
}
