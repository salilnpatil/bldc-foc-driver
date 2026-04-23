#ifndef F405_HW_H
#define F405_HW_H

/*
 * F405_hw.h — board pin map for the BLDC FOC driver board (STM32F405RGT6).
 *
 * Values are derived directly from the committed EagleCAD schematic at
 *   Hardware_assets/Design_FIle(EagleCad)/Layout_Design_File.sch
 *
 * Two categories of pins below:
 *
 *   HARDWIRED  — physically routed on the PCB from the MCU to the DRV8323
 *                gate driver through series resistors (R19..R27). These
 *                values are authoritative and should not be changed.
 *
 *   EXTERNAL   — the board brings the DRV8323 control pins (ENABLE, FAULT,
 *                CAL) out on JP3, and spare MCU GPIOs out on JP11..JP15.
 *                The connections from MCU GPIO to DRV-ENABLE, to the MA730
 *                encoder's chip-select, and to the limit switch / PWM
 *                input / debug UART are made off-board with jumper wires
 *                chosen at assembly time. The placeholders below are
 *                *suggested defaults* taken from pins broken out on JP15
 *                (GPIOs) and JP12 (UART-capable).
 *
 *                >>> VERIFY these match your physical jumper wiring before
 *                >>> flashing. A wrong ENABLE pin can leave the DRV8323 in
 *                >>> a permanently enabled state with no software control.
 *
 * Note: the parent sketch also calls into the STM32G4 hardware CORDIC,
 * which does not exist on the F4 family. Those calls must be removed for
 * the F405 target. This header only defines pins; it does not fix that.
 */

// ---------------------------------------------------------------------------
// HARDWIRED — 6-PWM gate-driver inputs to DRV8323RS (via R20/R23..R27)
// ---------------------------------------------------------------------------
// Phase A:  PA11 (R27) -> INHA,  PA10 (R26) -> INLA
// Phase B:  PA9  (R25) -> INHB,  PC8  (R24) -> INLB
// Phase C:  PC7  (R23) -> INHC,  PC6  (R20) -> INLC
//
// Timer caveat: these 6 pins span TIM1 CH2/CH3/CH4 (PA9/PA10/PA11) and
// TIM3/TIM8 CH1/CH2/CH3 (PC6/PC7/PC8). PA11 is TIM1_CH4 which has no
// complementary output on the F4, and phase B's high-side (PA9) and
// low-side (PC8) sit on different timers. SimpleFOC's BLDCDriver6PWM will
// therefore fall back to independent-channel PWM with software dead-time.
// Confirm phasing and dead-time on a scope during bring-up.
#define MOT1_OUT_H  PA11
#define MOT1_OUT_L  PA10
#define MOT2_OUT_H  PA9
#define MOT2_OUT_L  PC8
#define MOT3_OUT_H  PC7
#define MOT3_OUT_L  PC6

// ---------------------------------------------------------------------------
// HARDWIRED — SPI bus from MCU to DRV8323RS (via R19/R21/R22)
// ---------------------------------------------------------------------------
// PC10 -> SCLK, PC11 -> MISO (R21 -> DRV SDO), PC12 -> MOSI (R22 -> DRV SDI),
// PB5  -> CS   (R19 -> DRV SCS_N). This SPI is used only for DRV8323
// configuration and is not exercised by the current sketch.
#define DRV8323_CS  PB5

// ---------------------------------------------------------------------------
// EXTERNAL — jumper-wired via headers. Verify against your board.
// ---------------------------------------------------------------------------

// MA730 magnetic encoder chip-select. The MA730 shares the MCU SPI bus
// (SCLK=PC10, MISO=PC11, MOSI=PC12) with the DRV8323; only CS is unique.
// Available GPIOs on JP15: PC0, PC1, PC2, PC3, PA0, PA1.
// Default: PC0 (JP15 pin 2).
#define SENSOR1_CS  PC0

// DRV8323 master ENABLE. Jumper JP3 pin 7 to an MCU GPIO.
// Default: PC1 (JP15 pin 3).
#define ENABLE      PC1

// Homing limit switch (sketch enables INPUT_PULLUP).
// Default: PC2 (JP15 pin 4).
#define LIMIT_SW    PC2

// External RC-style PWM setpoint input. Must land on a pin that maps to a
// timer input-capture channel in the stm32duino core.
// Default: PA0 (JP15 pin 6) — TIM2_CH1 / TIM5_CH1 on STM32F405.
#define PWM_IN2     PA0

// Debug UART (SimpleFOC monitor / println). On STM32F405, USART3 is
// available on PB10 (TX) / PB11 (RX), both broken out on JP12. The I2C
// pull-ups R16/R17 on these nets do not interfere with UART operation.
#define PIN_SERIAL_TX  PB10
#define PIN_SERIAL_RX  PB11

#endif // F405_HW_H
