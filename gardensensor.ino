// ---- Includes ----------------------------------------------------------------------------------
// Library for ADC ADS1115. https://github.com/RobTillaart/ADS1X15
// Library name: ADS1X15
#include "ADS1X15.h"

// Low-Power library. https://github.com/rocketscream/Low-Power
// Library name: Low-Power
#include "LowPower.h"

// ---- Configuration -----------------------------------------------------------------------------
#define GARDENSENSOR_SW_VERSION "0.0.1"


#define SERIAL_MONITOR_BAUDRATE 9600  // baud
#define I2C_ADDRESS_ADC         0x48

#define ADC_GAIN                   1 // (max. voltage +-4.096V)
#define ADC_MODE                   1 // singleshot
#define ADC_DATARATE               0 // 8 Samples/sec (default : 4 = 128 SPS)
#define ADC_CHANNEL_WATER_PRESSURE 0x00
#define ADC_CHANNEL_1              0x01
#define ADC_CHANNEL_2              0x02
#define ADC_CHANNEL_3              0x03


// ---- Globals -----------------------------------------------------------------------------------
ADS1115 ADS(I2C_ADDRESS_ADC);

boolean led_state{false};

// ---- Setup -------------------------------------------------------------------------------------
void setup()
{
  // ---- Setup peripherals ----

  // I/O pins
  pinMode(LED_BUILTIN, OUTPUT);

  // Serial
  Serial.begin(SERIAL_MONITOR_BAUDRATE);

  // ADC
  ADS.setGain(ADC_GAIN);
  ADS.setMode(ADC_MODE);
  ADS.setDataRate(ADC_DATARATE);
  ADS.begin();

  delay(1000);

  // ---- Print status ----
  Serial.println("");
  Serial.println("== GardenSensor ==============");
  Serial.print("= GardenSensor version: ");
  Serial.println(GARDENSENSOR_SW_VERSION);

  Serial.print("= ADS1X15 version:      ");
  Serial.println(ADS1X15_LIB_VERSION);
  Serial.println("== setup done ================");
  Serial.println("");
}

// ---- Main Loop ---------------------------------------------------------------------------------
void loop()
{
  // toggle LED
  digitalWrite(LED_BUILTIN, led_state);
  led_state = !led_state;

  // ADC status
  PrintAdcStatus();

  // Enter low-power mode
  delay(1000);
  EnterLowPowerMode();

}

// ---- Functions ---------------------------------------------------------------------------------
void PrintAdcStatus(){
  int16_t raw_channel0 = ADS.readADC(ADC_CHANNEL_WATER_PRESSURE);
  int16_t raw_channel1 = ADS.readADC(ADC_CHANNEL_1);
  int16_t raw_channel2 = ADS.readADC(ADC_CHANNEL_2);
  int16_t raw_channel3 = ADS.readADC(ADC_CHANNEL_3);

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

void EnterLowPowerMode() {
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
