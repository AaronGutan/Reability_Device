//#include <AccelStepper.h>
#include <nRF24L01.h>
#include <RF24.h>

//RADIO
RF24 radio(48, 53);
long receivedData = 0;
uint8_t pipe;
uint8_t i;

//STEPPER
//AccelStepper stepper(1, 12, 13);
int MAXSPEED    = 15000;                   //максимальное кол-во STEP)
int GEAR_COUNT  = 9;                      //делитель   (макс. кол-во "передач")
int GEAR;                                 //кол-во STEP в одной "передаче"

//DIRECT CONNECT STEPPER
int stepPin8 = 12; //
int dirPin9 = 13;
int MAX_PULSE_DURATION   = 450;
int INTERVAL_BETWEEN_PULSE_DURATION   = 5;


//пины концевиков
const int EndMech_FINISH_Pin29 = 29;       //end  position
const int EndMech_START_Pin27 = 27;        //start position

//состояние концевиков
boolean start;
boolean finish;

//формат обмена = 3 цифры
int Digit_1 = 0;                            //направление
int Digit_2 = 0;                            //текущая "передача"
int Digit_3 = 1;                            //зарезервировано

boolean dir = 0;                               //направление 0 - стоп, 1 - идти в end position, 2 - идти в start position
int triga = 0;

String inString   = "";    //incoming str from COM
String inRFString = "";    //incoming str from RF

void setup()
{  

  pinMode(stepPin8, OUTPUT);
  pinMode(dirPin9, OUTPUT);
  
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  //режим пинов концевиков
  pinMode(EndMech_FINISH_Pin29, INPUT);
  pinMode(EndMech_START_Pin27, INPUT);

  //считываем сотояние концевиков
  start  = digitalRead(EndMech_START_Pin27);
  finish = digitalRead(EndMech_FINISH_Pin29);

  radio.begin();
  radio.setChannel(0x30);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(1, 0x0123456789LL);
  radio.startListening();
  
  //stepper.setMaxSpeed(MAXSPEED);
   	
}

void loop()
{  

//if (radio.available(&pipe)) {
    
//    radio.read(&receivedData, sizeof(receivedData));

//    inRFString = String(receivedData);

    ////Serial.println(inRFString);
    
//    if (receivedData > 0) {

//        Digit_1 = (int)map(inRFString.charAt(0), 48, 57, 1, 10) - 1;
//        Digit_2 = (int)map(inRFString.charAt(1), 48, 57, 1, 10) - 1;
//        Digit_3 = (int)map(inRFString.charAt(2), 48, 57, 1, 10) - 1;    

//      }
//      else {

//        Digit_1 = 0;
//        Digit_2 = 0;
//        Digit_3 = 0;

//      }
  
//  }
  
         

  if (Digit_1 == 0) {
    //stepper.stop();
    //led_stop();
    return;
  }
  if (Digit_2 == 0) {
    //stepper.stop();
    //led_stop();
    return;
  }
  if (Digit_3 == 0) {
    //stepper.stop();
    //led_stop();
    return;
  }
  
  

  GEAR = get_gear(MAX_PULSE_DURATION, GEAR_COUNT, Digit_2) ;
  
  //Serial.println(finish);

  
  
  dir = get_dir();

  //if (get_permission()) {
    if (1==1) {

    //при смене направления остановится
    if (dir != 0) {
      if (triga != dir) {
        //stepper.stop();
        //led_stop();
      }
    }

    RotateCycle (10, GEAR, INTERVAL_BETWEEN_PULSE_DURATION, dir);
    //led_work();
  }
  else {
    //stepper.stop();
    //led_stop();
  }
  
  triga = dir;
  
}

void serialEvent() {

  while (Serial.available() > 0) {

    byte inChar = Serial.read();

    //c = 0;

    if (isAlphaNumeric(inChar)) {
      // конвертировать входящий байт в символ и склеить:
      inString += (char)inChar;
    }
    // если приняли символ Enter, обрабатываем:
    if (inChar == '\n') {

      if (inString.length() == 3) {

        Digit_1 = map(inString.charAt(0), 48, 57, 1, 10) - 1;
        Digit_2 = map(inString.charAt(1), 48, 57, 1, 10) - 1;
        Digit_3 = map(inString.charAt(2), 48, 57, 1, 10) - 1;

      }
      else if (inString.length() < 3) {

        Digit_1 = 0;
        Digit_2 = 0;
        Digit_3 = 0;

      }
      else {
        if (inString.startsWith("max")) {
          MAXSPEED = inString.substring(3).toInt();
        }
        //if (inString.startsWith("gear")) {
        //  GEAR_COUNT = inString.substring(4).toInt();
        //}
      }
      // очистим для нового ввода:
      inString = "";
    }
  }
}

int get_gear(int MAX_SPEED, int G_COUNT, int CURRENT_GEAR) {

  if (G_COUNT == 0) {
    return 0;
  }
  if (CURRENT_GEAR == 0) {
    return 0;
  }

  int ret = MAX_SPEED - (MAX_SPEED / G_COUNT * CURRENT_GEAR);

  if ( ret == 0 ) {
    return 5;
  }
  else {
    return ret;
  }
}

int get_dir() {

  if(Digit_1 == 1) {
    return  LOW;
  }
  if(Digit_1 == 2) {
    return  HIGH;
  }
  return 0;
}

boolean get_permission() {
  //считываем состояние концевиков
  start = digitalRead(EndMech_START_Pin27);
  finish = digitalRead(EndMech_FINISH_Pin29);

  if (start == LOW && dir == HIGH)
  {
    //stepper.stop();
    //led_stop();
    return false;
  }
  if (finish == LOW && dir == LOW)
  {
    //stepper.stop();
    //led_stop();
    return false;
  }
  return true;
}

void RotateCycle (long count, int duration1, int duration2, bool dir)
{

  digitalWrite (dirPin9, dir);

  for (long a = 1; a < count; a++) {

    digitalWrite(stepPin8, HIGH);
    delayMicroseconds (duration1);
    digitalWrite(stepPin8, LOW);
    delayMicroseconds (duration2);

    
  }
  
}

void LED_BUILTIN_FLASH() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);
}
