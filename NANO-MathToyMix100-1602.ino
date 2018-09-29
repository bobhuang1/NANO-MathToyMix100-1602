#include <Wire.h>
#include <LiquidCrystal_I2C_Arduino.h>

#define BUTTONPIN  5
#define ALARMPIN 2
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C display(0x27, 16, 2);

int intCurrentStage = 0;
int intFirstOperationType = 1; // 1-plus, 2-minus, 3-multiply
int intSecondOperationType = 1; // 1-plus, 2-minus, 3-multiply

int intFirstNumber = 1;
int intSecondNumber = 1;
int intThirdNumber = 1;

String strQuestion1 = "";
String strQuestion2 = "";

int intResult1 = 0;
int intResult2 = 0;

int intResult1Position = 0;
int intResult2Position = 0;

int intQuestionCounter = 1;

String strPlusSign;
String strMinusSign;
String strMultiplySign;
String strDivideySign;
String strEqualSign;

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 30;    // the debounce time; increase if the output flickers

byte multiply[8] = {
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b00100,
  0b01010,
  0b10001,
  0b00000
};

byte plus[8] = {
  0b00000,
  0b00100,
  0b00100,
  0b11111,
  0b00100,
  0b00100,
  0b00000,
  0b00000
};

byte minus[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte divide[8] = {
  0b00000,
  0b00100,
  0b00000,
  0b11111,
  0b00000,
  0b00100,
  0b00000,
  0b00000
};

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTONPIN, INPUT);
  pinMode(ALARMPIN, OUTPUT);

  display.begin();
  display.backlight();
  display.createChar(0, plus);
  display.createChar(1, minus);
  display.createChar(2, multiply);
  display.createChar(3, divide);
  display.clear();
  display.setCursor(3, 0);
  display.print("Plus-Minus");
  display.setCursor(4, 1);
  display.print("Practice");
  randomSeed(analogRead(0));
  digitalWrite(ALARMPIN, HIGH);
  delay(1000);
  digitalWrite(ALARMPIN, LOW);
  strPlusSign  = String((char)0);
  strMinusSign  = String((char)1);
  strMultiplySign  = String((char)2);
  strDivideySign  = String((char)3);
  strEqualSign  = String((char)61);
}

void generateQuestion1() {
  intFirstOperationType = random(1, 4); // 1-plus, 2-minus, 3-multiply
  if (intFirstOperationType == 3)
  {
    intSecondOperationType = random(1, 3);
    intFirstNumber = random(1, 10);
    intSecondNumber = random(1, 10);
    if (intSecondOperationType == 2)
    {
      intThirdNumber = random(1, intFirstNumber * intSecondNumber);
      strQuestion1 =  String(intFirstNumber) + strMultiplySign + String(intSecondNumber) + strMinusSign + String(intThirdNumber) + strEqualSign + "?";
      intResult1 = intFirstNumber * intSecondNumber - intThirdNumber;
    }
    else
    {
      intThirdNumber = random(1, 100);
      strQuestion1 =  String(intFirstNumber) + strMultiplySign + String(intSecondNumber) + strPlusSign + String(intThirdNumber) + strEqualSign + "?";
      intResult1 = intFirstNumber * intSecondNumber + intThirdNumber;
    }
  }
  else if (intFirstOperationType == 2)
  {
    intSecondOperationType = random(1, 4);
    intFirstNumber = random(50, 100);
    if (intSecondOperationType == 1)
    {
      intSecondNumber = random(0, intFirstNumber);
      intThirdNumber = random(1, 100);
      strQuestion1 =  String(intFirstNumber) + strMinusSign + String(intSecondNumber) + strPlusSign + String(intThirdNumber) + strEqualSign + "?";
      intResult1 = intFirstNumber - intSecondNumber + intThirdNumber;
    }
    else if (intSecondOperationType == 2)
    {
      intSecondNumber = random(30, intFirstNumber);
      intThirdNumber = random(1, (intFirstNumber - intSecondNumber));
      strQuestion1 =  String(intFirstNumber) + strMinusSign + String(intSecondNumber) + strMinusSign + String(intThirdNumber) + strEqualSign + "?";
      intResult1 = intFirstNumber - intSecondNumber - intThirdNumber;
    }
    else // multiply
    {
      intSecondNumber = random(1, 10);
      intThirdNumber = random(1, 10);
      intFirstNumber = random(intSecondNumber * intThirdNumber, 100);
      strQuestion1 =  String(intFirstNumber) + strMinusSign + String(intSecondNumber) + strMultiplySign + String(intThirdNumber) + strEqualSign + "?";
      intResult1 = intFirstNumber - (intSecondNumber * intThirdNumber);
    }
  }
  else // first operation is plus
  {
    intSecondOperationType = random(1, 4);
    intFirstNumber = random(1, 100);
    if (intSecondOperationType == 1)
    {
      intSecondNumber = random(1, 100);
      intThirdNumber = random(1, 100);
      strQuestion1 =  String(intFirstNumber) + strPlusSign + String(intSecondNumber) + strPlusSign + String(intThirdNumber) + strEqualSign + "?";
      intResult1 = intFirstNumber + intSecondNumber + intThirdNumber;
    }
    else if (intSecondOperationType == 2)
    {
      intSecondNumber = random(1, 100);
      intThirdNumber = random(1, (intFirstNumber - intSecondNumber));
      strQuestion1 =  String(intFirstNumber) + strPlusSign + String(intSecondNumber) + strMinusSign + String(intThirdNumber) + strEqualSign + "?";
      intResult1 = intFirstNumber + intSecondNumber - intThirdNumber;
    }
    else
    {
      intSecondNumber = random(1, 10);
      intThirdNumber = random(1, 10);
      strQuestion1 =  String(intFirstNumber) + strPlusSign + String(intSecondNumber) + strMultiplySign + String(intThirdNumber) + strEqualSign + "?";
      intResult1 = intFirstNumber + (intSecondNumber * intThirdNumber);
    }
  }
  intResult1Position = strQuestion1.length() + 3;
}

