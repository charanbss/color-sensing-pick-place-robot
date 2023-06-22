#include <LiquidCrystal.h>
#include <Servo.h>

const int rs = 2, en = 3, d4 = 4, d5 = 8, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//SERVO CONTROL GLOBAL VARIABLES
Servo servo1;   //base motor
Servo servo2;   //arm motor
int dir = 0;
int pos = 0;

int R, G, B;

//Colors
int pick_color = 0; //red-1, green-2, blue-3
int drop_color = 0; //red-1, green-2, blue-3

//pins
int red = 9;
int green = 10;
int blue = 11;
int angle = 0;

//AUTO VS MANUAL (CURRENTLY NO CHOICE)
int auto_manual = -1;

//MANUAL CALIBRATION GLOBAL VARIABLES
int ldr = 0;
int ldr_array[3];

//AUTOMATIC CALIBRATION GLOBAL VARIABLES
//readings
int ldr_red = 0;
int ldr_green = 0;
int ldr_blue = 0;
int maximum = 0;
//initial max intensities & value setting
int red_int =255;
int green_int = 255;
int blue_int = 255;
//calibration
bool calibrated = false;
int tol = 20;

void ColorInput(){
  Serial.println("------");
  Serial.println("Where to pick the object?:");
  Serial.println("1. Red");
  Serial.println("2. Green");
  Serial.println("3. Blue");
  Serial.println("------");
  
  while(Serial.available() == 0){}

  int pickChoice = Serial.read();
  switch (pickChoice) {
    case 49:
      pick_color = 1;
      break;
    case 50:
      pick_color = 2;
      break;
    case 51:
      pick_color = 3;
      break;
    default:
      Serial.println("Please give a valid input!");
      break;
  }
  Serial.print("You chose:");
  Serial.println(pick_color);
  
  Serial.println("------");
  Serial.println("Where to drop the object?:");
  Serial.println("1. Red");
  Serial.println("2. Green");
  Serial.println("3. Blue");
  Serial.println("------");
  
  while(Serial.available() == 0){}

  int dropChoice = Serial.read();
  switch (dropChoice) {
    case 49:
      drop_color = 1;
      break;
    case 50:
      drop_color = 2;
      break;
    case 51:
      drop_color = 3;
      break;
    default:
      Serial.println("Please give a valid input!");
      break;
  }
  Serial.print("You chose:");
  Serial.println(drop_color);
}

int ColorLogic() {
  //Serial.println(R);
  //Serial.println(G);
  //Serial.println(B);
  
  if(R<G && R<B){
    if(G-R>20 && B-R>20){
      Serial.println("It's red down there!\n");
      return 1;
    }
  }
  
  if(R>G && R>B){
    if(R-G>20 && R-B >20){
      Serial.println("It's green down there!\n");
      return 2;
    }
  }
  
  if(B<R && B<G){
    if(R-B>20 && G-B>20){
      Serial.println("It's blue down there!\n");
      return 3;
    }
  }
}

void FinalAction() {
  servo1.write(6);
  delay(500);
  pos =6;
  Serial.print("angle =");
  Serial.println(pos);
  bool cond1 = false;
  while(!cond1){
    if(auto_manual == 0){
      postAutoReadings();
    }
    else{
      postManualReadings();
    }

    if(pick_color == ColorLogic()){
      cond1 = true;
    }
    //Serial.println(cond1);

    pos += 3;
    servo1.write(pos);
    delay(15);
  }
  
  arm_pickup();

  servo1.write(6);
  delay(500);
  pos = 6;
  bool cond2 = false;
  while(!cond2){
    if(auto_manual == 0){
      postAutoReadings();
    }
    else{
      postManualReadings();
    }

    if(drop_color == ColorLogic()){
      cond2 = true;
    }

    Serial.print("angle =");
    Serial.println(pos);
    //Serial.println(cond2);

    pos += 3;
    servo1.write(pos);
    delay(15);
  }

  arm_drop();

  lcd.clear();
  lcd.print("Yayy! Done.");
}

