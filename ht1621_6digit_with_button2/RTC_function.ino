uint8_t hour12(uint8_t hour24)
{
    if (hour24 == 0)return 12;
    if (hour24 > 12)return (hour24 - 12);
    return hour24;
}

char *strAmPm(uint8_t hour)
{
  return (hour<12)?"AM":"PM";
}
