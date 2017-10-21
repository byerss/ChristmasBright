#include <Wire.h>
#include <Flash.h>
#include "RTClib.h"


RTC_DS1307 rtc;

String minPad;
String secPad;

int relaystatus = 0;

long previousMillis = 0;
long interval = 5000; 

int powerRelay[] = {2,3,4,5,6,7,8,9};
boolean relayStatus[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
boolean state0[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}; //OFF
boolean state1[] = {LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH}; //DIMMED
boolean state2[] = {HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW}; //FULL BRIGHTNESS


int setTime;
int setAbs;
int riseTime;
int riseAbs;
int nowAbs;
int nowHour;
int nowMinute;

int setHour;
int setMin;
int riseHour;
int riseMin;

int pmOFFhour;
int pmOFFmin;
int amONhour;
int amONmin;
int dimTimehour;
int dimTimemin;


//NUMBER OF MINUTES OFFSET FROM SUNRISE/SUNSET EVENT
//POSITIVE NUMBER IS AFTER THE EVENT, NEGATIVE IS BEFORE EVENT

int eveningOffset = -10;   //10 MINTUES BEFORE SUNSET
int morningOffset = 5;  //5 MINUTES AFTER SUNRISE

//MORNING ON TIME
int amONtime = 630;

//EVENING OFF TIME
int pmOFFtime = 2330; //11:30 PM

//EVENING DIM TIME
int dimTime = 2200; //10 PM


//SUNRISE DATA FOR TIGARD, OR (ALL TIMES PST - DOES NOT ACCOUNT FOR DST)
FLASH_TABLE(int, sunRise, 31, 
    {751,751,751,751,751,751,750,750,750,750,749,749,748,748,747,747,746,745,745,744,743,742,742,741,740,739,738,737,736,735,733},
    {732,731,730,729,727,726,725,723,722,721,719,718,716,715,713,712,710,709,707,705,704,702,700,659,657,655,654,652,650},
    {648,646,645,643,641,639,637,636,634,632,630,628,626,624,622,621,619,617,615,613,611,609,607,605,603,602,600,558,556,554,552},
    {550,548,547,545,543,541,539,537,535,534,532,530,528,526,525,523,521,519,518,516,514,513,511,509,508,506,505,503,502,500},
    {459,457,456,454,453,451,450,449,447,446,445,444,442,441,440,439,438,437,436,435,434,433,432,431,430,430,429,428,428,427,426},
    {426,425,425,424,424,424,423,423,423,422,422,422,422,422,422,422,422,422,422,423,423,423,423,424,424,424,425,425,426,426},
    {427,428,428,429,430,430,431,432,433,433,434,435,436,437,438,439,440,441,442,443,444,445,446,447,448,449,450,451,453,454,455},
    {456,457,458,459,501,502,503,504,505,507,508,509,510,511,513,514,515,516,517,519,520,521,522,524,525,526,527,528,530,531,532},
    {533,535,536,537,538,539,541,542,543,544,545,547,548,549,550,552,553,554,555,556,558,559,600,601,603,604,605,606,608,609},
    {610,611,613,614,615,616,618,619,620,622,623,624,625,627,628,629,631,632,633,635,636,638,639,640,642,643,644,646,647,649,650},
    {651,653,654,656,657,658,700,701,702,704,705,707,708,709,711,712,713,715,716,718,719,720,721,723,724,725,726,728,729,730},
    {731,732,733,735,736,737,738,739,740,740,741,742,743,744,745,745,746,747,747,748,748,749,749,749,750,750,750,751,751,751,751}
  );
  
  
//SUNSET DATA FOR TIGARD, OR (ALL TIMES PST - DOES NOT ACCOUNT FOR DST)
FLASH_TABLE(int, sunSet, 31,
    {1638,1639,1640,1641,1642,1643,1644,1646,1647,1648,1649,1650,1651,1653,1654,1655,1657,1658,1659,1701,1702,1703,1705,1706,1707,1709,1710,1712,1713,1715,1716},
    {1717,1719,1720,1722,1723,1725,1726,1728,1729,1731,1732,1733,1735,1736,1738,1739,1741,1742,1744,1745,1746,1748,1749,1751,1752,1753,1755,1756,1758},
    {1759,1800,1802,1803,1804,1806,1807,1809,1810,1811,1813,1814,1815,1817,1818,1819,1821,1822,1823,1824,1826,1827,1828,1830,1831,1832,1834,1835,1836,1837,1839},
    {1840,1841,1843,1844,1845,1846,1848,1849,1850,1852,1853,1854,1855,1857,1858,1859,1901,1902,1903,1904,1906,1907,1908,1910,1911,1912,1913,1915,1916,1917},
    {1918,1920,1921,1922,1923,1925,1926,1927,1928,1930,1931,1932,1933,1934,1936,1937,1938,1939,1940,1941,1942,1943,1944,1945,1946,1947,1948,1949,1950,1951,1952},
    {1953,1954,1954,1955,1956,1957,1957,1958,1959,1959,2000,2000,2001,2001,2002,2002,2002,2003,2003,2003,2003,2003,2003,2004,2004,2004,2004,2003,2003,2003},
    {2003,2003,2002,2002,2002,2001,2001,2000,2000,1959,1959,1958,1957,1957,1956,1955,1954,1953,1953,1952,1951,1950,1949,1948,1947,1945,1944,1943,1942,1941,1939},
    {1938,1937,1935,1934,1933,1931,1930,1928,1927,1925,1924,1922,1921,1919,1917,1916,1914,1912,1911,1909,1907,1905,1904,1902,1900,1858,1857,1855,1853,1851,1849},
    {1848,1846,1844,1842,1840,1838,1836,1834,1832,1830,1829,1827,1825,1823,1821,1819,1817,1815,1813,1811,1809,1807,1805,1804,1802,1800,1758,1756,1754,1752},
    {1750,1748,1746,1745,1743,1741,1739,1737,1735,1733,1732,1730,1728,1726,1725,1723,1721,1719,1718,1716,1714,1713,1711,1709,1708,1706,1705,1703,1702,1700,1659},
    {1657,1656,1655,1653,1652,1651,1649,1648,1647,1646,1645,1644,1642,1641,1640,1639,1638,1638,1637,1636,1635,1634,1634,1633,1632,1632,1631,1631,1630,1630},
    {1629,1629,1629,1628,1628,1628,1628,1628,1628,1628,1628,1628,1628,1628,1629,1629,1629,1630,1630,1630,1631,1632,1632,1633,1633,1634,1635,1636,1636,1637,1638}
  );
  
  
  void setup() {
  
  
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);  
   
   
   for (int i = 0; i < 8; i++) {
     pinMode(powerRelay[i], OUTPUT);
     digitalWrite(powerRelay[i], HIGH);
   }
   
   //digitalWrite(powerRelay[0], HIGH); //Why does first item write low?
    
  
  
//START RTC  
//RTC MUST BE SET TO PST FOR SUN DATA TABLES TO WORK PROPERLY

  #ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();

  if (! rtc.isrunning()) {
    Serial.println("RTC Error");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
 
}
  
  
  
 void loop() {
  
    
    DateTime now = rtc.now();

    riseTime = sunRise[now.month() - 1][now.day() - 1];  //-1 SINCE ARRAYS ARE ZERO INDEXED
    delay(5); //Needed for some reason or else second table lookup doesnt work
    setTime = sunSet[now.month() - 1][now.day() - 1];   //-1 SINCE ARRAYS ARE ZERO INDEXED

      
    nowAbs =  (now.hour() * 60) + now.minute();
    nowHour = now.hour();
    nowMinute = now.minute();
    setHour = setTime/100;
    setMin = setTime - (setHour * 100);
    riseHour = riseTime/100;
    riseMin = riseTime - (riseHour * 100);
    
    setAbs = setHour*60 + setMin;
    riseAbs = riseHour*60 + riseMin;
    
    //setAbs =  (setTime/100 * 60) + (setTime - (setTime/100 * 60));
    //riseAbs = (riseTime/100 * 60) + (riseTime - (riseTime/100 * 60));
    
    //int amONtimeAbs = (amONtime/100)*60 + (amONtime - (amONtime/100)*60);
    //int pmOFFtimeAbs = (pmOFFtime/100)*60 + (pmOFFtime - (pmOFFtime/100)*60);

    amONhour = amONtime/100;
    amONmin = amONtime - (amONhour * 100);
    pmOFFhour = pmOFFtime/100;
    pmOFFmin = pmOFFtime - (pmOFFhour * 100);
    dimTimehour = dimTime/100;
    dimTimemin = dimTime - (dimTimehour * 100);
    
    int amONtimeAbs = amONhour*60 + amONmin;
    int pmOFFtimeAbs = pmOFFhour*60 + pmOFFmin;
    int dimTimeAbs = dimTimehour*60 + dimTimemin;
  
  //TURN ON BETWEEN (eveningOffset) MINUTES FROM SUNSET AND (pmOFFtime)
   if ( (nowAbs - setAbs) >= eveningOffset && (nowAbs) <= pmOFFtimeAbs ) {
     
     if (nowAbs < dimTimeAbs) { //FULL BRIGHTNESS ON FROM SUNSET UNTIL DIM TIME
        for (int i = 0; i < 8; i++) {
         relayStatus[i] = state2[i];  }
     relaystatus = 2;
     Serial.println("Evening Full Brightness");
    // Serial.println(dimTimeAbs - nowAbs);
     }
     
     else {     //DIMMED AFTER DIM TIME TO OFF TIME
       for (int i = 0; i < 8; i++) {
       relayStatus[i] = state1[i]; 
        
    }
     relaystatus = 1;
     Serial.println("Evening Dimmed");
     
       
     }
     
     
     
   }
   
  //TURN ON BETWEEN (amONtime UNTIL (morningOffset) MINUTES FROM SUNRISE
   else if ( (nowAbs - riseAbs) <= morningOffset && (nowAbs) >= amONtimeAbs ) {
      for (int i = 0; i < 9; i = i++) {
         relayStatus[i] = state2[i];
     }
      relaystatus = 2;
      Serial.println("Morning ON");
   }
   
    
   //TURN OFF ALL OTHER TIMES
   else {
      for (int i = 0; i < 8; i++) {
         relayStatus[i] = state0[i];
     }
      relaystatus = 0; 
      Serial.println("OTHER TIME OFF");
    }
  
  
  //ACTUATE RELAYS
  for (int i = 0; i < 8; i++) {
         digitalWrite(powerRelay[i], relayStatus[i]);
         
     }
     
     
     
     

unsigned long currentMillis = millis();

 
  if(currentMillis - previousMillis > interval) {
        
     Serial.print("CURRENT TIME: ");
     Serial.print(nowHour);
     Serial.print(":");
     Serial.println(nowMinute);
     
     Serial.print("RISE TIME: ");
     Serial.print(riseHour);
     Serial.print(":");
     Serial.println(riseMin);
     
     Serial.print("SET TIME: ");
     Serial.print(setHour);
     Serial.print(":");
     Serial.println(setMin);
     
     Serial.print("AM ON TIME: ");
     Serial.print(amONhour);
     Serial.print(":");
     Serial.println(amONmin);
     
     Serial.print("PM OFF TIME: ");
     Serial.print(pmOFFhour);
     Serial.print(":");
     Serial.println(pmOFFmin);
     
     Serial.print("PM DIM TIME: ");
     Serial.print(dimTimehour);
     Serial.print(":");
     Serial.println(dimTimemin);
 /*    
     for(int i = 0; i < 8; i++) {
     Serial.println(relayStatus[i]);
     }
 */    
     previousMillis = currentMillis;
     
  }

  
   
   
delay(5000);
  
  
 } 
  
  
  
  
  
  
  
  
  
  
  
