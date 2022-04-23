#include <CountDown.h>
#include <Password.h>
#include <Keypad.h>

//CountDown
CountDown CD;

//KeyPad
const byte ROWS = 4; // Four rows
const byte COLS = 4; //  columns
// Define the Keymap
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
int gameType;
bool messageUpdate = false;

//Message update
const unsigned long eventInterval = 1000;
unsigned long previousTime = 0;

void setup(){
  Serial.begin(9600);
  keypad.addEventListener(keypadEvent); //add an event listener for this keypad
  
  CD.setResolution(CountDown::SECONDS);
}

void loop(){
  keypad.getKey();

  if(CD.remaining() != 0){
    if(messageUpdate){
      unsigned long currentTime = millis();
      if (currentTime - previousTime >= eventInterval) {
        Serial.println(CD.remaining());
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
          CD.stop();
          break;
        case 2:
          Serial.println("PASSWORD CLEAR");
          password.reset();
          break;  
      }   
      break;
    case 'A':
      Serial.println("COUNTDOWN");
      CD.start(60);
      messageUpdate = true;
      gameType = 1;
      break;
    case 'B':
      Serial.println("SUITCASE BOMB");
      gameType = 2;
      messageUpdate = true;
      CD.start(60);
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
    Serial.println("SUITCASE DEACTIVATED");
    CD.stop();
    messageUpdate = false;
  }else{
    Serial.println("PASSWORD WRONGW");
    password.reset();
  }
}
