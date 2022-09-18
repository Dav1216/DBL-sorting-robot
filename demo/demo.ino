#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

Servo myservo;  // create servo object to control a servo
// variable to store the servo position
int pos = 0;
const int s0 = 8;
const int s1 = 9;
const int s2 = 10;
const int s3 = 11;
const int out = 12;
// Variables
int red = 0;
int green = 0;
int blue = 0;
// Calibrated value
int calMin = 5;
int calMaxR = 50;
int CalMaxG = 50;
int calMaxB = 50;
//boolean values for establishing whether a disk is green, black or white or is it just the converyor belt detected
int c;
int w;
int b;
int g;
//angle values for the positions of the servo arm
int restPosition = 0;
int nonRestPosition = 120;
//colors of conveyor belt
int beltRed;
int beltBlue;
int beltGreen;
int beltMean;
int diskNumber = 0;
int pattern = 0;
int patternChosen = 0;
String neededColor;
int iteration = -1;

//buttons:
int input1Pin = 3;
int input3Pin = 5;
int input4Pin = 6;
int input5Pin = 7;
// the number of the pushbutton pin
//for the breaking beam sensor
int IRSensor = 4; // connect ir sensor to arduino pin 1
int LED = 13; // conect Led to arduino pin 13

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the address for LCD
// LCD custom characters for displaying the pattern on the screen
byte diskWhite[8] = {
  0b00000,
  0b00000,
  0b01110,
  0b10001,
  0b10001,
  0b10001,
  0b01110,
  0b00000
};

byte diskBlack[8] = {
  0b00000,
  0b00000,
  0b01110,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00000
};

byte diskGreen[8] = {
  0b00000,
  0b00000,
  0b01110,
  0b10101,
  0b11011,
  0b10101,
  0b01110,
  0b00000
};

//the setup of the pins as output or input plus the setting of the operating frequency of the color sensor, in the last two lines of setup to 100%
void setup()
{
  Serial.begin(9600);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(out, INPUT);
  myservo.attach(2);  // attaches the servo on pin 6 to the servo object
  digitalWrite(s0, HIGH);
  digitalWrite(s1, HIGH);
  // buttons:
  pinMode(input1Pin, INPUT);
  pinMode(input3Pin, INPUT);
  pinMode(input4Pin, INPUT);
  pinMode(input5Pin, INPUT);
  //breaking beam
  pinMode (IRSensor, INPUT); // sensor pin INPUT
  pinMode (LED, OUTPUT); // Led pin OUTPUT
  // LCD:
  lcd.init();
  lcd.backlight();
  // creating the pictures of disks
  lcd.createChar(0, diskWhite);
  lcd.createChar(1, diskBlack);
  lcd.createChar(2, diskGreen);

}

//this function fetches the values of the rgb of the belt in order for us to see when it is free without any disks and when disks are there
/*void colorsOfBelt() {
  Serial.println("Clear belt area!!");
  delay(5000);
  for (int i = 0; i <= 4; i++) {
    color();
    beltRed = beltRed + red;
    beltGreen = beltGreen + green;
    beltBlue = beltBlue + blue;
    delay(500);
  }
  beltRed = beltRed / 5;
  beltGreen = beltGreen / 5;
  beltBlue = beltBlue / 5;

  beltRed = map(beltRed, calMin, calMaxR, 255, 0);
  beltGreen = map(beltGreen, calMin, CalMaxG, 255, 0);
  beltBlue = map(beltBlue, calMin, calMaxB, 255, 0);
  beltMean = (beltRed + beltGreen + beltBlue) / 3;
  Serial.println("Belt colors retained");
  Serial.println("beltRed");
  Serial.println(beltRed);
  Serial.println("beltGreen");
  Serial.println(beltGreen);
  Serial.println("beltBlue");
  Serial.println(beltBlue);
  Serial.println("beltMean");
  Serial.println(beltMean);
  delay(5000);
  }
*/

//function to calibrate the color sensor by puting the white disk in front of it, fetches the maximum and minimum values of an rgb corresponding to black and white
void calibrate() {
  Serial.println("Clear the conveyor belt, press c");
  while (Serial.read() != 'c') {
    //do nothing
    ;
  }
  color();
  calMaxR = red;
  CalMaxG = green;
  calMaxB = blue;
  Serial.println("Put white colored disk in front of sensor, then enter c again");
  while (Serial.read() != 'c') {
    //do nothing
    ;
  }
  color();
  calMin = (red + green + blue) / 3;
  Serial.println("calibrated successfully.");
  delay(3000);
}

//function that fetches the raw values that will be used later to calibrate the color sensor
void color() {
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);//outputting the color red
  //count OUT, pRed, RED
  red = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  digitalWrite(s3, HIGH);//outputting the color blue
  //count OUT, pBLUE, BLUE
  blue = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  digitalWrite(s2, HIGH);//outputting the color green
  //count OUT, pGreen, GREEN
  green = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
}

