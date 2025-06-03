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

#define WIDTH 32
#define HEIGHT 32
#define TOTAL_BITS (WIDTH * HEIGHT)
#define BITS_PER_PACKET 24
#define TOTAL_PACKETS ((TOTAL_BITS + BITS_PER_PACKET - 1) / BITS_PER_PACKET)

uint8_t imageBits[TOTAL_BITS]; // 0 o 1
bool received[TOTAL_BITS];  // Para saber qué bits ya llegaron

const char ID_PERMITIDO[] = "TX01"; 
uint8_t ID_EMISOR = 0b00111110;
uint8_t ID_RECEPTOR = 0b00111101;

void processPacket(byte* data, int length) {
  if (length < 4) return;

  byte sequence = data[0];
  if (sequence >= TOTAL_PACKETS) return;

  int baseIndex = sequence * BITS_PER_PACKET;
  for (int i = 0; i < 3; i++) {
    for (int b = 7; b >= 0; b--) {
      if (baseIndex < TOTAL_BITS) {
        byte bitValue = (data[i + 1] >> b) & 1;
        imageBits[baseIndex] = bitValue;
        received[baseIndex] = true;
        baseIndex++;
      }
    }
  }
}

void printImageProgressive() {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      int idx = i * WIDTH + j;
      if (!received[idx]) {
        Serial.print(" "); // No recibido
      } else {
        Serial.print(imageBits[idx] ? '#' : '.');
      }
    }
    Serial.println();
  }
}

void printByteBinary(uint8_t b) {
  for (int i = 7; i >= 0; i--) {
    Serial.print( (b & (1 << i)) ? '1' : '0' );
  }
  Serial.print(" ");
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
      Serial.print("Paquete recibido en binario: ");
      for (int i = 0; i < buflen; i++) {
        printByteBinary(buf[i]);
      }
      Serial.println();
    }
}


        // Convertir el mensaje recibido a cadena
/*         char mensaje[buflen + 1];
        memcpy(mensaje, buf, buflen);
        mensaje[buflen] = '\0'; // Asegurar el fin de cadena
        Serial.println(mensaje ); // Mostrar solo el contenido */


        // Verificar si el mensaje inicia con el ID permitido
/*         if (strncmp(&mensaje[8], ID_EMISOR, strlen(ID_EMISOR)) == 0) {
            digitalWrite(12, HIGH);
            Serial.print("Mensaje ACEPTADO de ");
            Serial.println(ID_PERMITIDO);
            Serial.print("Contenido: ");
            digitalWrite(12, LOW);

            
        } else {
            Serial.println("Mensaje rechazado: ID no coincide.");
        }
    }
} */

 /*        uint8_t cabecera = buf[0];
        
        uint8_t protocolo1[VW_MAX_MESSAGE_LEN - 1];
        memcpy(protocolo1, &buf[1], buflen - 1);

        uint8_t idEmisor = protocolo1[0];

        uint8_t protocolo2[VW_MAX_MESSAGE_LEN - 2];
        memcpy(protocolo2, &protocolo1[1], buflen - 2);

        char men[buflen + 1];
        memcpy(men, buf, buflen);
        men[buflen] = '\0';

        Serial.println("Recibido");
        Serial.println(cabecera);

        if (idEmisor == ID_EMISOR){
            uint8_t idReceptor = protocolo2[0];

            uint8_t protocolo3[VW_MAX_MESSAGE_LEN - 3];
            memcpy(protocolo3, &protocolo2[1], buflen - 3);

            if (idReceptor == ID_RECEPTOR){
                uint8_t mensaje[3];

                mensaje[0] = protocolo3[0];
                mensaje[1] = protocolo3[1];
                mensaje[2] = protocolo3[2];

                uint8_t checksum = protocolo3[4]; 

                //Esto debe ir dentro de un if que verifica el checksum
                processPacket(mensaje, 3);
                printImageProgressive();
            }
        }
    } */
