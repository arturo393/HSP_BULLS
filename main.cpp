
#include "mbed.h"
#include "MAX14720.h"
#include "MAX30205.h"
#include "LIS2DH.h"

/// define the HVOUT Boost Voltage default for the MAX14720 PMIC
#define HVOUT_VOLTAGE 4500 // set to 4500 mV

/// define all I2C addresses
#define MAX30205_I2C_SLAVE_ADDR_TOP (0x92)
#define MAX30205_I2C_SLAVE_ADDR_BOTTOM (0x90)
#define MAX14720_I2C_SLAVE_ADDR (0x54)
#define BMP280_I2C_SLAVE_ADDR (0xEC)
#define MAX30101_I2C_SLAVE_ADDR (0xAE)
#define LIS2DH_I2C_SLAVE_ADDR (0x32)

///
/// wire Interfaces
///
/// Define with Maxim VID and a Maxim assigned PID, set to version 0x0001 and non-blocking
//USBSerial usbSerial(0x0b6a, 0x0100, 0x0001, false);
/// I2C Master 1
I2C i2c1(I2C1_SDA, I2C1_SCL); // used by MAX30205 (1), MAX30205 (2), BMP280
/// I2C Master 2
I2C i2c2(I2C2_SDA, I2C2_SCL); // used by MAX14720, MAX30101, LIS2DH
/// SPI Master 0 with SPI0_SS for use with MAX30001
SPI spi(SPI0_MOSI, SPI0_MISO, SPI0_SCK, SPI0_SS); // used by MAX30001

/// SPI Master 1
//QuadSpiInterface quadSpiInterface(SPI1_MOSI, SPI1_MISO, SPI1_SCK,
//                                  SPI1_SS); // used by S25FS512

///
/// Devices
///
/// Pressure Sensor
//BMP280 bmp280(&i2c1, BMP280_I2C_SLAVE_ADDR);
/// Top Local Temperature Sensor
MAX30205 MAX30205_top(i2c1, MAX30205_I2C_SLAVE_ADDR_TOP);
/// Bottom Local Temperature Sensor
MAX30205 MAX30205_bottom(i2c1, MAX30205_I2C_SLAVE_ADDR_BOTTOM);
// Accelerometer
LIS2DH lis2dh(&i2c2, LIS2DH_I2C_SLAVE_ADDR);

InterruptIn lis2dh_Interrupt(P4_7);

DigitalOut led(LED1);

//Serial
Serial pc(USBTX, USBRX);

int main()
{
    // Assign turnOff function to falling edge of button
    //button.fall(&turnOff);

    // Wait 1 second to see the buck-boost regulator turn on
    char buff[20];
    int32_t ret;
    uint16_t rawTemp_top;
    uint16_t rawTemp_bottom;
    float celsius_top, celsius_bottom;
    uint32_t expandTemperatureValue = 0;

    int16_t acc_x, acc_y, acc_z;

    lis2dh.init();

    if (lis2dh.detect(buff) == -1)
        pc.printf("LIS2DH not detected\n\r");
    else
        pc.printf("LIS2DH: %s\n\r", buff);

    while (1)
    {
        // read temperature values
        MAX30205_top.readTemperature(rawTemp_top);
        //convert to celcius
        celsius_top = MAX30205_top.toCelsius(rawTemp_top);
        pc.printf("Top Temperature: %f\n\r", celsius_top);

        if (!MAX30205_bottom.readTemperature(rawTemp_bottom))
        {
            celsius_bottom = MAX30205_top.toCelsius(rawTemp_bottom);
            pc.printf("Bottom Temperature: %d\n\r", celsius_bottom);
        }
        else
            pc.printf("Bottom Temperature: Read data failure\n\r");

        if (lis2dh.detect(buff) == -1)
            pc.printf("LIS2DH not detected\n\r");
        else
            pc.printf("LIS2DH: %s\n\r", buff);

        // read accelerometer values
        lis2dh.get_motion_fifo(&acc_x, &acc_y, &acc_z);
        pc.printf("Acc X:%d\tAcc Y:%d\tAcc:%d\t\n\r", acc_x, acc_y, acc_z);
        wait(5);
        led = 1; // set LED1 pin to high
        wait(2);

        led = 0; // set LED1 pin to low
    }
}