void Auto_Manual() {
  Serial.println("------");
  Serial.println("Choose a calibration:");
  Serial.println("1. Automatic Calibration");
  Serial.println("2. Manual Calibration");
  Serial.println("------");
  
  while(Serial.available() == 0){}

  int menuChoice = Serial.read();
  Serial.print("You chose:");
  switch (menuChoice) {
    case 50:
      auto_manual = 0;
      ManualCalibrate();
      break;
    case 49:
      auto_manual = 1;
      AutoCalibrate();
      break;
    default:
      Serial.println("Please give a valid input!");
  }
  
  //Serial.println(auto_manual+1);
}

void ManualCalibrate() {
  Serial.print("MANUAL CALIBRATION\n");
  lcd.clear();
  lcd.print("Manual");
  delay(500);
  lcd.clear();
  servo1.write(70);
  Serial.print("Enter 0 as input to stop calibration!\n");
  
  while(!calibrated) {
  //red intensity
  digitalWrite(red, HIGH);
  delay(1500);
  ldr = analogRead(A2);
  Serial.print("red=");
  Serial.println(ldr);
  lcd.setCursor(0, 0); 
  lcd.print("R:");
  lcd.print(ldr);
  digitalWrite(red, LOW);

  //green intensity
  digitalWrite(green, HIGH);
  delay(1500);
  ldr = analogRead(A2);
  Serial.print("green=");
  Serial.println(ldr);
  lcd.print(" G:");
  lcd.print(ldr);
  digitalWrite(green, LOW);

  //blue intensity
  digitalWrite(blue, HIGH);
  delay(1500);
  ldr = analogRead(A2);
  Serial.print("blue=");
  Serial.println(ldr);
  lcd.setCursor(0, 1);
  lcd.print("B:");
  lcd.print(ldr);
  digitalWrite(blue, LOW);
  
  lcd.display();
  delay(1500);
  lcd.clear();

  int input = Serial.read();
  if(input == 48){
    calibrated = true;
    lcd.print("Calibrated!");
    Serial.print("Calibration stopped by the user\n"); 
  }
 }

 Serial.print("----------\n");
 delay(500);
}

void postManualReadings(){
  //showing readings
  //red intensity
  digitalWrite(red, HIGH);
  delay(1000);
  int ldr1 = analogRead(A2);
  Serial.print("red=");
  Serial.println(ldr1);
  lcd.setCursor(0, 0); 
  lcd.print("R:");
  lcd.print(ldr1);
  digitalWrite(red, LOW);
  
  //green intensity
  digitalWrite(green, HIGH);
  delay(1000);
  int ldr2 = analogRead(A2);
  Serial.print("green=");
  Serial.println(ldr2);
  lcd.print(" G:");
  lcd.print(ldr2);
  digitalWrite(green, LOW);
  
  //blue intensity
  digitalWrite(blue, HIGH);
  delay(1000);
  int ldr3 = analogRead(A2);
  Serial.print("blue=");
  Serial.println(ldr3);
  lcd.setCursor(0, 1);
  lcd.print("B:");
  lcd.print(ldr3);
  digitalWrite(blue, LOW);
  
  lcd.display();
  delay(500);

  R = ldr1;
  G = ldr2;
  B = ldr3;

  //int ldr_array[3] = {ldr1,ldr2,ldr3};
  //return ldr_array;
}

