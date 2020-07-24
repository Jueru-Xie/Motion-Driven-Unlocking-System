
#include <Wire.h>
const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;
int Mcounter = 0;
float key[19][3];
float measures[19][3];

//******************
boolean autoSetup;
boolean result=false;
void setup() {
  Serial.begin(19200);
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  pinMode(LED_BUILTIN, OUTPUT);


  calculate_IMU_error();
  keySetUp();
  autoSetup = false;
  delay(20);
}
void loop() {

  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); 

  
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;

  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - 0.58;
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + 1.58;
  previousTime = currentTime;  
  currentTime = millis();   
  elapsedTime = (currentTime - previousTime) / 1000;
  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;


  gyroAngleX = gyroAngleX + GyroX * elapsedTime; 
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;

  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  
  // Print the values on the serial monitor
  //Serial.print(roll);
  Serial.print(AccX);
  Serial.print("/");
 // Serial.print(pitch);
  Serial.print(AccY);
  Serial.print("/");
  Serial.print(AccZ);
  Serial.print("/");
  Serial.print(GyroX);
  Serial.print("/");
  Serial.print(GyroY);
  Serial.print("/");
  Serial.println(GyroZ);

 
  //Serial.println(yaw);
if(Mcounter == 51){
  Mcounter = 0;
  result = false;
}

if(Mcounter == 10){
  digitalWrite(LED_BUILTIN, HIGH);
}

if(Mcounter == 20){
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("at 20");
}
if(Mcounter>20 && Mcounter<40){
  measures[Mcounter-21][0]=AccX;
  measures[Mcounter-21][1]=AccY;
  measures[Mcounter-21][1]=AccZ;
}

  if(Mcounter == 40){
    compare();
    if(autoSetup){
      autoKeySetUp();
       Serial.println("Stored");
    }
   
  }

  //looping part, the program loops every 5 second 
  //0.0-1.0 idle
  //1.0-2.0 LED on shows ready to take gesture
  //2.0-4.0 savin the gestures and matching saved guesues with the key
  //4.0-5.0 blink three times if matches, blink two times if not match.
  if(result == true){
    if(Mcounter == 40){
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if(Mcounter == 43){
       digitalWrite(LED_BUILTIN, LOW);
    }
    if(Mcounter == 46){
       digitalWrite(LED_BUILTIN, HIGH);
    }
    if(Mcounter == 49){
       digitalWrite(LED_BUILTIN, LOW);
    } 
  }

    if(result == false){
    if(Mcounter == 40){
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if(Mcounter == 42){
       digitalWrite(LED_BUILTIN, LOW);
    }
    if(Mcounter == 44){
       digitalWrite(LED_BUILTIN, HIGH);
    }
    if(Mcounter == 46){
       digitalWrite(LED_BUILTIN, LOW);
    }
    if(Mcounter == 48){
       digitalWrite(LED_BUILTIN, HIGH);
    }
    if(Mcounter == 50){
       digitalWrite(LED_BUILTIN, LOW);
    }
  }
  
  Mcounter++;
  delay(100);
}
void compare(){
 // Serial.println("into compare");
  int error = 0;
  for(int i =0; i<20; i++){
    for(int j =0; j<3; j++){
     // Serial.println("Comparing"+ i+j);
        if((key[i][j] <= measures[i][j]+0.2)&&(key[i][j] >= measures[i][j]-0.2)){
          //do nothing
        }
        else{
          error++;
        }
    }
  }

  if (error<43){
    result = true;
  }
  //Serial.println("out compare");
}
void autoKeySetUp(){
 // key = measures;
}


void keySetUp() {
  key[0][0]= -0.02;
  key[0][1]=-0.05;
  key[0][2]=0.97;
  key[1][0]=-0.02;
  key[1][1]=-0.04;
  key[1][2]=0.96;
  key[2][0]=-0.02;
  key[2][1]=-0.04;
  key[2][2]=0.97;
  key[3][0]=-0.04;
  key[3][1]=-0.03;
  key[3][2]=0.97;
  key[4][0]=0.01;
  key[4][1]=-0.13;
  key[4][2]=0.94;
  key[5][0]=-0.12;
  key[5][1]=0.02;
  key[5][2]=0.95;
  key[6][0]=-0.01;
  key[6][1]=0.02;
  key[6][2]=0.96;
  key[7][0]=0.00;
  key[7][1]=0.02;
  key[7][2]=0.96;
  key[8][0]=0.03;
  key[8][1]=-0.04;
  key[8][2]=0.95;
  key[9][0]=0.19;
  key[9][1]=-0.04;
  key[9][2]=0.89;
  key[10][0]=-0.15;
  key[10][1]=0.02;
  key[10][2]=0.99;
  key[11][0]=-0.02;
  key[11][1]=-0.04;
  key[11][2]=0.96;
  key[12][0]=-0.01;
  key[12][1]=-0.04;
  key[12][2]=0.97;
  key[13][0]=-0.07;
  key[13][1]=0.17;
  key[13][2]=1.10;
  key[14][0]=0.04;
  key[14][1]=-0.15;
  key[14][2]=0.96;
  key[15][0]=-0.02;
  key[15][1]=0.02;
  key[15][2]=0.92;
  key[16][0]=-0.04;
  key[16][1]=-0.02;
  key[16][2]=0.97;
  key[17][0]=-0.04;
  key[17][1]=-0.02;
  key[17][2]=0.96;
  key[18][0]=-0.12;
  key[18][1]=-0.05;
  key[18][2]=0.88;
  
}

void calculate_IMU_error() {
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;

    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;

  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();

    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }

  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
}
