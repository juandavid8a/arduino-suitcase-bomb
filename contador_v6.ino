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
bool countGame = false;
int varSeconds;

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
    if(preGame){ preGame = false; countdownGame(); }else{ countGame = false; }
    if(posGame){ posGame = false; soundLarge(); }
    if(bombGame){ bombGame = false; varSeconds = 0;}
  }else{
    if(messageUpdate){
      unsigned long currentTime = millis();
      if (currentTime - previousTime >= eventInterval) {
        Serial.println(CD[1].remaining());
        if(preGame || bombGame){ soundSmall(1); }
        if(posGame){ soundSmallDouble(2); }
        if(CD[1].remaining() == 6){ posGame = true; } 
        previousTime = currentTime;
      }
    }
  }
}

void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
    case PRESSED:
      soundSmall(1);
  switch (eKey){
    case '*':
      if(bombGame){
        checkPassword();
      }else{
        if(countGame){
          Serial.println("ERROR: COUNTDOWN ACTIVATED");
          soundSmallDouble(2);
        }else{
          if(varSeconds > 0){
            Serial.println("Minutos: " + String(varSeconds / 60));
            gameType = 1; 
            countdownPreGame();
          }else{
            Serial.println("ERROR: NO MINUTES");
            soundSmallDouble(2);
          } 
        } 
      }
      break;
    case '#':
      switch (gameType) {
        case 1:
          if(countGame){
            Serial.println("COUNTDOWN STOP");
            soundSmallDouble(10);
            messageUpdate = false;
            countGame = false;
            preGame = false;
            posGame = false;
            CD[1].stop();
          }
          break;
        case 2:
          Serial.println("PASSWORD CLEAR");
          password.reset();
          soundSmallDouble(2);
          break;  
      }   
      break;
    case 'B':
      bombGameStart();
      break;
    case 'C':
      countGameStart();
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

void countGameStart(){
  if(bombGame){
    Serial.println("ERROR: SUITCASE BOMB ACTIVATED");
    soundSmallDouble(2);
  }else{
    Serial.println("COUNTDOWN");
    soundSmall(1);
    setNumbers();
    countdownPreGame();
    gameType = 1;
  }
}

void bombGameStart(){
  if(bombGame){
    Serial.println("ERROR: SUITCASE BOMB ACTIVATED");
    soundSmallDouble(2);
  }else{
    if(countGame){
      Serial.println("ERROR: COUNTDOWN ACTIVATED");
      soundSmallDouble(2);
    }else{
      Serial.println("SUITCASE BOMB");
      password.reset();
      soundSmall(1);
      setNumbers();
      gameType = 2;
      preGame = false;
      posGame = false;
      bombGame = true;
      messageUpdate = true;
      CD[1].start(varSeconds);        
    }
  }
}

void setNumbers(){
  String number1 = String(keypad.waitForKey());
  String number2 = String(keypad.waitForKey());
  String numbers = number1 + number2;
  Serial.println("Minutos: " + numbers);
  varSeconds = 60 * numbers.toInt();
}

void checkPassword(){
  if (password.evaluate()){
    Serial.println("SUITCASE BOMB DEACTIVATED");
    CD[1].stop();
    password.reset();
    soundSmallDouble(20);
    messageUpdate = false;
    bombGame = false;
    preGame = false;
    posGame = false;
  }else{
    Serial.println("PASSWORD WRONG");
    soundSmallDouble(2);
    password.reset();
  }
}

void countdownPreGame(){
  posGame = false;
  bombGame = false;
  preGame = true;
  countGame = true;
  messageUpdate = true;
  CD[1].start(5);
}

void countdownGame(){
  messageUpdate = true;
  soundLarge();
  CD[1].start(varSeconds);
}

void soundSmall(int data){
  for (data; data >= 1; data--){
    digitalWrite(12,HIGH);
    delay(10);
    digitalWrite(12,LOW);
    delay(200);
  } 
}

void soundSmallDouble(int data){
   for (data; data >= 1; data--){
      digitalWrite(12,HIGH);
      delay(10);
      digitalWrite(12,LOW);
      delay(80);
    }
}

void soundLarge(){
  digitalWrite(12,HIGH);
  delay(1000);
  digitalWrite(12,LOW); 
}
