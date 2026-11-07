#include <Wire.h>

#define EEPROM_I2C_ADDRESS 0x50 
#define EEPROM_PAGE_SIZE 256

void writeEEPROM(uint32_t address, uint8_t *data, size_t length) {
  size_t bytesWritten = 0;

  while (bytesWritten < length) {
    uint32_t currentAddress = address + bytesWritten;
    byte deviceAddress = EEPROM_I2C_ADDRESS | ((currentAddress >> 16) & 0x03);

    size_t pageOffset = currentAddress % EEPROM_PAGE_SIZE;
    size_t maxWriteThisPage = EEPROM_PAGE_SIZE - pageOffset;
  
    size_t bytesToInc = length - bytesWritten;
    if (bytesToInc > maxWriteThisPage) {
      bytesToInc = maxWriteThisPage;
    }
  
    // FIXED: Added the missing closing brace here
    if (bytesToInc > 28) {
      bytesToInc = 28;
    }
    
    Wire.beginTransmission(deviceAddress);
    Wire.write((byte)((currentAddress >> 8) & 0xFF));
    Wire.write((byte)(currentAddress & 0xFF));
    Wire.write(&data[bytesWritten], bytesToInc);
    Wire.endTransmission();
    delay(5);
    bytesWritten += bytesToInc;
  }
}

void readEEPROM(uint32_t address, uint8_t *buffer, size_t length) {
  size_t bytesRead = 0;

  while (bytesRead < length) {
    uint32_t currentAddress = address + bytesRead;
    byte deviceAddress = EEPROM_I2C_ADDRESS | ((currentAddress >> 16) & 0x03);
    size_t bytesToRequest = length - bytesRead;

    if (bytesToRequest > 30) {
      bytesToRequest = 30;
    }
    
    Wire.beginTransmission(deviceAddress);
    Wire.write((byte)((currentAddress >> 8) & 0xFF)); 
    Wire.write((byte)(currentAddress & 0xFF));        
    Wire.endTransmission(false); 

    // FIXED: Changed uint256_t to uint8_t
    uint8_t bytesReceived = Wire.requestFrom(deviceAddress, (byte)bytesToRequest);
    
    for (size_t i = 0; i < bytesReceived; i++) {
      if (Wire.available()) {
        buffer[bytesRead] = Wire.read();
        bytesRead++; 
      }
    }
  }
}

char readbyte() {
  Wire.beginTransmission(0x50);
  Wire.write(0x00);  
  Wire.write(0x64);  
  if (Wire.endTransmission(false) != 0) {
    return '?';
  }

  Wire.requestFrom(0x50, 1);
  if (Wire.available()) {
    return Wire.read();
  }
  return '?';
}

void setup() {
  Serial.begin(9600);
  Wire.begin(21, 22); 

  Serial.println("Welcome to the World....");
  
  char msg[] = "hello world";
  char msg2[]  = "This is 2026";
  writeEEPROM(250, (uint8_t*)msg, sizeof(msg)); 

  //writeEEPROM(0, (uint8_t*)msg2, sizeof(msg2));
}

void loop() {
  delay(1000);

  char buffer[12]; 
  readEEPROM(250, (uint8_t*)buffer, sizeof(buffer));
  
  Serial.print("Full String from 250 : ");
  Serial.println(buffer);
  
  Serial.print("Single Byte at 100: ");
   readEEPROM(100, (uint8_t*)buffer, sizeof(buffer));
  Serial.println("-------------------");
  
  readEEPROM(0, (uint8_t*)buffer, sizeof(buffer));
  
  Serial.print("Full String from 0 : ");
  Serial.println(buffer);

  delay(5000);
}