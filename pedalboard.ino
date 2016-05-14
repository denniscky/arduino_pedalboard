#include <Bounce.h>
#define OCTLEN 12
#define HI_OCTLEN 8
#define CHANNEL 3
#define C_1 36
#define C_2 48
#define C_3 60
#define LED 6
#define SWELLPIN 44
#define SWELLCONTROL 80
#define ANALOGTHRES 8
#define ANALOGSTEP 8

// Create Bounce objects for each button.  The Bounce object
// automatically deals with contact chatter or "bounce", and
// it makes detecting changes very simple.
// eg. Bounce C1  = Bounce(0, 5);  
// 5 = 5 ms debounce time which is appropriate for good
// quality mechanical pushbuttons if a button is too "sensitive"
// to rapid touch, you can increase this time.

// Break into 3 arrays of one octave each for easy reading
int pinsOctave1[OCTLEN]    = {27, 0, 1, 2, 3, 4, 5,    7, 9,10,11,12};
int pinsOctave2[OCTLEN]    = {13,14,15,16,17,18,26,   25,24,23,22,21};
int pinsOctave3[HI_OCTLEN] = {20,19,43,38,39,40,41,   42};

// First octave
Bounce bounceOctave1[OCTLEN] = {
  Bounce(pinsOctave1[0],5), 
  Bounce(pinsOctave1[1],5),
  Bounce(pinsOctave1[2],5), 
  Bounce(pinsOctave1[3],5),
  Bounce(pinsOctave1[4],5), 
  Bounce(pinsOctave1[5],5),
  Bounce(pinsOctave1[6],5), 
  Bounce(pinsOctave1[7],5),
  Bounce(pinsOctave1[8],5),
  Bounce(pinsOctave1[9],5), 
  Bounce(pinsOctave1[10],5),
  Bounce(pinsOctave1[11],5),
};

// Second octave
Bounce bounceOctave2[OCTLEN] = {
  Bounce(pinsOctave2[0],5), 
  Bounce(pinsOctave2[1],5),
  Bounce(pinsOctave2[2],5), 
  Bounce(pinsOctave2[3],5),
  Bounce(pinsOctave2[4],5), 
  Bounce(pinsOctave2[5],5),
  Bounce(pinsOctave2[6],5), 
  Bounce(pinsOctave2[7],5),
  Bounce(pinsOctave2[8],5),
  Bounce(pinsOctave2[9],5), 
  Bounce(pinsOctave2[10],5),
  Bounce(pinsOctave2[11],5),
};

// Third octave
Bounce bounceOctave3[HI_OCTLEN] = {
  Bounce(pinsOctave3[0],5), 
  Bounce(pinsOctave3[1],5),
  Bounce(pinsOctave3[2],5), 
  Bounce(pinsOctave3[3],5),
  Bounce(pinsOctave3[4],5), 
  Bounce(pinsOctave3[5],5),
  Bounce(pinsOctave3[6],5), 
  Bounce(pinsOctave3[7],5),
};

// Swell shoe variables 
int val;
int previousVal;
int increase;

void setup() {
  pinMode(LED, OUTPUT);
  
  for (int i=0; i<OCTLEN; i++) {
    pinMode(pinsOctave1[i], INPUT_PULLUP);
  }
  for (int i=0; i<OCTLEN; i++) {
    pinMode(pinsOctave2[i], INPUT_PULLUP);
  }
  for (int i=0; i<HI_OCTLEN; i++) {
    pinMode(pinsOctave3[i], INPUT_PULLUP);
  }
  
  // Swell shoe sets to 0 at start (it will adjust itself on first frame)
  val = 0;
  previousVal = 0;
  increase = 0;
}

// For LED stuff, no actual functionality
boolean isPlayed = false;

void loop() {
  // Update all the buttons.  There should not be any long
  // delays in loop(), so this runs repetitively at a rate
  // faster than the buttons could be pressed and released.
  for (int i=0; i<OCTLEN; i++) {
    bounceOctave1[i].update();
  }
  for (int i=0; i<OCTLEN; i++) {
    bounceOctave2[i].update();
  }
  for (int i=0; i<HI_OCTLEN; i++) {
    bounceOctave3[i].update();
  }

  // Check each button for "falling" edge.
  // Send a MIDI Note On message when each button presses
  // Update the Joystick buttons only upon changes.
  // falling = high (not pressed - voltage from pullup resistor)
  //           to low (pressed - button connects pin to ground)
  
  // Check each button for "rising" edge
  // Send a MIDI Note Off message when each button releases
  // For many types of projects, you only care when the button
  // is pressed and the release isn't needed.
  // rising = low (pressed - button connects pin to ground)
  //          to high (not pressed - voltage from pullup resistor)
  for (int i=0; i<OCTLEN; i++) {
    if (bounceOctave1[i].fallingEdge()) {
      usbMIDI.sendNoteOn(C_1 + i, 99, CHANNEL);
      isPlayed = true;
    }
    else if (bounceOctave1[i].risingEdge()) {
      usbMIDI.sendNoteOff(C_1 + i, 0, CHANNEL);
      isPlayed = false;
    }
  }
  for (int i=0; i<OCTLEN; i++) {
    if (bounceOctave2[i].fallingEdge()) {
      usbMIDI.sendNoteOn(C_2 + i, 99, CHANNEL);
      isPlayed = true;
    }
    else if (bounceOctave2[i].risingEdge()) {
      usbMIDI.sendNoteOff(C_2 + i, 0, CHANNEL);
      isPlayed = false;
    }
  }
  for (int i=0; i<HI_OCTLEN; i++) {
    if (bounceOctave3[i].fallingEdge()) {
      usbMIDI.sendNoteOn(C_3 + i, 99, CHANNEL);
      isPlayed = true;
    }
    else if (bounceOctave3[i].risingEdge()) {
      usbMIDI.sendNoteOff(C_3 + i, 0, CHANNEL);
      isPlayed = false;
    }
  }

  if (isPlayed) {
    digitalWrite(LED, HIGH);
  }
  else {
    digitalWrite(LED, LOW);
  }
  
  // Swell shoe section
  val = analogRead(SWELLPIN);
  increase = val-previousVal;
  if (abs(increase) >= ANALOGTHRES) {
    usbMIDI.sendControlChange(SWELLCONTROL, val/ANALOGSTEP, CHANNEL);
    previousVal = val;
  }
}

