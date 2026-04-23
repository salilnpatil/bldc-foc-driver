#include "SPI.h"
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "encoders/ma730/MagneticSensorMA730.h"
#include "utilities/stm32pwm/STM32PWMInput.h"
#include "utilities/stm32math/STM32G4CORDICTrigFunctions.h"
#include "settings/stm32/STM32FlashSettingsStorage.h"

//#include "F405_hw.h"
#include "G431_hw.h"
#define V_PSU 12.0f
#define V_LIMIT 12.0f
#define GEAR_REDUCTION 3          // ON PITCH Axis
#define ACCESSIBLE_ANGLE 3.00f    // mechanically available movement on PITCH axis in radians

// #define PWM_IN_MIN 4.36f
// #define PWM_IN_MAX 10.61f

#define PWM_IN_MIN 4.94f
#define PWM_IN_MAX 10.06f

float motor_angle;
float max_angle;
float min_angle;
float target_angle;

HardwareSerial Serial3(PIN_SERIAL_RX, PIN_SERIAL_TX);

MagneticSensorMA730 sensor1(SENSOR1_CS);

STM32PWMInput pwmInput = STM32PWMInput(PWM_IN2);

STM32FlashSettingsStorage settings = STM32FlashSettingsStorage();

// Motor instance
BLDCMotor motor = BLDCMotor(7);
BLDCDriver6PWM driver = BLDCDriver6PWM(MOT1_OUT_H, MOT1_OUT_L, MOT2_OUT_H, MOT2_OUT_L, MOT3_OUT_H, MOT3_OUT_L, ENABLE);

long ts;      // timestamp
int its = 0;  // loop iterations per second

struct FloatMapper {
  inline FloatMapper(float min_in, float max_in, float min_out, float max_out)
    : _mult((max_out - min_out) / (max_in - min_in)), _bias_out(min_out), _bias_in(min_in) {}
  inline void map(float input, float &output) {
    output = (input - _bias_in) * _mult + _bias_out;
  }
  long double _mult;
  float _bias_out, _bias_in;
};

FloatMapper *floatMapper1;


void input_Mapper() {

  // SimpleFOCDebug::println("inside input mapper");

  while (digitalRead(LIMIT_SW) == HIGH) {

    motor.loopFOC();
    motor_angle += 0.0007f;
    motor.move(motor_angle);
  }

  max_angle = sensor1.getAngle();
  min_angle = (max_angle - (ACCESSIBLE_ANGLE * GEAR_REDUCTION));

  floatMapper1 = new FloatMapper(PWM_IN_MIN, PWM_IN_MAX, min_angle, max_angle);

  SimpleFOCDebug::print("Min Angle: ");
  SimpleFOCDebug::println(min_angle);

  SimpleFOCDebug::print("Max Angle: ");
  SimpleFOCDebug::println(max_angle);
  delay(300);
}



void setup() {
  Serial3.begin(115200);
  SimpleFOCDebug::enable(&Serial3);
  motor.useMonitoring(Serial3);

  pinMode(LIMIT_SW, INPUT_PULLUP);
  SimpleFOC_CORDIC_Config();

  delay(300);
  SimpleFOCDebug::print("MCU Speed: ");
  SimpleFOCDebug::println((int)SystemCoreClock);
  SimpleFOCDebug::println();

  pwmInput.initialize();

  sensor1.init();

  driver.voltage_power_supply = V_PSU;
  driver.voltage_limit = V_LIMIT;

  motor.linkDriver(&driver);
  motor.linkSensor(&sensor1);
  motor.voltage_limit = V_LIMIT / 2.0f;
  motor.controller = MotionControlType::angle;
  motor.torque_controller = TorqueControlType::voltage;

//  motor.sensor_direction = CW;
// motor.zero_electric_angle = 4.85;

  // velocity PI controller parameters
  motor.PID_velocity.P = 0.2f;
  motor.PID_velocity.I = 4;
  motor.PID_velocity.D = 0;

  motor.LPF_velocity.Tf = 0.0001f;  // velocity low pass filtering time constant
                                    // the lower the less filtered

  motor.P_angle.P = 10;       // angle P controller
  motor.velocity_limit = 50;  // maximal velocity of the position control

  driver.init();
  motor.init();
  motor.initFOC();

  motor_angle = sensor1.getAngle();
  SimpleFOCDebug::print("Angle: ");
  SimpleFOCDebug::println(motor_angle);

  SimpleFOCDebug::print("Limit SW posi: ");
  SimpleFOCDebug::println(digitalRead(LIMIT_SW));

  input_Mapper();

  ts = millis();
  SimpleFOCDebug::println("SimpleFOC G4 initialized.");
  SimpleFOCDebug::println();
}

void loop() {


  motor.loopFOC();

  floatMapper1->map(pwmInput.getDutyCyclePercent(), target_angle);

  if(target_angle > max_angle)
    target_angle = max_angle;

  else if(target_angle < min_angle)
    target_angle = min_angle;

  motor.move(target_angle);

  // its++; 
  // if (millis() - ts > 100) {
  //   ts = millis();
  //   // dutyPercent = pwmInput.getDutyCyclePercent();
  //   // motor.move(dutyPercent);
  //   // SimpleFOCDebug::println(its);

  //   // SimpleFOCDebug::println(dutyPercent);

  //   // if (digitalRead(LIMIT_SW) == HIGH)
  //   //   SimpleFOCDebug::println("LIMIT_SW: ON");
  //   // else
  //   //   SimpleFOCDebug::println("LIMIT_SW: OFF");

  //   SimpleFOCDebug::print("Current Angle: ");
  //   SimpleFOCDebug::println(sensor1.getAngle());

  //     SimpleFOCDebug::print("Target Angle: ");
  //   SimpleFOCDebug::println(target_angle);

  //   its = 0;
  // }
}
