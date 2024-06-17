
#include <Stepper.h>
#include <Servo.h>

const int buzzerPin = 6; // Pin connected to the buzzer
const int servoPin = 7;

const int dotDuration = 200; // Duration of a dot in milliseconds
const int dashDuration = 600; // Duration of a dash in milliseconds
const int symbolGap = 200; // Gap between symbols in milliseconds
const int letterGap = 600; // Gap between letters in milliseconds
const int wordGap = 1400; // Gap between words in milliseconds



// Define the number of steps per revolution for your 28BYJ-48 motor
const int stepsPerRevolution = 2048;
const int moveSpaceWord = 50;
const int moveDash = 50;
const int movePoint = 20;
const int moveNext = 20;

Stepper myStepper(stepsPerRevolution,8,10,9,11);


const int upPen = 105;
const int downPen =  115;
Servo myServo;


// Morse code dictionary
const char* morseCode[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", 
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", 
  "..-", "...-", ".--", "-..-", "-.--", "--.."
};

// Function to play a dot
void playDot() {
  digitalWrite(buzzerPin, HIGH);
  delay(dotDuration);
  digitalWrite(buzzerPin, LOW);
  delay(symbolGap);
}

// Function to play a dash
void playDash() {
  digitalWrite(buzzerPin, HIGH);
  delay(dashDuration);
  digitalWrite(buzzerPin, LOW);
  delay(symbolGap);
}

void move_dot(){
  myServo.write(downPen);
  delay(500);
  myStepper.step(movePoint);
  delay(500);
  myServo.write(upPen);
  delay(500);
  myStepper.step(movePoint);
  
}

void move_dash(){
  myServo.write(downPen);
  delay(500);
  myStepper.step(moveDash);
  delay(500);
  myServo.write(upPen);
  delay(500);
  myStepper.step(movePoint);
}




// Function to play Morse code for a given character
void playMorse(char c) {
  if (c >= 'A' && c <= 'Z') {
    c = c - 'A';
  } else if (c >= 'a' && c <= 'z') {
    c = c - 'a';
  } else {
    return;
  }

  const char* morse = morseCode[c];
  while (*morse) {
    if (*morse == '.') {
      playDot();
      move_dot();
    } else if (*morse == '-') {
      playDash();
      move_dash();
    }
    morse++;
  }
  delay(letterGap - symbolGap); // Adjust gap after each letter
}

void setup() {

  pinMode(buzzerPin, OUTPUT);
  myStepper.setSpeed(15); // Set to 15 RPM for smooth operation
  myServo.attach(servoPin);

  //start pen up
  myServo.write(upPen);

  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Enter a string to convert to Morse code:");
}

void loop() {
  if (Serial.available()) {
    String inputString = Serial.readStringUntil('\n');
    inputString.trim();

    if(inputString == "START"){
      myStepper.step(150);
    }
    else if (inputString == "F"){
      myStepper.step(stepsPerRevolution);
    }
    else if (inputString == "B"){
      myStepper.step(-stepsPerRevolution);
    }
    else if (inputString == "1"){
      myServo.write(downPen);
    }
    else if (inputString == "2"){
      myServo.write(upPen);
    }
    else if (inputString == "."){
      playDot();
    }
    else if (inputString == "-"){
      playDash();
    }
    else{
      myStepper.step(150);
      for (int i = 0; i < inputString.length(); i++) {
        char c = inputString[i];
        if (c == ' ') {
          delay(wordGap - letterGap); // Adjust gap after each word
        } else {
          playMorse(c);
          myStepper.step(moveSpaceWord);
        }
      }
      Serial.println("Done!");
      myStepper.step(stepsPerRevolution);
    } 
  }
}