void generateQuestion2() {
  intFirstOperationType = random(1, 4); // 1-plus, 2-minus, 3-multiply
  if (intFirstOperationType == 3)
  {
    intSecondOperationType = random(1, 3);
    intFirstNumber = random(1, 10);
    intSecondNumber = random(1, 10);
    if (intSecondOperationType == 2)
    {
      intThirdNumber = random(1, intFirstNumber * intSecondNumber);
      strQuestion2 =  String(intFirstNumber) + strMultiplySign + String(intSecondNumber) + strMinusSign + String(intThirdNumber) + strEqualSign + "?";
      intResult2 = intFirstNumber * intSecondNumber - intThirdNumber;
    }
    else
    {
      intThirdNumber = random(1, 100);
      strQuestion2 =  String(intFirstNumber) + strMultiplySign + String(intSecondNumber) + strPlusSign + String(intThirdNumber) + strEqualSign + "?";
      intResult2 = intFirstNumber * intSecondNumber + intThirdNumber;
    }
  }
  else if (intFirstOperationType == 2)
  {
    intSecondOperationType = random(1, 4);
    intFirstNumber = random(50, 100);
    if (intSecondOperationType == 1)
    {
      intSecondNumber = random(0, intFirstNumber);
      intThirdNumber = random(1, 100);
      strQuestion2 =  String(intFirstNumber) + strMinusSign + String(intSecondNumber) + strPlusSign + String(intThirdNumber) + strEqualSign + "?";
      intResult2 = intFirstNumber - intSecondNumber + intThirdNumber;
    }
    else if (intSecondOperationType == 2)
    {
      intSecondNumber = random(30, intFirstNumber);
      intThirdNumber = random(1, (intFirstNumber - intSecondNumber));
      strQuestion2 =  String(intFirstNumber) + strMinusSign + String(intSecondNumber) + strMinusSign + String(intThirdNumber) + strEqualSign + "?";
      intResult2 = intFirstNumber - intSecondNumber - intThirdNumber;
    }
    else // multiply
    {
      intSecondNumber = random(1, 10);
      intThirdNumber = random(1, 10);
      intFirstNumber = random(intSecondNumber * intThirdNumber, 100);
      strQuestion2 =  String(intFirstNumber) + strMinusSign + String(intSecondNumber) + strMultiplySign + String(intThirdNumber) + strEqualSign + "?";
      intResult2 = intFirstNumber - (intSecondNumber * intThirdNumber);
    }
  }
  else // first operation is plus
  {
    intSecondOperationType = random(1, 4);
    intFirstNumber = random(1, 100);
    if (intSecondOperationType == 1)
    {
      intSecondNumber = random(1, 100);
      intThirdNumber = random(1, 100);
      strQuestion2 =  String(intFirstNumber) + strPlusSign + String(intSecondNumber) + strPlusSign + String(intThirdNumber) + strEqualSign + "?";
      intResult2 = intFirstNumber + intSecondNumber + intThirdNumber;
    }
    else if (intSecondOperationType == 2)
    {
      intSecondNumber = random(1, 100);
      intThirdNumber = random(1, (intFirstNumber - intSecondNumber));
      strQuestion2 =  String(intFirstNumber) + strPlusSign + String(intSecondNumber) + strMinusSign + String(intThirdNumber) + strEqualSign + "?";
      intResult2 = intFirstNumber + intSecondNumber - intThirdNumber;
    }
    else
    {
      intSecondNumber = random(1, 10);
      intThirdNumber = random(1, 10);
      strQuestion2 =  String(intFirstNumber) + strPlusSign + String(intSecondNumber) + strMultiplySign + String(intThirdNumber) + strEqualSign + "?";
      intResult2 = intFirstNumber + (intSecondNumber * intThirdNumber);
    }
  }
  intResult2Position = strQuestion2.length() + 3;
}

void loop()
{
  if (intQuestionCounter > 50)
  {
    intQuestionCounter = 1;
  }
  if (intCurrentStage == 0)
  {
    generateQuestion1();
    display.clear();
    display.setCursor(0, 0);
    if (intQuestionCounter < 10)
    {
      display.print("0");
    }
    display.print(intQuestionCounter);
    intQuestionCounter++;
    display.setCursor(3, 0);
    display.print(strQuestion1);
    shortBeep();
    intCurrentStage++;
  }
  else if (intCurrentStage == 2)
  {
    generateQuestion2();
    display.setCursor(0, 1);
    if (intQuestionCounter < 10)
    {
      display.print("0");
    }
    display.print(intQuestionCounter);
    intQuestionCounter++;
    display.setCursor(3, 1);
    display.print(strQuestion2);
    shortBeep();
    intCurrentStage++;
  }

  int reading = digitalRead(BUTTONPIN);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    if (reading != buttonState)
    {
      buttonState = reading;
      if (buttonState == HIGH)
      {
        if (intCurrentStage == 1)
        {
          display.setCursor(intResult1Position, 0);
          display.print(intResult1);
          shortBeep();
          intCurrentStage++;
        }
        else if (intCurrentStage == 3)
        {
          display.setCursor(intResult2Position, 1);
          display.print(intResult2);
          shortBeep();
          delay(1500);
          intCurrentStage = 0;
        }
      }
    }
  }
  lastButtonState = reading;
}

void shortBeep() {
  digitalWrite(ALARMPIN, HIGH);
  delay(60);
  digitalWrite(ALARMPIN, LOW);
}