//the main loop which prints the intensity of which RGB color, finds out which disk we have and what disk we need for the patterns and moves the motor with the servo functions built down below
void loop()
{

  servoRestPosition();
  int statusSensor = digitalRead(IRSensor);
  lcdMessage4();
  if (statusSensor == 1) {

    digitalWrite(LED, LOW); // LED LOW
  }

  else
  {
    digitalWrite(LED, HIGH); // LED High
    delay(500);
    color();
    red = map(red, calMin, calMaxR, 255, 0);
    green = map(green, calMin, CalMaxG, 255, 0);
    blue = map(blue, calMin, calMaxB, 255, 0);
    Serial.print("R Intensity: ");
    Serial.print(red);
    Serial.print("G Intensity: ");
    Serial.print(green);
    Serial.print("B Intensity: ");
    Serial.println(blue);
    w = isWhite(red, green, blue);
    b = isBlack(red, green, blue);
    g = isGreen(red, green, blue, w, b);
    delay(1500);


    if (w && diskNumber < 6) {
      if (neededColor == "white") {
        Serial.println("need white, got white");
        delay(9000);
        servoNonRestPosition();
        delay(7500);
        pushDiskOntoStack();
        diskNumber += 1;
        Serial.println(diskNumber);
        w = 0;
        lcd.write((byte)0);


      } else if (neededColor == "black") {
        Serial.println("need black, got white");
      } else if (neededColor == "green") {
        Serial.println("need green, got white");
      }
    } else if (b) {
      if (neededColor == "white") {
        Serial.println("need white, got black");
      } else if (neededColor == "black") {
        Serial.println("need black, got black");
        delay(9000);
        servoNonRestPosition();
        delay(7500);
        pushDiskOntoStack() ;
        diskNumber += 1;
        Serial.println(diskNumber);
        b = 0;
        lcd.write((byte)1);

      } else if (neededColor == "green") {
        Serial.println("need green, got black");
      }
    } else if (g) {
      if (neededColor == "white") {
        Serial.println("need white, got green");
      } else if (neededColor == "black") {
        Serial.println("need black, got green");
      } else if (neededColor == "green") {
        Serial.println("need green, got green");
        delay(9000);
        servoNonRestPosition();
        delay(7500);
        pushDiskOntoStack();
        diskNumber += 1;
        Serial.println(diskNumber);
        g = 0;
        lcd.write((byte)2);

      }
    }

    delay(200);

  }

  if (Serial.read() == 'c') {
    calibrate();
    //  colorsOfBelt();
  }

  // c = isConveyer(red, green, blue);

  lcdMessage1();
  lcdMessage2();


  for (int i = 3 ; i <= 7 ; i++) {
    checkPush(i);
  }
  if (pattern == 1) {
    neededColor = pattern1(diskNumber);
  } else if (pattern == 3) {
    neededColor = pattern3(diskNumber);
  } else if (pattern == 4) {
    neededColor = pattern4(diskNumber);
  }



  //if (c) {
  //  //Serial.print("CBelt");
  //  } else {

}

// function to show on LCD a message about choosing a pattern when no pattern has been chosen yet
void lcdMessage1() {
  if (pattern == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Please choose");
    lcd.setCursor(0, 1);
    lcd.print("a pattern");
  }
}

// function to show on LCD a message about emptying the full stack
void lcdMessage2() {
  if (diskNumber == 6 and iteration == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Stack full,empty");
    lcd.setCursor(0, 1);
    lcd.print("it, press Btn 4");
    iteration = 2;
  }
}

// function displaying a message on LCD
// showing pictograms explaining the chosen pattern
void lcdMessage3() {
  if (iteration == 0) {
    if (diskNumber < 6) {
      if (pattern == 1) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Creating pattern");
        lcd.setCursor(0, 1);
        for (int i = 0; i <= 2; i++) {
          lcd.write((byte)0);
          lcd.write((byte)1);
        }
        iteration = 1;
      } else if (pattern == 2) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Creating pattern");
        lcd.setCursor(0, 1);
        //   for (int i = 0; i <= 2; i++) {
        //     lcd.write((byte)1);
        //     lcd.write((byte)2);
        //   }
        for (int i = 0; i <= 4; i++) {
          lcd.write((byte)1);
        }
        iteration = 1;
        lcd.write((byte)2);
      } else if (pattern == 3) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Creating pattern");
        lcd.setCursor(0, 1);
        //   for (int i = 0; i <= 2; i++) {
        //     lcd.write((byte)0);
        //     lcd.write((byte)2);
        //   }
        for (int i = 0; i <= 4; i++) {
          lcd.write((byte)0);
        }
        iteration = 1;
        lcd.write((byte)2);
      } else if (pattern == 4) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Creating pattern");
        lcd.setCursor(0, 1);
        for (int i = 0; i <= 1; i++) {
          lcd.write((byte)0);
          lcd.write((byte)1);
          lcd.write((byte)2);
        }
        iteration = 1;
      }
    }
  }
}

void lcdMessage4() {
  if (diskNumber < 6 && iteration == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Creating pattern");
    lcd.setCursor(0, 1);
    iteration = 1;
  }
}