void AutoCalibrate() {
  Serial.print("AUTOMATIC CALIBRATION");
  lcd.clear();
  lcd.print("Automatic");
  delay(500);

  servo1.write(70);
  
  while(!calibrated){
  Serial.print("\nReadings:\n");
  //red intensity
  analogWrite(red, red_int);
  delay(1000);
  ldr_red = analogRead(A2);
  Serial.print("Red = ");
  Serial.println(ldr_red);
  lcd.setCursor(0, 0); 
  lcd.print("R:");
  lcd.print(ldr_red);
  analogWrite(red, 0);

  //green intensity
  analogWrite(green, green_int);
  delay(1000);
  ldr_green = analogRead(A2);
  Serial.print("Green = ");
  Serial.println(ldr_green);
  lcd.print(" G:");
  lcd.print(ldr_green);
  analogWrite(green, 0);

  //blue intensity
  analogWrite(blue, blue_int);
  delay(1000);
  ldr_blue = analogRead(A2);
  Serial.print("Blue = ");
  Serial.println(ldr_blue);
  lcd.setCursor(0, 1); 
  lcd.print("B:");
  lcd.print(ldr_blue);
  analogWrite(blue, 0);
  
  lcd.display();
  delay(500);

  if (ldr_red>ldr_green && ldr_red>ldr_blue){
      maximum =ldr_red;
      }
    else if(ldr_green>ldr_blue){
      maximum =ldr_green;
      }
    else{
      maximum =ldr_blue;
      }

  if(!calibrated){
    if((maximum-ldr_red) > tol)
    {red_int = red_int - 1;}
    if((maximum-ldr_green) > tol)
    {green_int = green_int - 1;}
    if((maximum-ldr_blue) > tol)
    {blue_int = blue_int - 1;}

    Serial.print("\nNew intensities:\n");
    Serial.print("Red = ");
    Serial.println(red_int);
    Serial.print("Green = ");
    Serial.println(green_int);
    Serial.print("Blue = ");
    Serial.println(blue_int);
    }

  //calibration check
  if((((maximum-ldr_red)<tol) && ((maximum-ldr_green)<tol)) && ((maximum-ldr_blue)<tol)){
    calibrated = true;
    lcd.clear();
    lcd.print("Done");
    Serial.print("\nCalibration Done\n");
    delay(1000);
    }
  
  Serial.print("----------\n");
  lcd.clear();
  }

  lcd.print("Calibrated!");

  delay(500);
}

void postAutoReadings(){
  //showing readings
  //red intensity
  analogWrite(red, red_int);
  delay(1000);
  int ldr1 = analogRead(A2);
  Serial.print("red=");
  Serial.println(ldr1);
  lcd.setCursor(0, 0); 
  lcd.print("R:");
  lcd.print(ldr1);
  analogWrite(red, 0);
  
  //green intensity
  analogWrite(green, green_int);
  delay(1000);
  int ldr2 = analogRead(A2);
  Serial.print("green=");
  Serial.println(ldr2);
  lcd.print(" G:");
  lcd.print(ldr2);
  analogWrite(green, 0);
  
  //blue intensity
  analogWrite(blue, blue_int);
  delay(1000);
  int ldr3 = analogRead(A2);
  Serial.print("blue=");
  Serial.println(ldr3);
  lcd.setCursor(0, 1);
  lcd.print("B:");
  lcd.print(ldr3);
  analogWrite(blue, 0);
  
  lcd.display();
  delay(500);

  R = ldr1;
  G = ldr2;
  B = ldr3;
  
  //int ldr_array[3] = {ldr1, ldr2, ldr3};

  //return &ldr_array[0];
}

void arm_pickup() {
  int pos_here = 160;
  
  while(true){
    pos_here-=2;
    servo2.write(pos_here);

    int prox = analogRead(A1);
    Serial.println(prox);

    if(prox < 80){
      break;
    }
  }

  digitalWrite(7, HIGH);
  delay(3000);
  servo2.write(160);
}

void arm_drop() {
  int pos_here = 160;
  
  while(true){
    pos_here-=2;
    servo2.write(pos_here);

    int prox = analogRead(A1);
    Serial.println(prox);

    if(prox < 80){
      break;
    }
  }

  digitalWrite(7, LOW);
  delay(1000);
  servo2.write(160);
}

void setup() {
  Serial.begin(9600);
  servo1.attach(5);       //base motor
  servo1.write(60);
  servo2.attach(6);       //arm motor
  servo2.write(160);
  pinMode(red, OUTPUT);   //color sensor red
  pinMode(green, OUTPUT); //color sensor green
  pinMode(blue, OUTPUT);  //color sensor blue
  pinMode(A1, INPUT);     //proximity sensor
  pinMode(A2, INPUT);     //color sensor LDR
  pinMode(7, OUTPUT);     //electromagnet
  lcd.begin(16, 2);
  lcd.print("Hello!");
  delay(1500);
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("Choose mode of");
  lcd.setCursor(0,1);
  lcd.print("calibration");
  Auto_Manual();
  lcd.clear();
}

void loop(){
  lcd.setCursor(0,0);
  lcd.print("Choose colours");
  lcd.setCursor(0,1);
  lcd.print("to move object");
  ColorInput();
  lcd.clear();

  FinalAction();
}
