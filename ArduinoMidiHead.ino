#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>

struct MySettings : public midi::DefaultSettings {
  static const long BaudRate = 128000;
};

enum NoteStatus : bool {
  NOT_PLAYING = 0,
  PLAYING
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MySettings)

// constants won't change. They're used here to set pin numbers:
const int buttonPin[22] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43}; // the number of the pushbutton pin
const int ledPinPot =  13;      // the number of the LED pin
const int ledPinBut = 12;

// variables will change:
int buttonState[22] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // current state of the button
int lastButtonState[22] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // previous state of the button
unsigned long cd[22] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //Cooldown

//Poteniometer Pins
const int pot[4] = {A0, A1, A2, A3};
int potVal[4] = {0, 0, 0, 0};
int lastPotVal[4] = {0, 0, 0, 0};


void setup() 
{
  // initialize the LED pin as an output:
  pinMode(ledPinBut, OUTPUT);
  pinMode(ledPinPot, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(ledPinPot, LOW);
  digitalWrite(ledPinBut, LOW);
  //LaunchMidi and listen
  MIDI.begin(5);
}

void loop()
{
  digitalWrite(ledPinPot, LOW);
  digitalWrite(ledPinBut, LOW);
  ButtonMidiDetect();
  PotentiometerCCDetect();
}


void PotentiometerCCDetect()
{
  int diff = 4; // difference amount

  for (int x = 0; x < 4; x++)
  {
    //Potentiometer 1
    potVal[x] = analogRead(pot[x]);
    int potValDiff = potVal[x] - lastPotVal[x];

    if (abs(potValDiff) > diff) // execute only if new and old values differ enough
    {
      MIDI.sendControlChange(1, (potVal[x] / 8), (x+1)); //+1 cus midi range is 1 - 127
      digitalWrite(ledPinPot, HIGH);
      lastPotVal[x] = potVal[x]; // reset old value with new reading
    }
  }
}

void ButtonMidiDetect()
{
  unsigned long currTime = millis();

  for (int x = 0; x < 22; x++)
  {
    // read the state of the pushbutton value:
    buttonState[x] = digitalRead(buttonPin[x]);

    if (buttonState[x] != lastButtonState[x]) //If there is a change in button state
    {
      if (buttonState[x] == HIGH) //If the button is pressed
      {
        if (currTime - cd[x] >= 30) //Has enough time passed so we can press the button again?
        {
          cd[x] = currTime;
          MIDI.sendNoteOff((67 + x), 0, 1);

          digitalWrite(ledPinBut, HIGH);
          lastButtonState[x] = buttonState[x];
        }
        else
        {
          
        }

      }
      //else if(buttonState[x] != lastButtonState[x])
      else if (buttonState[x] == LOW)
      {
        MIDI.sendNoteOn((67 + x), 127, 1);
        lastButtonState[x] = buttonState[x];
      }
      //Error Handling
      else
      {

      }

    }
  }
}
