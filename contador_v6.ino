#include <Password.h>
#include <Keypad.h>

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
Password password = Password( "1234" );

void setup(){
  Serial.begin(9600);
  keypad.addEventListener(keypadEvent); //add an event listener for this keypad
}

void loop(){
  keypad.getKey();
}

void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
    case PRESSED:
    
  switch (eKey){
    case '*': checkPassword(); break;
    case '#': password.reset(); break;
    case 'A': Serial.println("COUNTDOWN"); break;
    case 'B': Serial.println("BOMB CASE"); break;
    default: 
      password.append(eKey);
      Serial.print("Pressed: ");
      Serial.println(eKey);
     }
  }
}

void checkPassword(){
  if (password.evaluate()){
    Serial.println("Success");
    //Add code to run if it works
  }else{
    Serial.println("Wrong");
    password.reset();
  }
}
