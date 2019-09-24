#include <nRF24L01.h>
#include <RF24.h>
 
RF24 radio(7, 10);

String inString = "";                       //incoming str from COM
String dataToBeTransmitted = "";

void setup() {

  radio.begin();
  
  /* Указываем канал передачи данных (от 0 до 127) (на одном канале может быть только 1 приёмник и до 6 передатчиков). Выбираем канал в котором нет шумов! */
  radio.setChannel(0x30);
  
  /* Указываем скорость передачи данных RF24_250KBPS = 250Кбит/сек RF24_1MBPS = 1Мбит/сек RF24_2MBPS = 2Мбит/сек Скорость должна быть одинакова на приёмнике и передатчике. При самой низкой скорости имеем самую высокую чувствительность и дальность. */
  radio.setDataRate(RF24_1MBPS); 
  
  /* Указываем мощность передатчика RF24_PA_MIN=-18dBm RF24_PA_LOW=-12dBm    RF24_PA_HIGH=-6dBm    RF24_PA_MAX=0dBm  */
  radio.setPALevel(RF24_PA_MAX);
 
  /* Открываем трубу с адресом 0x0123456789LL для передачи данных (передатчик может одновременно вещать только по одной трубе). */
  radio.openWritingPipe(0x0123456789LL);

  Serial.begin(115200);
}
 
void loop() {

  //Serial.println(dataToBeTransmitted);
  if (dataToBeTransmitted > 0) {
        radio.write(&dataToBeTransmitted, sizeof(dataToBeTransmitted));
      }

}

void serialEvent() {

  int c; //результат = число из строки

  //Serial.println(current_step);

  while (Serial.available() > 0) {

    int inChar = Serial.read();

    c = 0;
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {

      //c = inString.toInt();

      dataToBeTransmitted = inString;
      
      /*
      // clear the string for new input:
      inString = "";

      if (c >= 0) {
        dataToBeTransmitted = (long)c;
      }
      if (c < 0) {
        dataToBeTransmitted = (long)0;
      }*/
    }
  }
}
