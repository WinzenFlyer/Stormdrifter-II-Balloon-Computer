/* Controls the DS18B20 temperature sensor and HIH-4030 Humidity sensor and also measures battery voltage from the voltage divider on the board. 
*  The BMP085 is now controlled by using the Adafruit Library for it.
*/


/* Reads Temperature Sensor DS18B20 in DEG CELSIUS
 *
 */
float getTemp()
{
  byte data[12];
  byte addr[8];
  
  if( !ds.search(addr))
  {
    ds.reset_search();
    return -1000;
  }
  
  if( OneWire::crc8( addr, 7) != addr[7])
  {
    //Serial.println("CRC is not valid!");  //No Serialconnection used
    return -1001;
  }
  
  if( addr[0] != 0x10 && addr[0] != 0x28)
  {
    //Serial.print("Device is not recognized");  //NO serial Connection used
    return -1002;
  }
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion with parasite power on at the end
  
//  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad
  
  for (int i = 0; i < 9; i++)
  { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];
  
  float tempRead = ((MSB << 8) | LSB); // using two's complement
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;

}
