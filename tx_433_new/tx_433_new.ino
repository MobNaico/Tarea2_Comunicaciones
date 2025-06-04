#include <VirtualWire.h>
#include <avr/pgmspace.h>

const uint8_t ID_EMISOR = 0x03;
const uint8_t ID_RECEPTOR = 0x03;

const int LED_PIN = 13;
const char matriz_txt[] PROGMEM =
    "00000000000111110000000000000000"
    "00000000000110100000000000000000"
    "00000000000100100000000000000001"
    "00000000000100100000000000000001"
    "00000000000100100000000000000000"
    "00000000000100100000000000000000"
    "00000000000100100000000000000000"
    "00000000000110100000000000000000"
    "00000000000110100000000000000000"
    "00000000000110111000000000000000"
    "00000011111000111000000000000000"
    "00001110000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000100000000"
    "00000000000000000000000010000000"
    "00000000000000000000000010000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000";

void setup() {
    vw_set_ptt_inverted(true);
    vw_setup(2000);
    vw_set_tx_pin(2);
    Serial.begin(9600);
    Serial.println("Iniciando transmisión...");

    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    const int BITS_POR_MENSAJE = 24;
    const int TOTAL_BITS = 1024;
    const int TOTAL_PAQUETES = (TOTAL_BITS + BITS_POR_MENSAJE - 1) / BITS_POR_MENSAJE;

    for (int paquete_num = 0; paquete_num < TOTAL_PAQUETES; paquete_num++) {
        uint8_t paquete[6];  // [cabecera][id_emisor][id_receptor][mensaje(3)][checksum]

        paquete[0] = paquete_num + 1;
        paquete[1] = ID_EMISOR;
        paquete[2] = ID_RECEPTOR;

        uint8_t byte0 = 0, byte1 = 0, byte2 = 0;
        for (int i = 0; i < BITS_POR_MENSAJE; i++) {
            int bit_idx = paquete_num * BITS_POR_MENSAJE + i;
            char bit_char = (bit_idx < TOTAL_BITS) ? pgm_read_byte_near(matriz_txt + bit_idx) : '0';
            uint8_t bit_val = (bit_char == '1') ? 1 : 0;

            if (i < 8)
                byte0 |= (bit_val << (7 - i));
            else if (i < 16)
                byte1 |= (bit_val << (15 - i));
            else
                byte2 |= (bit_val << (23 - i));
        }

        paquete[3] = byte0;
        paquete[4] = byte1;
        paquete[5] = byte2;

        uint8_t checksum = 0;
        for (int i = 0; i < 5; i++) {
            checksum += paquete[i];
        }

        uint8_t paquete_envio[7];
        for (int i = 0; i < 6; i++) {
            paquete_envio[i] = paquete[i];
        }
        paquete_envio[6] = checksum;

        digitalWrite(LED_PIN, HIGH);
        vw_send(paquete_envio, 7);
        vw_wait_tx();
        digitalWrite(LED_PIN, LOW);

        Serial.print("Paquete ");
        Serial.print(paquete_num + 1);
        Serial.print(" enviado: [");

        for (int i = 0; i < 7; i++) {
            printByteBin(paquete_envio[i]);
            if (i < 6) Serial.print(", ");
        }
        Serial.println("]");


        delay(2000);
    }

    Serial.println("Transmisión completa.");

}

void printByteBin(uint8_t b) {
    for (int i = 7; i >= 0; i--) {
        Serial.print(bitRead(b, i));
    }
}
