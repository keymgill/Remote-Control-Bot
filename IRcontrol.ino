/*

  Kedar McGill  2/17
*/

#include <Servo.h>                          // Include servo library

Servo servoLeft;                            // Declare left and right servos
Servo servoRight;

const int ServoRightPin = 10;                // I/O pin constants
const int ServoLeftPin = 11;
const int BeeperPin = 4;
const int IrDetPin = 9;

const byte Enter = 11;                       // Non-alpha remote button constants
const byte ChUp = 16;
const byte ChDn = 17;
const byte VolUp = 18;
const byte VolDn = 19;
const byte Power = 21;

const int StopCode = 5;

int bin_1 = 1000;                            //Binary 1 threshold (Microseconds)
int bin_0 = 400;                             //Binary 0 threshold (Microseconds)
int irPulse;                                 // Stores pulses from IR detector
int debug = 1;
int defaultDelay = 20;                       // servo delay
int CurrentRemoteCode = StopCode;

void setup()                                 // Setup runs once then loop takes over
{
  Serial.begin(9600);
  servoLeft.attach(ServoLeftPin);
  servoRight.attach(ServoRightPin);
  pinMode(IrDetPin, INPUT);                 // IR detector
  pinMode(BeeperPin, OUTPUT);               // speaker pin
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
}

void loop()                                  // Auto-repeats after setup
{
  CurrentRemoteCode = GetIrCode();
  if (debug == 1) {
    Serial.print("key pressed: ");
    Serial.println(CurrentRemoteCode);
  }
  
  Obey(CurrentRemoteCode);

  //arm manuever 
  //maneuver(200,
}

void Obey(int command) {

  switch (command)
  {
    case 1:                                  // 1 -> pivot left 
      maneuver(400, 0, defaultDelay);
      break;

    case 2:                                 // 2 -> forward
    case ChUp:                              // or Channel up button
      maneuver(-400, -400, defaultDelay);
      break;

    case 3:                                // 3 -> pivot right
      maneuver(0, 400, defaultDelay);
      break;

    case 4:                                // 4 -> rotate left
    case VolDn:                            // Or volume down button pressed
      maneuver(400, -400, defaultDelay);
      break;

    case StopCode:                         // StopCode -> stay still
      maneuver(0, 0, defaultDelay);
      break;

    case 6:                                // 6 -> rotate right
    case VolUp:                            // or volume up button pressed
      maneuver(-400, 400, defaultDelay);
      break;

    case 7:                                // 7 -> pivot backward-left
      maneuver(-400, 0, defaultDelay);
      break;

    case 8:                                // 8 -> backwards
    case ChDn:                             // or channel down button pressed
      maneuver(400, 400, defaultDelay);
      break;

    case 9:                                // 9 -> pivot backward-right
      maneuver(0, -400, defaultDelay);
      break;

    case Enter:                            // enter -> beep
      tone(BeeperPin, 264, 100);
      break;

    case Power:                            // power -> drop dead
      maneuver(0, 0, -1);
      break;

    default:                               // if no button pressed -> stay still
      maneuver(0, 0, defaultDelay);
  }
}

/*
   Decode SONY TV remote messages coming from the IR detector.
   Returns: Code for button pressed on remote.
   Source: http://www.parallax.com/tabid/768/ProductID/149/Default.aspx
   Modified for Arduino.
*/
int GetIrCode() {
  unsigned long irPulse = -1;                // Local variables
  int irCode = 0;
  unsigned long pLen = 0;
  int data[12];

  while ((pLen = pulseIn(IrDetPin, LOW, 40000ul)) < 2200) {
    if (pLen == 0) {
      return StopCode ;
    }
  }
  data[0] = pulseIn(IrDetPin, LOW);      //Start measuring bits, I only want low pulses
  data[1] = pulseIn(IrDetPin, LOW);
  data[2] = pulseIn(IrDetPin, LOW);
  data[3] = pulseIn(IrDetPin, LOW);
  data[4] = pulseIn(IrDetPin, LOW);
  data[5] = pulseIn(IrDetPin, LOW);
  data[6] = pulseIn(IrDetPin, LOW);
  data[7] = pulseIn(IrDetPin, LOW);
  data[8] = pulseIn(IrDetPin, LOW);
  data[9] = pulseIn(IrDetPin, LOW);
  data[10] = pulseIn(IrDetPin, LOW);
  data[11] = pulseIn(IrDetPin, LOW);


  for (int i = 0; i < 7; i++) {       // Parse them
    if (data[i] > bin_1) {              // is it a 1?
      bitSet(irCode, i);                // set the corresponding bit in irCode to 1
    }
    else if (data[i] < bin_0) {           // is it too small to be a zero?
      return -1;                       // return -1 to indicate a error
    }
  }

  // Convert alphanumeric codes to their corresponding button values.
  if (irCode <= 9) irCode++;
  if (irCode == 10) irCode = 0;

  return irCode;
}


/*
   Control servo direction, speed and maneuver duration.
   Parameters: speedLeft - left servo speed
               speedRight - right servo speed
               Backward  Linear  Stop  Linear   Forward
               -600      -100......0......100       600
               msTime - time to block code execution before another maneuver
   Source:     http://learn.parallax.com/ManeuverFunction
*/
void maneuver(int speedLeft, int speedRight, int msTime)
{
  servoLeft.writeMicroseconds(1500 + speedLeft);   // Set Left servo speed
  servoRight.writeMicroseconds(1500 - speedRight); // Set right servo speed
  if (msTime == -1)                                // if msTime = -1
  {
    servoLeft.detach();                            // Stop servo signals
    servoRight.detach();
  }
  delay(msTime);
}









