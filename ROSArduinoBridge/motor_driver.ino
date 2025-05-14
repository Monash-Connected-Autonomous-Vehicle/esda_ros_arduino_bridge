void setMotorFreq(int motor, int speed) {
  if (speed == 0) {
    // Disable output
    if (motor == 1) TCCR1A &= ~_BV(COM1A1); // Disconnect OC1A
    if (motor == 2) TCCR1A &= ~_BV(COM1B1); // Disconnect OC1B
    return;
  }

  // Calculate frequency
  int freq;
  if (speed < 0) {
    // Map [-255, -1] → [1000, 1399]
    freq = map(speed, -255, -1, 1000, 1399);
  } else {
    // Map [1, 255] → [1601, 2000]
    freq = map(speed, 1, 255, 1601, 2000);
  }

  // Setup Timer1 for this frequency
  setupPWM_Timer1(freq);

  // Re-enable channel output
  if (motor == 1) TCCR1A |= _BV(COM1A1);
  if (motor == 2) TCCR1A |= _BV(COM1B1);

  // Duty cycle = 50%
  OCR1A = ICR1 / 2;
  OCR1B = ICR1 / 2;
}

/***************************************************************
   Motor driver definitions
   
   Add a "#elif defined" block to this file to include support
   for a particular motor driver.  Then add the appropriate
   #define near the top of the main ROSArduinoBridge.ino file.
   
   *************************************************************/

void setupPWM_Timer1(int pwm_freq_hz) {
  const long clock = 16000000;
  int top;

  if ((top = clock / (pwm_freq_hz * 1) - 1) <= 65535) {
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
  } else if ((top = clock / (pwm_freq_hz * 8) - 1) <= 65535) {
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);
  } else if ((top = clock / (pwm_freq_hz * 64) - 1) <= 65535) {
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11) | _BV(CS10);
  } else {
    return;
  }

  TCCR1A = _BV(WGM11);  // Clear COM1x1 for now, will enable in setMotorFreq
  ICR1 = top;
}


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
 /*
#elif defined ESDA_AC_MOTOR_DRIVER

  void initMotorController(){
    pinMode(MOTOR_LEFT, OUTPUT);
    pinMode(MOTOR_RIGHT, OUTPUT);
    setMotorFreq(MOTOR_LEFT_idx, 0);
    setMotorFreq(MOTOR_RIGHT_idx, 0);
    
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
  */
#else
  #error A motor driver must be selected!
#endif

#endif
