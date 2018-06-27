#include <EnableInterrupt.h>

#define SERIAL_PORT_SPEED 9600
#define RC_NUM_CHANNELS  4

#define RC_CH1  0
#define RC_CH2  1
#define RC_CH3  2
#define RC_CH4  3

#define RC_CH1_INPUT  A0
#define RC_CH2_INPUT  A1
#define RC_CH3_INPUT  A2
#define RC_CH4_INPUT  A3

uint16_t rc_values[RC_NUM_CHANNELS];
uint32_t rc_start[RC_NUM_CHANNELS];
volatile uint16_t rc_shared[RC_NUM_CHANNELS];

int E1 = 6;  
int E2 = 5;                      

int M1 = 4; 
int M2 = 7;     

int speed; // Forward/Back speed
int angle;
int motorSpeedA = 0;
int motorSpeedB = 0;

int pwm = 0;
int pwma = 0;
int pwmb = 0;
void rc_read_values() {
  noInterrupts();
  memcpy(rc_values, (const void *)rc_shared, sizeof(rc_shared));
  interrupts();
}

void calc_input(uint8_t channel, uint8_t input_pin) {
  if (digitalRead(input_pin) == HIGH) {
    rc_start[channel] = micros();
  } else {
    uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
    rc_shared[channel] = rc_compare;
  }
}

void calc_ch1() { calc_input(RC_CH1, RC_CH1_INPUT); }
void calc_ch2() { calc_input(RC_CH2, RC_CH2_INPUT); }
void calc_ch3() { calc_input(RC_CH3, RC_CH3_INPUT); }
void calc_ch4() { calc_input(RC_CH4, RC_CH4_INPUT); }

void setup() {
  Serial.begin(SERIAL_PORT_SPEED);

  pinMode(M1, OUTPUT);   //dc
  pinMode(M2, OUTPUT); //dc

  pinMode(RC_CH1_INPUT, INPUT);
  pinMode(RC_CH2_INPUT, INPUT);
  pinMode(RC_CH3_INPUT, INPUT);
  pinMode(RC_CH4_INPUT, INPUT);

  enableInterrupt(RC_CH1_INPUT, calc_ch1, CHANGE);
  enableInterrupt(RC_CH2_INPUT, calc_ch2, CHANGE);
  enableInterrupt(RC_CH3_INPUT, calc_ch3, CHANGE);
  enableInterrupt(RC_CH4_INPUT, calc_ch4, CHANGE);
}

void loop() {
  rc_read_values();

    if(rc_values[RC_CH2] == 0 & rc_values[RC_CH1] ==0){
    // sets motion to stop
    Serial.println(" No Signal ");
    }
   else if(rc_values[RC_CH2] > 1530){
     // speed positive
     speed = map(rc_values[RC_CH2], 1530,1990, 0, 255);
     if(speed > 255){
      speed = 255;
      }
    } 
   else if(rc_values[RC_CH2] < 1460){
      // speed negative
     speed = map(rc_values[RC_CH2], 1460,1030, 0, -255);
    }
   else{
     speed = 0;
    }


     motorSpeedA = speed;
     motorSpeedB = speed;
//  Serial.print("MotorSpeedA: "); Serial.print(motorSpeedA); Serial.print("\t");  Serial.print("MotorSpeedB: "); Serial.print(motorSpeedB); Serial.print("\t"); Serial.println();

    angle = map(rc_values[RC_CH1], 1000,1964, 255, -255);
    
    motorSpeedA = motorSpeedA + angle;
    motorSpeedB = motorSpeedB - angle;

    if(motorSpeedA < 40 & motorSpeedA > -40){
      motorSpeedA = 0;
    }
     if(motorSpeedB < 40 & motorSpeedB > -40){
      motorSpeedB = 0;
    }

    if(motorSpeedA > 255){motorSpeedA = 255;}
    if(motorSpeedB > 255){motorSpeedB = 255;}
    if(motorSpeedA < -255){motorSpeedA = -255;}
    if(motorSpeedB < -255){motorSpeedB = -255;}
      
  Serial.print("MotorSpeedA: "); Serial.print(motorSpeedA); Serial.print("\t");  Serial.print("MotorSpeedB: "); Serial.print(motorSpeedB); Serial.print("\t"); Serial.println();

  if(motorSpeedA > 1){
    digitalWrite(M1, LOW);
      pwma = motorSpeedA;
    }
  else if(motorSpeedA < -1){
    digitalWrite(M1, HIGH);
      pwma = map(motorSpeedA, -255, 0, 0, 255);
      analogWrite(E1, pwma);
    }
  else {
    digitalWrite(M1, LOW);
    pwma = motorSpeedA;
    }

  if(motorSpeedB > 1){
    digitalWrite(M2, LOW);
      pwmb = motorSpeedB;
    }
  else if(motorSpeedB < -1){
    digitalWrite(M2, HIGH);
      pwmb = map(motorSpeedB, -255, 0, 0, 255);
      analogWrite(E2, pwmb);
    }
  else {
    digitalWrite(M2, LOW);
    pwmb = motorSpeedB;
    }

    analogWrite(E1,pwma);
    analogWrite(E2,pwmb);

//
//  if(motorSpeedA > 0){
//    digitalWrite(M1, LOW);
////    pwma = map(motorSpeedA, -255,255, 0, 255);
//      pwma = motorSpeedA;
//      analogWrite(E1, motorSpeedA);
//    }
//  else if (motorSpeedA < 0){
//    digitalWrite(M1, HIGH);
//    pwma = 255 + motorSpeedA;
//    analogWrite(E1,pwma);
//    }
//
//  if(motorSpeedB > 0){
//    digitalWrite(M2, LOW);
////    pwma = map(motorSpeedA, -255,255, 0, 255);
//      pwmb = motorSpeedB;
//      analogWrite(E2, motorSpeedB);
//    }
//  else if (motorSpeedB < 0){
//    digitalWrite(M2, HIGH);
//    pwmb = 255 + motorSpeedB;
//    analogWrite(E2,pwmb);
//    }
//    
//Serial.print("PWMA: "); Serial.print(pwma); Serial.print("\t");  Serial.print("PWMB: "); Serial.print(pwmb); Serial.print("\t"); Serial.println();
  
  delay(200);
//  if(rc_values[RC_CH2] == 0){
//    // sets motion to stop
//    Serial.println(" No Signal ");
//    }
//   else if(rc_values[RC_CH2] > 1530){
//     // speed positive
//     speed = map(rc_values[RC_CH2], 1530,1990, 0, 255);
//     if(speed > 255){
//      speed = 255;
//      }
//     digitalWrite(M1, LOW);
//     digitalWrite(M2, LOW);
//    } 
//   else if(rc_values[RC_CH2] < 1460){
//      // speed negative
//     speed = map(rc_values[RC_CH2], 1460,1030, 255, 0);
//     digitalWrite(M1, HIGH);
//     digitalWrite(M2, HIGH);
//    }




  
//  Serial.print("CH1:"); Serial.print(rc_values[RC_CH1]); Serial.print("\t"); // right joystick direction
//  Serial.print("CH2:"); Serial.print(rc_values[RC_CH2]); Serial.print("\t"); // right joystick speed
//  Serial.print("CH3:"); Serial.print(rc_values[RC_CH3]); Serial.print("\t");
//  Serial.print("CH4:"); Serial.println(rc_values[RC_CH4]);
}
