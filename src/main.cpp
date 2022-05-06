#include <Arduino.h>
#include "MIDIUSB.h"
#include "Bounce2.h"
#include "USBCore.h"

#define PEDAL1 8
#define PEDAL2 9
#define PEDAL3 10
#define PEDAL4 11
#define LED_PIN LED_BUILTIN

// Instantiate a Button object
Button pedal1 = Button();

// Instantiate another Button object
Button pedal2 = Button();

// Instantiate a Button object
Button pedal3 = Button();

// Instantiate another Button object
Button pedal4 = Button();

// true means using PTT, false means unmuted
bool mute_state = true;

void noteOn(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}

void noteOff(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}

void controlChange(byte channel, byte control, byte value)
{
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

void setup()
{
  // put your setup code here, to run once:
  //Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // SETUP BUTTON A
  pedal1.attach(PEDAL1, INPUT_PULLUP);
  pedal1.interval(50);          // interval in ms
  pedal1.setPressedState(LOW); // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON

  // SETUP BUTTON B
  pedal2.attach({PEDAL2}, INPUT_PULLUP);
  pedal2.interval(50);          // interval in ms
  pedal2.setPressedState(LOW); // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON

  // SETUP BUTTON C
  pedal3.attach(PEDAL3, INPUT_PULLUP);
  pedal3.interval(20);          // interval in ms
  pedal3.setPressedState(LOW); // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON

  // SETUP BUTTON D
  pedal4.attach({PEDAL4}, INPUT_PULLUP);
  pedal4.interval(20);          // interval in ms
  pedal4.setPressedState(LOW); // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
}

void loop()
{
  // put your main code here, to run repeatedly:
  // UPDATE THE BUTTONS
  pedal1.update();
  pedal2.update();
  pedal3.update();
  pedal4.update();

  //Serial.println(pedal3.read());

  if (!mute_state)
  {
    if (pedal1.pressed() || pedal3.pressed())
    {
      //Serial.println("Sending note on");
      digitalWrite(LED_PIN, HIGH);
      noteOn(0, 48, 64); // Channel 0, middle C, normal velocity
    }
    else if ((pedal1.released() || pedal3.released()) && (pedal1.read() == HIGH && pedal3.read() == HIGH))
    {
      //Serial.println("Sending note off");
      digitalWrite(LED_PIN, LOW);
      noteOff(0, 48, 64); // Channel 0, middle C, normal velocity
    }
  }
  else if ((pedal1.released() || pedal3.released()) && (pedal1.read() == HIGH && pedal3.read() == HIGH))
  {
    mute_state = false;
    //Serial.println("Sending note off");
    digitalWrite(LED_PIN, LOW);
    noteOff(0, 48, 64); // Channel 0, middle C, normal velocity
  }
  if (pedal2.pressed() || pedal4.pressed())
  {
    mute_state = !mute_state;
    if (mute_state)
    {
      //Serial.println("Sending note on");
      digitalWrite(LED_PIN, HIGH);
      noteOn(0, 48, 64); // Channel 0, middle C, normal velocity
    }
    else
    {
      //Serial.println("Sending note off");
      digitalWrite(LED_PIN, LOW);
      noteOff(0, 48, 64); // Channel 0, middle C, normal velocity
    }
  }
}