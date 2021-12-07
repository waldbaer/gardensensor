// ------------------------------------------------------------------------------------------------
// KNX Garden Sensor
// Copyright (2021) Sebastian Waldvogel, MIT license
// ------------------------------------------------------------------------------------------------

// ---- Includes ----------------------------------------------------------------------------------
// Library for ADC ADS1115. https://github.com/RobTillaart/ADS1X15
// Library name: ADS1X15
// pinning Arduino pro mini: SCL: A5, SDA: A4
#include "ADS1X15.h"

// Low-Power library. https://github.com/rocketscream/Low-Power
// Library name: Low-Power
#include "LowPower.h"

// KNX library. https://github.com/thorsten-gehrig/arduino-tpuart-knx-user-forum
#include "KnxTpUart.h"

// ---- Configuration -----------------------------------------------------------------------------
#define GARDENSENSOR_SW_VERSION                 "0.1.0"

// Board voltage
#define VOLTAGE_3_3V                            (3300)
#define VOLTAGE_5V                              (5000)
#ifndef BOARD_VOLTAGE
  #error Board voltage is not defined. Please configure it via make environment!
#endif
// BOARD_VOLTAGE must be configured via make environment
#if ((BOARD_VOLTAGE != VOLTAGE_3_3V) && (BOARD_VOLTAGE != VOLTAGE_5V))
  #error "Unsupport board voltage configured!"
#endif

// Output type: serial / KNX
#define OUTPUT_TYPE_SERIAL                      0
#define OUTPUT_TYPE_KNX                         1
#define OUTPUT_TYPE                             OUTPUT_TYPE_KNX // OUTPUT_TYPE_SERIAL or OUTPUT_TYPE_KNX

#define SERIAL_MONITOR_BAUDRATE                 9600
#define I2C_ADDRESS_ADC                         0x48

#define ADC_GAIN                                1 // (max. voltage +-4.096V)
#define ADC_MODE                                1 // singleshot
#define ADC_DATARATE                            0 // 8 Samples/sec (default : 4 = 128 SPS)
#define ADC_CHANNEL_0                           0x00
#define ADC_CHANNEL_1                           0x01
#define ADC_CHANNEL_2                           0x02
#define ADC_CHANNEL_3                           0x03

#define ADC_INTERNAL_CHANNEL_0                  A0        // interal 10bit ADC (channel A0)
#define ADC_INTERNAL_RESOLUTION                 1023      // 10bit resolution

#define KNX_SERIAL_BAUDRATE                     19200
#define KNX_SERIAL_MODE                         SERIAL_8E1
#define KNX_PHYSICAL_ADDRESS                    "1.1.211"
#define KNX_GA_AD0                              "10/5/0"  // external 16 bit ADC (ADS1115 channel0)
#define KNX_GA_AD1                              "10/5/1"  // external 16 bit ADC (ADS1115 channel1)
#define KNX_GA_AD2                              "10/5/2"  // external 16 bit ADC (ADS1115 channel2)
#define KNX_GA_AD3                              "10/5/3"  // external 16 bit ADC (ADS1115 channel3)
#define KNX_GA_AD4                              "10/5/4"  // interal 10bit ADC (channel A0)


#define INITIAL_SETUP_DELAY                     1000 // ms
// due to bad quality of watchdog (used for lower power sleep) the absolute time must be adapted according to hardware
// correction factor: corrected_time=time*(560/646)
#define MONITORING_CYCLE_TIME                   780 // ~15min


// ---- Globals -----------------------------------------------------------------------------------
ADS1115 ADS(I2C_ADDRESS_ADC);

#if(OUTPUT_TYPE == OUTPUT_TYPE_KNX)
KnxTpUart knx(&Serial, KNX_PHYSICAL_ADDRESS);
#endif

