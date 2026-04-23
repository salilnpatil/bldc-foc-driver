#include <SimpleFOC.h>
#define MOT1_OUT_H PA8
#define MOT2_OUT_H PA9
#define MOT3_OUT_H PA10
#define MOT1_OUT_L PA7 // PA7     //PC13
#define MOT2_OUT_L   PB0       //PA12
#define MOT3_OUT_L   PB1     //PB1       //PB15
// Motor instance
MagneticSensorI2C sensor1 = MagneticSensorI2C(AS5600_I2C);

// example of stm32 defining 2nd bus
// TwoWire Wire1(PB11, PB10);
BLDCMotor motor = BLDCMotor(7);
BLDCDriver6PWM driver = BLDCDriver6PWM(MOT1_OUT_H, MOT1_OUT_L, MOT2_OUT_H, MOT2_OUT_L, MOT3_OUT_H, MOT3_OUT_L);
// BLDCDriver6PWM driver = BLDCDriver6PWM(8, 28, 9, 29, 10, 30, 27);
// LowsideCurrentSense currentSense = LowsideCurrentSense(0.003f, -64.0f/7.0f, A_OP1_OUT, A_OP2_OUT, A_OP3_OUT);


// encoder instance 


//Encoder encoder = Encoder(A_HALL2, A_HALL3, 2048, A_HALL1);

// // Interrupt routine intialisation
// // channel A and B callbacks
// void doA(){encoder.handleA();}
// void doB(){encoder.handleB();}
// void doIndex(){encoder.handleIndex();}

// instantiate the commander
// Commander command = Commander(Serial);
// void doTarget(char* cmd) { command.motion(&motor, cmd); }

void setup() {
  
  // // initialize encoder sensor hardware
  // encoder.init();
  // encoder.enableInterrupts(doA, doB); 
  sensor1.init();
  // // link the motor to the sensor
  motor.linkSensor(&sensor1);
  
  // driver config
  // power supply voltage [V]
  driver.voltage_power_supply = 12;
  driver.init();
  // link the motor and the driver
  motor.linkDriver(&driver);
 

  // aligning voltage [V]
  motor.voltage_sensor_align = 3;
  // index search velocity [rad/s]
  motor.velocity_index_search = 3;

  // set motion control loop to be used
    motor.foc_modulation = FOCModulationType::SpaceVectorPWM;
  motor.controller = MotionControlType::velocity_openloop;

  // contoller configuration 
  // default parameters in defaults.h

  // velocity PI controller parameters
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
  // default voltage_power_supply
  motor.voltage_limit = 6;
  // jerk control using voltage voltage ramp
  // default value is 300 volts per sec  ~ 0.3V per millisecond
  motor.PID_velocity.output_ramp = 1000;
 
  // velocity low pass filtering time constant
  motor.LPF_velocity.Tf = 0.01;

  // angle P controller
  motor.P_angle.P = 20;
  //  maximal velocity of the position control
  motor.velocity_limit = 10;


  // use monitoring with serial 
  // Serial.begin(115200);
  // comment out if not needed
  // motor.useMonitoring(Serial);
  
  // initialize motor
  motor.init();
  // align encoder and start FOC
  motor.initFOC();


  // Serial.println(F("Motor ready."));
  // Serial.println(F("Set the target angle using serial terminal:"));
  _delay(1000);
}

void loop() {

  
  // main FOC algorithm function
  motor.loopFOC();

  // Motion control function
  motor.move(5);


}