/*
ver ::cl 20120520
Configuracion basica para modulo receptor  RR 10
Utiliza libreria VirtualWire.h
pin 01 5v
pin 02 Tierra
pin 03 antena externa
pin 07 tierra
pin 10 5v
pin 11 tierra
pin 12 5v
pin 14 Arduino pin digital 2
pin 15 5v
*/

#include <VirtualWire.h>

#define TOTAL_BYTES 128

const uint8_t clave = 0x3F;

uint8_t imageBytes[TOTAL_BYTES];
bool recibido[43];

const uint8_t ID_EMISOR = 0b00000011;
const uint8_t ID_RECEPTOR = 0b00000011;

void llenarImg(uint8_t img[3], uint8_t cabecera){
  for (int i = 0; i <= 2; i++){
    imageBytes[3 * (cabecera - 1) + i] = img[i];
  }
  recibido[cabecera] = true;
}

void imprimirImg(){
  for (int i = 0; i <= TOTAL_BYTES - 1; i++){
    int cab = (i / 3) + 1;

    for (int j = 7; j >= 0; j--) {
      if (recibido[cab]){
        Serial.print((imageBytes[i] & (1 << j)) ? '1' : '0');
      }
      else{
        Serial.print(" ");
      }
    }
    if (((i+1) % 4 == 0) && ((i+1) != 0)){
      Serial.println();
    }
  }
}

void printByteBinary(uint8_t b) {
  for (int i = 7; i >= 0; i--) {
    Serial.print( (b & (1 << i)) ? '1' : '0' );
  }
  Serial.print(" ");
}

uint8_t calcularCRC8(const uint8_t *datos, size_t longitud) {
  uint8_t crc = 0x00;

  for (size_t i = 0; i < longitud; i++) {
    crc ^= datos[i];

    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x07;
      } else {
        crc <<= 1;
      }
    }
  }

  return crc;
}

void descifradoXOR(uint8_t* mensaje){
    for (int i = 3; i <= 5; i++) {
        mensaje[i] ^= clave;
    }
}

void setup(){
    Serial.begin(9600);
    Serial.println("Configurando Recepcion");
    vw_set_ptt_inverted(true); 
    vw_setup(2000);
    vw_set_rx_pin(2);
    vw_rx_start();
}

void loop(){
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if (vw_get_message(buf, &buflen)) {
      uint8_t cabecera = buf[0];
      descifradoXOR(buf);

      if (!recibido[cabecera]){
        uint8_t protocolo1[VW_MAX_MESSAGE_LEN - 1];
        memcpy(protocolo1, &buf[1], buflen - 1);

        uint8_t idEmisor = protocolo1[0];

        uint8_t protocolo2[VW_MAX_MESSAGE_LEN - 2];
        memcpy(protocolo2, &protocolo1[1], buflen - 2);

        if (idEmisor == ID_EMISOR){
          uint8_t idReceptor = protocolo2[0];

          uint8_t protocolo3[VW_MAX_MESSAGE_LEN - 3];
          memcpy(protocolo3, &protocolo2[1], buflen - 3);

          if (idReceptor == ID_RECEPTOR){
              uint8_t mensaje[3];

              Serial.println();

              digitalWrite(13,true);
              delay(200);
              digitalWrite(13,false);
              delay(200);

              mensaje[0] = protocolo3[0];
              mensaje[1] = protocolo3[1];
              mensaje[2] = protocolo3[2];

              uint8_t checksum = protocolo3[3]; 
              uint8_t paqueteSinCheck[6];

              for (int i = 0; i < 6; i++) {
                paqueteSinCheck[i] = buf[i];
              } 

              uint8_t check = calcularCRC8(paqueteSinCheck, 6);

              if (checksum == check){
                Serial.print("Paquete recibido en binario: ");
                for (int i = 0; i < buflen; i++) {
                  printByteBinary(buf[i]);
                }
                Serial.println();
                
                llenarImg(mensaje, cabecera);
                imprimirImg();
              }
          }
        }
      }     
    }
}

 
