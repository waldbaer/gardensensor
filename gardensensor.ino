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
#define GARDENSENSOR_SW_VERSION                 "0.0.1"

#define OUTPUT_TYPE_SERIAL                      0
#define OUTPUT_TYPE_KNX                         1

// OUTPUT_TYPE: OUTPUT_TYPE_SERIAL or OUTPUT_TYPE_KNX
#define OUTPUT_TYPE                             OUTPUT_TYPE_KNX

#define SERIAL_MONITOR_BAUDRATE                 9600
#define I2C_ADDRESS_ADC                         0x48

#define ADC_GAIN                                1 // (max. voltage +-4.096V)
#define ADC_MODE                                1 // singleshot
#define ADC_DATARATE                            0 // 8 Samples/sec (default : 4 = 128 SPS)
#define ADC_CHANNEL_CISTERN_WATER_PRESSURE      0x00
#define ADC_CHANNEL_1                           0x01
#define ADC_CHANNEL_2                           0x02
#define ADC_CHANNEL_3                           0x03

#define KNX_SERIAL_BAUDRATE                     19200
#define KNX_SERIAL_MODE                         SERIAL_8E1
#define KNX_PHYSICAL_ADDRESS                    "1.1.220"
#define KNX_GA_CISTERN_WATER_PRESSURE_STATUS    "10/5/0"

#define INITIAL_SETUP_DELAY                     1000 // ms
#define MONITORING_CYCLE_TIME                   3600 // sec


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

  // Serial
  #if(OUTPUT_TYPE == OUTPUT_TYPE_SERIAL)
  setupSerialMonitor();
  #endif
  #if(OUTPUT_TYPE == OUTPUT_TYPE_KNX)
  setupKnx();
  #endif

  // ADC
  setupAdc();

  delay(INITIAL_SETUP_DELAY);

  // ---- Print status ----
  #if(OUTPUT_TYPE == OUTPUT_TYPE_SERIAL)
  Serial.println("");
  Serial.println("== GardenSensor ==============");
  Serial.print("= GardenSensor version: ");
  Serial.println(GARDENSENSOR_SW_VERSION);

  Serial.print("= ADS1X15 version:      ");
  Serial.println(ADS1X15_LIB_VERSION);
  Serial.println("== setup done ================");
  Serial.println("");
  #endif
}


// ---- Main Loop ---------------------------------------------------------------------------------
void loop()
{
  // toggle LED
  digitalWrite(LED_BUILTIN, true);

  // Print ADC status via serial monitor
  #if(OUTPUT_TYPE == OUTPUT_TYPE_SERIAL)
  PrintAdcStatus();
  #endif

  // Send ADC via KNX telegrams
  #if(OUTPUT_TYPE == OUTPUT_TYPE_KNX)
  SendAdcStatusKnx();
  #endif

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

void PrintAdcStatus(){
  int16_t raw_channel0{ADS.readADC(ADC_CHANNEL_CISTERN_WATER_PRESSURE)};
  int16_t raw_channel1{ADS.readADC(ADC_CHANNEL_1)};
  int16_t raw_channel2{ADS.readADC(ADC_CHANNEL_2)};
  int16_t raw_channel3{ADS.readADC(ADC_CHANNEL_3)};

  Serial.print("Voltage monitoring: ");
  Serial.print(ADS.toVoltage(raw_channel0), 3);
  Serial.print(" | ");
  Serial.print(ADS.toVoltage(raw_channel1), 3);
  Serial.print(" | ");
  Serial.print(ADS.toVoltage(raw_channel2), 3);
  Serial.print(" | ");
  Serial.print(ADS.toVoltage(raw_channel3), 3);
  Serial.print("\n");
}

void SendAdcStatusKnx() {
  #if(OUTPUT_TYPE == OUTPUT_TYPE_KNX)

  // Cistern water pressure status: Send current ADC value
  int16_t raw_adc{ADS.readADC(ADC_CHANNEL_CISTERN_WATER_PRESSURE)};
  float adc_voltage{ADS.toVoltage(raw_adc)};
  knx.groupWrite4ByteFloat(KNX_GA_CISTERN_WATER_PRESSURE_STATUS, adc_voltage);

  #endif
}


void LowPowerSleep(size_t delay) {
  for(size_t sleep_8s{delay / 8}; sleep_8s > 0; sleep_8s--) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}
