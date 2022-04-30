#include <HID-Project.h>                    //include HID_Project library
#include <HID-Settings.h>
#include <Math.h>

#define button1 7
#define button2 18
#define button3 20
#define button4 6
#define button5 15
#define button6 19

#define holdTime 500

int buttonIds[6] = {
  7, 18, 20, 6, 15, 19
};

KeyboardKeycode buttonOutputs[6] = {
  KEY_DELETE,
  KEY_UP_ARROW,
  KEY_DELETE,
  KEY_LEFT_ARROW,
  KEY_DOWN_ARROW,
  KEY_RIGHT_ARROW
};

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  // set pin modes
  pinMode(A3, INPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(button5, INPUT_PULLUP);
  pinMode(button6, INPUT_PULLUP);

  Consumer.begin();                         //initialize computer connection
  delay(1000);                              //wait for computer to connect
  for(int a = 0; a < 52; a++) {
    Consumer.write(MEDIA_VOLUME_DOWN);      //set the volume to 0
    delay(2);
  }
}



int rawValue;
int oldValue;
byte potPercentage;
byte oldPercentage;

byte actualVolume = 0;


bool buttonsPressed[6] = {false, false, false, false, false, false};
bool buttonsInitial[6] = {false, false, false, false, false, false};
double buttonsStartMillis[6] = {millis(), millis(), millis(), millis(), millis(), millis()};

void loop() {
  // put your main code here, to run repeatedly:
  // read input twice
  rawValue = analogRead(A3);
  rawValue = analogRead(A3); // double read
  // ignore bad hop-on region of a pot by removing 8 values at both extremes
  rawValue = constrain(rawValue, 8, 1015);
  // add some deadband
  if (rawValue < (oldValue - 4) || rawValue > (oldValue + 4)) {
    oldValue = rawValue;
    // convert to percentage
    potPercentage = map(oldValue, 8, 1008, 51, 0);
    // Only print if %value changes
    if (oldPercentage != potPercentage) {
      Serial.print("Pot percentage is: ");
      Serial.print(potPercentage);
      Serial.println(" %");
      oldPercentage = potPercentage;
      
      if (actualVolume < potPercentage) {
        while (actualVolume < potPercentage){
          Consumer.write(MEDIA_VOLUME_UP);
          actualVolume++; 
        }
      }
      else if (actualVolume > potPercentage) {
        while (actualVolume > potPercentage) {
          Consumer.write(MEDIA_VOLUME_DOWN);
          actualVolume--;
        }
      }
    }
  }



  // if button pressed input once and start counter,
  // if counter started then if time reached then start rapid input
  // if released then reset

  for (int i=0; i<6; i++) {
    if (!digitalRead(buttonIds[i])) {
      buttonsPressed[i] = true;
    }
    else {
      
      buttonsPressed[i] = false;
    }
    if (buttonsPressed[i]) {
      // check if initial input has been made
      if (!buttonsInitial[i]) {
        buttonsInitial[i] = true;
        buttonsStartMillis[i] = millis();
        Keyboard.write(buttonOutputs[i]);
      }
      // check counter
      else if ((millis() - buttonsStartMillis[i]) > holdTime) {
        Keyboard.write(buttonOutputs[i]);
        delay(30);
      }
    }
    else {
      buttonsInitial[i] = false;
    }
  }
}
