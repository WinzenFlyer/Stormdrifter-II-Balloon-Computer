/*
 * Program Code for Stormdrifter II Flight Board by Glinka Dynamics
 * Based on the EURUS Balloon Code by James Coxon (GPS), Anthony Stirk's NTX2 RTTY code and Tom Igoe's SD Library.
 * Version of June 2, 2014.
 */

#include <stdlib.h>
#include <util/crc16.h>
#include <string.h>
#include <OneWire.h>


//
// Variablen
//

// GPS Variables.
int32_t lat = 0, lon = 0, alt = 0, lat_dec = 0, lon_dec = 0;
uint8_t hour = 0, minute = 0, second = 0, lock = 0, sats = 0;
unsigned long startGPS = 0;
int GPSerrorM = 0, GPSerrorL = 0, GPSerrorP = 0, GPSerrorT = 0, count = 0, n, gpsstatus, lockcount = 0;
byte navmode = 99;
byte error = 0;
int lon_int = 0, lat_int = 0;


// Sensor and Datastring Variables/Datatypes.
int id = 0;

// Datastring CRC16 Checksum Mechanism
uint16_t crccat(char *msg)
{
  uint16_t x;
  for(x = 0xFFFF; *msg; msg++)
    x = _crc_xmodem_update(x, *msg);
  snprintf(msg, 8, "*%04X\n", x);
  return(x);
}

//Pin Definitions.
const int RADIOPIN = 0;
const int RED_LED = 22;
const int YELLOW_LED = 1;


//Temperature Chip I/0.
OneWire ds(2); // on digital pin 2 //init von DS18b20

// Buffer Definitions.
uint8_t buf[250]; // GPS receive buffer.
char dataString[300];
char DSTemperString[20]; // Temperature of the DS18B20.

// temperature humidity pressure Bmp DS18b20 HIH-5030
double temperature_DS = 0;

// Software Serial for Debugging.
//SoftwareSerial GeigerSerial(7, 6); // RX, TX //geiger ausgeschaltet

// SD Chip Select
const int chipSelect = 4;

// Mathematic
int pows_of_ten[4] = {1, 10, 100, 1000};


void setup()
{
  pinMode(chipSelect, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(RADIOPIN, OUTPUT);
  Serial.begin(9600); //GPS-Serial
  setupGPS();
  delay(100);
}

void loop()
{
  //
  //GPS einrichten
  //
  
  // GPS Control Routines, first check the Navigation Mode.
  gps_check_nav(); // gps ausgeschaltet
  // Check and set the navigation mode (Airborne, 1G).
  if(navmode != 6) {
    setupGPS();
  }
  
  
  
  //
  //GPS auslesen
  //
  
  gps_check_lock(); // Check if the GPS has Lock.
  gps_get_position(); // Read the Latitude, Longitude and Altitude.
  gps_get_time(); // Read the current Time.
  error = GPSerrorM + GPSerrorL + GPSerrorP + GPSerrorT; // Add up the errors.
  print_lat(); // Convert the Latitude into the proper format.
  print_lon(); // Convert the Longitude into the proper format.



  //
  //Lock anzeigen
  //
  
  // LED Control Routines
  if(lock != 3)
  {
    digitalWrite(RED_LED, HIGH);
  }
  else
  {
    digitalWrite(RED_LED, LOW);
  }



  //
  //Sensoren auslesen
  //
  
  // Read the Sensors, BMP085, DS18B20 and HIH-5030.
//  temperature = bmp.readTemperature(); //bmp //ausgeschaltet 
//  pressure = bmp.readPressure();  //bmp //ausgeschaltet
  temperature_DS = getTemp();  // ds18b20
//  Humidity = analogRead(A3);  //hih //ausgeschaltet

  // Read the HIH-6121.
/*  byte _status;
  unsigned int H_dat, T_dat;
  RH, T_C;

    _status = fetch_humidity_temperature(&H_dat, &T_dat);
  //HIH Errorcontrol
    switch(_status)
    {
    case 0:  
      Serial.println("Normal.");
      break;
    case 1:  
      Serial.println("Stale Data.");
      break;
    case 2:  
      Serial.println("In command mode.");
      break;
    default: 
      Serial.println("Diagnostic."); 
      break; 
    }       
  // Daten umrechnen zur besseren Lesbarkeit
    RH = (float) H_dat * 6.10e-3;
    T_C = (float) T_dat * 1.007e-2 - 40.0;
    print_float(RH, 1);
    print_float(T_C, 2);

  */  //hih ausgeschaltet
  
    // Convert the Sensor Data from Doubles into Strings.
    dtostrf(temperature_DS, 4, 2, DSTemperString);

  //
  // Read The Geiger Counter.
  //
  
  //    get_radiation(); //geiger ausgeschaltet



  //
  // String drucken und senden
  //
  
    // make a string for assembling the data to log:
    sprintf(dataString,"$$OERNEN-II,%d,%02d:%02d:%02d,%s%i.%07ld,%s%i.%07ld,%ld,%d,%d,%d,%d,%s", id, hour, minute, second, 
      lat < 0 ? "-" : "",lat_int,lat_dec, lon < 0 ? "-" : "",lon_int,lon_dec, alt, navmode, error, lock, sats, DSTemperString);

    // Append the checksum, skipping the $$ prefix.
    crccat(dataString + 2);

  // Deactivate the Interrupts, Broadcast the String and enable the Interrupts again.
  noInterrupts();
  rtty_txstring(dataString);
  interrupts();
  
  //
  // Kontrollsumme erhöhen
  //
  
  // Increase the count by 1.
  id++;
}


