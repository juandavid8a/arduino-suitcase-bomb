#include <CountDown.h>
#include <Password.h>
#include <Keypad.h>

//CountDown
CountDown CD[2];

//KeyPad
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = { 9,8,7,6 };
byte colPins[COLS] = { 5,4,3,2, };

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//Password
Password password = Password( "1234" );

//Generals
int soundSmallPin = 11;
int soundLargePin = 12;
int gameType;
bool messageUpdate = false;
bool preGame = false;
bool posGame = false;
bool bombGame = false;

//Message update
const unsigned long eventInterval = 1000;
unsigned long previousTime = 0;

void setup(){
  Serial.begin(9600);
  pinMode(soundSmallPin, OUTPUT); 
  pinMode(soundLargePin, OUTPUT);
  keypad.addEventListener(keypadEvent);
  
  CD[0].setResolution(CountDown::SECONDS);
  CD[1].setResolution(CountDown::SECONDS);
}

void loop(){
  keypad.getKey();

  if(CD[1].remaining() == 0){
    if(preGame){ preGame = false; countdownGame(); }
    if(posGame){ posGame = false; soundLarge(); }
  }else{
    if(messageUpdate){
      unsigned long currentTime = millis();
      if (currentTime - previousTime >= eventInterval) {
        Serial.println(CD[1].remaining());
        if(preGame || bombGame){ soundSmall(); }
        if(posGame){ soundSmallDouble(); }
        if(CD[1].remaining() == 6){ posGame = true; } 
        previousTime = currentTime;
      }
    }
  }
}

void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
    case PRESSED:

  switch (eKey){
    case '*': checkPassword(); break;
    case '#':
      switch (gameType) {
        case 1:
          Serial.println("COUNTDOWN STOP");
          messageUpdate = false;
          preGame = false;
          posGame = false;
          CD[1].stop();
          break;
        case 2:
          Serial.println("PASSWORD CLEAR");
          password.reset();
          break;  
      }   
      break;
    case 'C':
      Serial.println("COUNTDOWN");
      countdownPreGame();
      gameType = 1;
      break;
    case 'B':
      Serial.println("SUITCASE BOMB");
      gameType = 2;
      preGame = false;
      posGame = false;
      bombGame = true;
      messageUpdate = true;
      CD[1].start(60);
      break;
    default:
      switch (gameType) {
        case 2:
          password.append(eKey);
          Serial.println("Tecla: " + String(eKey));
          break;  
      } 
    }
  }
}

void checkPassword(){
  if (password.evaluate()){
    Serial.println("SUITCASE BOMB DEACTIVATED");
    CD[1].stop();
    messageUpdate = false;
    bombGame = false;
    preGame = false;
    posGame = false;
  }else{
    Serial.println("PASSWORD WRONG");
    password.reset();
  }
}

void countdownPreGame(){
  preGame = true;
  posGame = false;
  bombGame = false;
  messageUpdate = true;
  CD[1].start(5);
}

void countdownGame(){
  messageUpdate = true;
  soundLarge();
  CD[1].start(60);
}

void soundSmall(){
  digitalWrite(12,HIGH);
  delay(10);
  digitalWrite(12,LOW); 
}

void soundSmallDouble(){
  digitalWrite(12,HIGH);
  delay(10);
  digitalWrite(12,LOW);
  delay(80);
  digitalWrite(12,HIGH);
  delay(10);
  digitalWrite(12,LOW);
  delay(80);
  digitalWrite(12,HIGH);
  delay(10);
  digitalWrite(12,LOW); 
}

void soundLarge(){
  digitalWrite(12,HIGH);
  delay(1000);
  digitalWrite(12,LOW); 
}
