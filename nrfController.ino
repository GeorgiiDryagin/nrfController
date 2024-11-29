
#include <SPI.h>          // библиотека для работы с шиной SPI
#include "nRF24L01.h"     // библиотека радиомодуля
#include "RF24.h"         // ещё библиотека радиомодуля

#define joyX A7
#define joyY A6
#define joyB A5

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги



byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

byte counter;
const byte dataSize = 3;
byte data[dataSize] = {0,0,0};
byte prev_data[dataSize] = {0,0,0};

bool isListsEqual(byte l1[], byte l2[], byte len){
  for (byte i = 0; i < len; i++)
    if (l1[i] != l2[i]) return false;
    
  return true;
}

byte toFiveOnEnd(byte n){
  return (n / 10) * 10 + 5;
}


void copyList(byte from[], byte to[], byte len){
  for (byte i = 0; i < len; i++)
    to[i] = from[i];
}

void updateData(){
  data[0] = toFiveOnEnd(map(analogRead(joyX),0,1024,0,255));
  data[1] = toFiveOnEnd(map(analogRead(joyY),0,1024,0,255));
  data[2] = !digitalRead(joyB);
}


void setup() {
  Serial.begin(9600);         // открываем порт для связи с ПК

  radio.begin();              // активировать модуль
  radio.setAutoAck(1);        // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();   // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);   // размер пакета, в байтах

  radio.openWritingPipe(address[0]);  // мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);             // выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX);   // уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp();        // начать работу
  radio.stopListening();  // не слушаем радиоэфир, мы передатчик

  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  pinMode(joyB, INPUT_PULLUP);
}

void loop() {
  updateData();
  if (!isListsEqual(data, prev_data, dataSize)){
    Serial.print("Sent: ");
    Serial.print(data[0]);Serial.print(" ");
    Serial.print(data[1]);Serial.print(" ");
    Serial.print(data[2]);Serial.print(" ");
    Serial.println("");
    radio.write(&data, sizeof(data));
    copyList(data, prev_data, dataSize);    
  }
  else  
    Serial.println(" wait ");
  delay(100);
}  
