#include <Arduino.h>
#include <Wire.h>

#define FLEX1_PIN 35
#define FLEX2_PIN 34

#define SDA_PIN 21
#define SCL_PIN 22

#define MPU_ADDR 0x68

void setup()
{
    Serial.begin(115200);

    // ADC setup
    pinMode(FLEX1_PIN, INPUT);
    pinMode(FLEX2_PIN, INPUT);

    // Start I2C
    Wire.begin(SDA_PIN, SCL_PIN);

    // MPU6050
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x6B);     
    Wire.write(0x00);       
    byte error = Wire.endTransmission();

    if (error == 0)
    {
        Serial.println("MPU6050 connected successfully!");
    }
    else
    {
        Serial.println("MPU6050 connection failed!");
    }

    delay(1000);
}

//  LOOP
void loop()
{
    // 1. READ FLEX SENSORS

    int flex1Value = analogRead(FLEX1_PIN);
    int flex2Value = analogRead(FLEX2_PIN);

    Serial.println();
    Serial.println("========== REHABILITATION GLOVE ==========");

    Serial.print("Flex Sensor 1: ");
    Serial.println(flex1Value);

    Serial.print("Flex Sensor 2: ");
    Serial.println(flex2Value);

    // 2. READ MPU6050

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);       // Starting register for accelerometer
    Wire.endTransmission(false);

    Wire.requestFrom((uint8_t)MPU_ADDR, (size_t)14, true);

    if (Wire.available() == 14)
    {
        // Accelerometer
        int16_t ax = Wire.read() << 8 | Wire.read();
        int16_t ay = Wire.read() << 8 | Wire.read();
        int16_t az = Wire.read() << 8 | Wire.read();

        // Skip temperature
        Wire.read();
        Wire.read();

        // Gyroscope
        int16_t gx = Wire.read() << 8 | Wire.read();
        int16_t gy = Wire.read() << 8 | Wire.read();
        int16_t gz = Wire.read() << 8 | Wire.read();

        Serial.println();

        Serial.println("---- MPU6050 ----");

        Serial.print("Accel X: ");
        Serial.println(ax);

        Serial.print("Accel Y: ");
        Serial.println(ay);

        Serial.print("Accel Z: ");
        Serial.println(az);

        Serial.print("Gyro X: ");
        Serial.println(gx);

        Serial.print("Gyro Y: ");
        Serial.println(gy);

        Serial.print("Gyro Z: ");
        Serial.println(gz);
    }
    else
    {
        Serial.println("MPU6050 data reading failed!");
    }

    Serial.println("==========================================");

    delay(500);
}







