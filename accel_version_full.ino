#include <AccelStepper.h>
#define HALFSTEP 8

#define motorPin1  4
#define motorPin2  5
#define motorPin3  6
#define motorPin4  7

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

int homePosition;
boolean foundHome = false;
long revCounter = 0;
int stepQueue = 0;
volatile unsigned long lastInterruptTime = micros();

void setup() {
  //debug
  Serial.begin(9600);
  //debug
  
  pinMode(2, INPUT);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(0, homePosInterrupt, RISING);
  attachInterrupt(1, forceIncrement, FALLING);
  
  stepper1.setMaxSpeed(1000);
  stepper1.setAcceleration(500.0);
  
  findHome();
}

void loop() {
  int stepsToMove = 0;
  //set slide holder to intial actual home position from which
  //it will increment in 1/6ths
  if((stepper1.distanceToGo() == 0) && (foundHome == false)){
    //debug
    Serial.println("moving to zero position");
    //debug
    
    stepper1.runToNewPosition(homePosition - 1850);
    stepper1.setCurrentPosition(0);
    foundHome = true;
  }  
  
  //increment 1/6th rev at a time.  due to gearing ratios (4096 half steps/360deg)
  //(4096 / 6 = 682.67) every third increment must move one half step less to make
  //the average increment equal to 682.67 steps.
  if(stepper1.distanceToGo() == 0){
    if(revCounter == 2){
      stepsToMove = 682;
      revCounter = 0;
    }
    else{
      stepsToMove = 683;
      revCounter++;
    }
    stepper1.move(stepsToMove);
  }  
  
  stepsToMove = 0;  
  stepper1.run();
}

void findHome(){
  //debug
  Serial.println("finding home");
  //debug
  
  stepper1.moveTo(4096);

  //debug
  Serial.println(" home pos: ");
  Serial.println(homePosition);
  //debug  
}

void homePosInterrupt(){
  if((micros()-lastInterruptTime) >= 1000000){
    homePosition = stepper1.currentPosition();  
    //debug
    Serial.println(homePosition);
    Serial.println((micros()-lastInterruptTime));
    //debug
  }
  lastInterruptTime = micros();
}

int nextIncrementValue(){
  if(revCounter == 2){
      return 682;
      revCounter = 0;
  }
  else{
    return 683;
    revCounter++;
  }
}

int addIncrementToQueue(){
  return int(nextIncrementValue);
}

void forceIncrement(){
  addIncrementToQueue();
}