// ---- Setup -------------------------------------------------------------------------------------
void setup()
{
  // ---- Setup peripherals ----

  // I/O pins
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, false);

  // Serial
  #if(OUTPUT_TYPE == OUTPUT_TYPE_SERIAL)
  setupSerialMonitor();
  #endif
  #if(OUTPUT_TYPE == OUTPUT_TYPE_KNX)
  setupKnx();
  #endif

  // ADC
  setupAdc();

  // Notify finalization of initialization with LED blink
  for(uint8_t blink_cnt{3}; blink_cnt > 0; blink_cnt--) {
    digitalWrite(LED_BUILTIN, true);
    delay(70);
    digitalWrite(LED_BUILTIN, false);
    delay(200);
  }

  delay(INITIAL_SETUP_DELAY);

  // ---- Print status ----
  #if(OUTPUT_TYPE == OUTPUT_TYPE_SERIAL)
  Serial.println("");
  Serial.println("== GardenSensor ==============");
  Serial.print("= GardenSensor version: ");
  Serial.println(GARDENSENSOR_SW_VERSION);

  Serial.print("= ADS1X15 version:      ");
  Serial.println(ADS1X15_LIB_VERSION);
  Serial.print("= Board voltage:        ");
  Serial.print((float)(BOARD_VOLTAGE) / 1000.0);
  Serial.println("V");

  Serial.println("== setup done ================");
  Serial.println("");
  #endif
}


// ---- Main Loop ---------------------------------------------------------------------------------
void loop()
{
  // toggle LED
  digitalWrite(LED_BUILTIN, true);

  // Read and send current ADC value
  SendAdcValues();

  digitalWrite(LED_BUILTIN, false);

  // Enter low-power mode
  LowPowerSleep(MONITORING_CYCLE_TIME);
}

// ---- Functions ---------------------------------------------------------------------------------

void setupSerialMonitor() {
    Serial.begin(SERIAL_MONITOR_BAUDRATE);
}

void setupKnx() {
  #if(OUTPUT_TYPE == OUTPUT_TYPE_KNX)
  Serial.begin(19200, SERIAL_8E1);
  knx.uartReset();
  #endif
}

void setupAdc() {
  ADS.setGain(ADC_GAIN);
  ADS.setMode(ADC_MODE);
  ADS.setDataRate(ADC_DATARATE);
  ADS.begin();
}

void SendAdcValues() {
  // Get current ADC values
  // read external ADC (16bit resolution)
  int16_t raw_adc0{ADS.readADC(ADC_CHANNEL_0)};
  float adc_voltage0{ADS.toVoltage(raw_adc0)};

  int16_t raw_adc1{ADS.readADC(ADC_CHANNEL_1)};
  float adc_voltage1{ADS.toVoltage(raw_adc1)};

  int16_t raw_adc2{ADS.readADC(ADC_CHANNEL_2)};
  float adc_voltage2{ADS.toVoltage(raw_adc2)};

  int16_t raw_adc3{ADS.readADC(ADC_CHANNEL_3)};
  float adc_voltage3{ADS.toVoltage(raw_adc3)};

  // read internal ADC (10bit resolution)
  int16_t const raw_adc4{analogRead(ADC_INTERNAL_CHANNEL_0)};
  float adc_voltage4{raw_adc4 * (((float)BOARD_VOLTAGE / 1000.0) / (float)ADC_INTERNAL_RESOLUTION)};

  // Print ADC status via serial monitor
  #if(OUTPUT_TYPE == OUTPUT_TYPE_SERIAL)
  Serial.print("Voltage monitoring: ");
  Serial.print(adc_voltage0, 3);
  Serial.print(" | ");
  Serial.print(adc_voltage1, 3);
  Serial.print(" | ");
  Serial.print(adc_voltage2, 3);
  Serial.print(" | ");
  Serial.print(adc_voltage3, 3);
  Serial.print(" | ");
  Serial.print(adc_voltage4, 3);
  Serial.print("\n");

  #elif(OUTPUT_TYPE == OUTPUT_TYPE_KNX)
  // Transmit all ADC values via KNX
  knx.groupWrite4ByteFloat(KNX_GA_AD0, adc_voltage0);
  knx.groupWrite4ByteFloat(KNX_GA_AD1, adc_voltage1);
  knx.groupWrite4ByteFloat(KNX_GA_AD2, adc_voltage2);
  knx.groupWrite4ByteFloat(KNX_GA_AD3, adc_voltage3);
  knx.groupWrite4ByteFloat(KNX_GA_AD4, adc_voltage4);
  #endif
}

void LowPowerSleep(size_t delay) {
  for(size_t sleep_8s{delay / 8}; sleep_8s > 0; sleep_8s--) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}
