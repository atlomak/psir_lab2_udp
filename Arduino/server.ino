/*
 * Created on Mon Nov 15 2023
 *
 * Author Andrzej Tlomak
 *
 */

#include <Ethernet.h>
#include <EthernetUdp.h>

// ************ SENSORS ************

const int analogInPin = 4;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
int sensorValue = 0;        // value read from the pot

// ************ UDP ************

IPAddress address_ip = IPAddress(192, 168, 0, 101);
unsigned int localPort = 28848;

byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0xE7};
EthernetUDP Udp;

int r; // response

// ************ PROTOCOL ************

const int MSG_ID_SIZE = 7;
const int DATA_SIZE = 3; // 4 digits, max 9999

const char SERVER_HELLO[] = "HELLOA";
const char CLIENT_REQUEST[] = "GEDATA";
const char SEND_INFO[] = "SDATAA";

struct Protocol
{
    char type[MSG_ID_SIZE];
    char data[DATA_SIZE];
};

const int PROTOCOL_SIZE = sizeof(Protocol);

char buffer[PROTOCOL_SIZE];

Protocol message;

// ************ functions prototypes ************

int check_msg(char *message, int size);
int send_data(Protocol *message, int *sensorValue);
int send_hello(Protocol *message);

void setup()
{
    Serial.begin(9600);
    Ethernet.begin(mac);
    Serial.println(Ethernet.localIP());
    Udp.begin(localPort);
}

// ************ main loop ************

void loop()
{
    int packetSize = Udp.parsePacket();

    if (packetSize)
    {
        Serial.println("packet arrived");
        r = Udp.read(buffer, PROTOCOL_SIZE);

        int c = check_msg(buffer, r);

        Serial.print("Matching result: ");
        Serial.println(c);
        Serial.print("RECV: ");
        Serial.println(buffer);

        if (!c) // proper request recived
        {
            Serial.println("proper request recived");
            Serial.print("Sensor: ");
            Serial.println(sensorValue);

            r = send_data(&message, &sensorValue);

            Serial.print("send data status: ");
            Serial.println(r);

            Serial.print("SEND: ");
            Serial.print(message.type);
            Serial.print(" ");
            Serial.println(message.data);
        }
        else
        {
            Serial.println("wrong request recived");
            Serial.println(buffer);
        }
    }
    else
    {
        Serial.println("no packet yet");

        r = send_hello(&message);

        Serial.print("send hello status: ");
        Serial.println(r);

        Serial.print("SEND: ");
        Serial.print(message.type);
        Serial.print(" ");
        Serial.println(message.data);
    }
    Serial.println("**********************");
    delay(1550);
}

int check_msg(char *message, int size)
{
    return strncmp(message, CLIENT_REQUEST, MSG_ID_SIZE);
}

int send_data(Protocol *message, int *sensorValue)
{
    Udp.beginPacket(address_ip, 28848);

    strncpy(message->type, SEND_INFO, MSG_ID_SIZE);
    itoa(*sensorValue, message->data, 10); // convert int to char

    Udp.write((const char *)message, PROTOCOL_SIZE);

    return Udp.endPacket();
}

int send_hello(Protocol *message)
{
    Udp.beginPacket(address_ip, 28848);

    strncpy(message->type, SERVER_HELLO, MSG_ID_SIZE);
    strncpy(message->data, "0", DATA_SIZE); // clear data

    Udp.write((const char *)message, PROTOCOL_SIZE);

    return Udp.endPacket();
}
