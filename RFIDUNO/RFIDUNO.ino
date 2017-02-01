#include <MFRC522.h>
#include <SPI.h>
#include <Servo.h>
#include <Wire.h> 

int led=7;
Servo myservo; 
int pos=0;
/*
Pins	SPI	   UNO  
1 (NSS) SAD (SS)   10    
2       SCK        13   
3       MOSI       11    
4       MISO       12   
5       IRQ        *      
6       GND       GND    
7       RST        5      
8      +3.3V (VCC) 3V3   
* Not needed
1 on ICPS header
*/
#define SAD 10
#define RST 9
MFRC522 nfc(SAD, RST);
int ABIERTO=80;

void setup() {
   myservo.attach(3);  
  

   SPI.begin();
   Serial.begin(9600);
   Serial.println("BUSCANDO MFRC522.");
   nfc.begin();
   byte version = nfc.getFirmwareVersion();
   if (! version) {
    Serial.print("NO SE ENCONTRO MFRC522 ");
    while(1); //halt
   }
   myservo.write(180); 
   Serial.print("BUSCANDO CHIP MFRC522 ");
   Serial.print("FIRMWARE VERSION. 0x");
   Serial.print(version, HEX);
   Serial.println(".");
}

#define TARJETA 1 
#define LLAVE 1 
//CLAVE DE LA TAJETA
byte Autorizado[TARJETA][4] = {{0x53, 0xE9, 0xC8, 0xF7, }};
// CLAVE DEL LLAVERO
byte Autorizado2[LLAVE][4] = {{0x83, 0x10, 0xF6, 0xE2, }}; 
void imprimeClave(byte *serial);
boolean esIgual(byte *key, byte *serial);
boolean chekaKey(byte *serial);

void loop() {

 

  //Serial.print("CERRADO        ");
  byte status;
  byte data[MAX_LEN];
  byte serial[5];
  boolean Abierto = false;
  
  status = nfc.requestTag(MF1_REQIDL, data);
  myservo.write(180);
  
  if (status == MI_OK) {
    status = nfc.antiCollision(data);
    memcpy(serial, data, 5);    
    if(chekaKey(serial)){ 
      Serial.println("AUTORIZADO");
       //imprimeClave(serial);
  
      Abierto = true;
      myservo.write(90);
    }else{ 
      //imprimeClave(serial);
      
      Serial.println("NO AUTORIZADO");
      Abierto = false;
    
    }    
    nfc.haltTag();
 
    delay(2000);
  }
  delay(500);
}

boolean esIgual(byte *key, byte *serial){
    for (int i = 0; i < 4; i++){
      if (key[i] != serial[i]){ 
        return false; 
      }
    }    
    return true;
  }

boolean chekaKey(byte *serial)
{
    for(int i = 0; i<TARJETA; i++)
    {
      if(esIgual(serial, Autorizado[i]))
        return true;
    }
      for(int i = 0; i<LLAVE; i++)
    {
      if(esIgual(serial, Autorizado2[i]))
        return true;
    }
   return false;
}

void imprimeClave(byte *serial)
{
    Serial.print("CLAVE: ");
    for (int i = 0; i < 4; i++) {
      Serial.print(serial[i], HEX);
      Serial.print(" ");
    }
}