//function to select the patterns, based on which button is pushed takes as input the pinNumber which is from 3 to 5 and if the input from it is high we select the corresponding pattern, so the pattern will take the specific values
//the fifth button serves as an input from the user that he has emptied the stack
void checkPush(int pinNumber) {
  int pushed = digitalRead(pinNumber);  // read input value
  if (pushed == HIGH && pinNumber == 3 && patternChosen == 0) {
    pattern = 1;
    patternChosen = 1;
    iteration = 0;
  } else if (pushed == HIGH && pinNumber == 5 && patternChosen == 0) {
    pattern = 3;
    patternChosen = 1;
    iteration = 0;
  } else if (pushed == HIGH && pinNumber == 6 && patternChosen == 0) {
    pattern = 4;
    patternChosen = 1;
    iteration = 0;
  } else if (pushed == HIGH && pinNumber == 7 && diskNumber == 6) {
    diskNumber = 0;
    iteration = 0;
  }
}

//function to verify if disc is white, takes in the red, blue, green values after calibration so real rgb values and outputs false or true based on whether the disk is the correct color or not
int isWhite(int red, int green, int blue)
{
  if ((red >= 178) && (green >= 178) && (blue >= 178)) {
    return 1;
  } else {
    return 0;
  }
}

//function to verify if disc is black, takes in the red, blue, green values after calibration so real rgb values and outputs false or true based on whether the disk is the correct color or not
int isBlack(int red, int green, int blue)
{
  // int blackMean = (red + green + blue) / 3;
  if ((red <= 100) && (green <= 100) && (blue <= 100)) {
    return 1;
  } else {
    return 0;
  }
}

//function to verify if disc is green,takes in the red, blue, green values after calibration so real rgb values and outputs false or true based on whether the disk is the correct color or not
int isGreen(int red, int green, int blue, int w, int b)
{
  if (red < green && blue < green && !w && !b) {
    return 1;
  } else {
    return 0;
  }
}
//function to verify if conveyor belt is free without disks, takes in the red, blue, green values after calibration so real rgb values and outputs false or true based on whether there is any disk in which case it is false and if it is not is true returned
int isConveyer(int red, int green, int blue) {
  if ((beltRed - 30 <= red && red <= beltRed + 30) && (beltGreen - 30 <= green && green <= beltGreen + 30) && (beltBlue - 30 <= blue && blue <= beltBlue + 30)) {
    return 1;
  } else {
    return 0;
  }
}

//function for returnin fast to a rest position, physical movement of the arm
void servoRestPosition() {
  myservo.write(restPosition); // tell servo to go to position in variable 'pos'
}

//function for returnin fast to a non-rest position, physical movement of the arm
void servoNonRestPosition() {
  myservo.write(nonRestPosition); // tell servo to go to position in variable 'pos'
}

//function for pushing disks slowly onto stack, not kicking them super hard, physical movement of the arm
void pushDiskOntoStack() {
  for (pos = nonRestPosition; pos >= restPosition; pos -= 2) { // goes from nonrest to rest whilst pushing disc
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

// pattern for white, black takes as input the disk diskNumber which counts the disks that we succesfully integrated in the pattern in the stack, returns what the next disk to pass shall be "black" or "white"
String pattern1(int diskNumber) {
  if ((diskNumber + 1) % 2 == 0) {
    return "black";
  } else {
    return "white";
  }
}

// pattern for black, green, takes as input the disk diskNumber which counts the disks that we succesfully integrated in the pattern in the stack, returns what the next disk to pass shall be "green" or "black"
/*String pattern2(int diskNumber) {
  if ((diskNumber + 1) % 2 == 0) {
    return "green";
  } else {
    return "black";
  }
  }


  // pattern for white, green,  takes as input the disk diskNumber which counts the disks that we succesfully integrated in the pattern in the stack, returns what the next disk to pass shall be "green" or "white"
  String pattern3(int diskNumber) {
  if ((diskNumber + 1) % 2 == 0) {
    return "green";
  } else {
    return "white";
  }
  }
*/
// pattern for black, green, takes as input the disk diskNumber which counts the disks that we succesfully integrated in the pattern in the stack, returns what the next disk to pass shall be "green" or "black"
// String pattern2(int diskNumber) {
//   if (diskNumber < 6) {
//     return "black";
//   } else {
//     return "green";
//   }
// }
// pattern for white, green,  takes as input the disk diskNumber which counts the disks that we succesfully integrated in the pattern in the stack, returns what the next disk to pass shall be "green" or "white"
String pattern3(int diskNumber) {
  if (diskNumber < 5) {
    return "white";
  } else {
    return "green";
  }
}
// pattern for white, black, green,  takes as input the disk diskNumber which counts the disks that we succesfully integrated in the pattern in the stack, returns what the next disk to pass shall be "green" or "white" or "black"
String pattern4(int diskNumber) {
  if ((diskNumber + 1) % 3 == 0) {
    return "green";
  } else if ((diskNumber + 1) % 3 == 1) {
    return "white";
  } else {
    return "black";
  }
}
