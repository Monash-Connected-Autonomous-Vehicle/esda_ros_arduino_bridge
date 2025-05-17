#ifdef USE_BASE
   
#ifdef POLOLU_VNH5019
  /* Include the Pololu library */
  #include "DualVNH5019MotorShield.h"

  /* Create the motor driver object */
  DualVNH5019MotorShield drive;
  
  /* Wrap the motor driver initialization */
  void initMotorController() {
    drive.init();
  }

  /* Wrap the drive motor set speed function */
  void setMotorSpeed(int i, int spd) {
    if (i == LEFT) drive.setM1Speed(spd);
    else drive.setM2Speed(spd);
  }

  // A convenience function for setting both motor speeds
  void setMotorSpeeds(int leftSpeed, int rightSpeed) {
    setMotorSpeed(LEFT, leftSpeed);
    setMotorSpeed(RIGHT, rightSpeed);
  }
#elif defined POLOLU_MC33926
  /* Include the Pololu library */
  #include "DualMC33926MotorShield.h"

  /* Create the motor driver object */
  DualMC33926MotorShield drive;
  
  /* Wrap the motor driver initialization */
  void initMotorController() {
    drive.init();
  }

  /* Wrap the drive motor set speed function */
  void setMotorSpeed(int i, int spd) {
    if (i == LEFT) drive.setM1Speed(spd);
    else drive.setM2Speed(spd);
  }

  // A convenience function for setting both motor speeds
  void setMotorSpeeds(int leftSpeed, int rightSpeed) {
    setMotorSpeed(LEFT, leftSpeed);
    setMotorSpeed(RIGHT, rightSpeed);
  }
#elif defined L298_MOTOR_DRIVER
  void initMotorController() {
    digitalWrite(RIGHT_MOTOR_ENABLE, HIGH);
    digitalWrite(LEFT_MOTOR_ENABLE, HIGH);
  }
  
  void setMotorSpeed(int i, int spd) {
    unsigned char reverse = 0;
  
    if (spd < 0)
    {
      spd = -spd;
      reverse = 1;
    }
    if (spd > 255)
      spd = 255;
    
    if (i == LEFT) { 
      if      (reverse == 0) { analogWrite(LEFT_MOTOR_FORWARD, spd); analogWrite(LEFT_MOTOR_BACKWARD, 0); }
      else if (reverse == 1) { analogWrite(LEFT_MOTOR_BACKWARD, spd); analogWrite(LEFT_MOTOR_FORWARD, 0); }
    }
    else /*if (i == RIGHT) //no need for condition*/ {
      if      (reverse == 0) { analogWrite(RIGHT_MOTOR_FORWARD, spd); analogWrite(RIGHT_MOTOR_BACKWARD, 0); }
      else if (reverse == 1) { analogWrite(RIGHT_MOTOR_BACKWARD, spd); analogWrite(RIGHT_MOTOR_FORWARD, 0); }
    }
  }
  
  void setMotorSpeeds(int leftSpeed, int rightSpeed) {
    setMotorSpeed(LEFT, leftSpeed);
    setMotorSpeed(RIGHT, rightSpeed);
  }
 
#elif defined ESDA_AC_MOTOR_DRIVER
  #include <Servo.h>
  Servo Servo_l;
  Servo Servo_r;
  
  void initMotorController(){
    /*Define the Servo Pins*/
    Servo_l.attach(MOTOR_LEFT);
    Servo_r.attach(MOTOR_RIGHT);

    Servo_l.writeMicroseconds(1500);  // Gearing Left Motor
    Servo_r.writeMicroseconds(1500);  // Gearing Right Motor
    delay(3000);
  }

  void setMotorFreq(int motor_idx, int spd) {
    spd = constrain(spd, -255, 255); // Ensure speed is within valid range
    
    int us;
    if (spd == 0) {
      us = 1500; // Stop command within the 1450-1550 stop zone
    } else if (spd < 0) {
      // Map reverse speeds (-255 to -1) to 1200-1400 μs
      us = map(spd, -255, -1, 1200, 1400);
    } else {
      // Map forward speeds (1 to 255) to 1600-1800 μs
      us = map(spd, 1, 255, 1600, 1800);
    }

    // Clamp the output to valid ranges (some ESCs require strict boundaries)
    us = constrain(us, 1200, 1800);
    
    // Apply the command to the appropriate motor
    if (motor_idx == MOTOR_LEFT_idx) {
      Servo_l.writeMicroseconds(us);
    } else {
      Servo_r.writeMicroseconds(us);
    }
  }
  
  void setMotorSpeed(int i, int spd) {
    if (i == LEFT) {
      setMotorFreq(MOTOR_LEFT_idx, spd);
    }
    else {
      setMotorFreq(MOTOR_RIGHT_idx, spd);
    }
  }
  
  void setMotorSpeeds(int leftSpeed, int rightSpeed) {
    setMotorSpeed(LEFT, leftSpeed);
    setMotorSpeed(RIGHT, rightSpeed);
  }
 
#else
  #error A motor driver must be selected!
#endif

#endif
