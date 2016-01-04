/*
 * helpers.c
 *
 *  Created on: 2015-03-24
 *      Author: Oliver
 */

#include "helpers.h"
#include "system.h"
#include "io.h"

#define I2C_BASE I2C_OPENCORES_0_BASE
#define BUTTONS PUSH_BUTTONS_BASE
#define ADC ADC_0_BASE
#define GYROSCOPE_ADDR 0x68
#define ACCELEROMETER_ADDR 0x53

#define DE2_FLAG WRITE_FLAG_BASE
#define PI_FLAG (volatile char *) READ_FLAG_BASE
#define BUFFER_IN (volatile char *) 0x2020
#define BUFFER_OUT (volatile char *) BUFFER_OUT_BASE

#define THRESH_TAP 0x80                                 //Tap threshold value
#define DUR 0x30                                        //Tap duration value
#define LATENT 0x40                                     //Tap Latency value
#define WINDOW 0xFF                                     //Tap window value
#define TAP_AXES 0x01                             		//Axis control for single tap/double tap value
#define INT_ENABLE 0x60		                            //Interrupt enable control value
#define INT_MAP B00100000                               //Interrupt mapping control value

#define ACCEL_X_OFFSET 3.2
#define ACCEL_Y_OFFSET 3

#define M_PI 3.14159265358979323846						//Mathematical constant pi used in calculations
#define RAD_TO_DEG 57.29578								//Constant used to convert angle from Radians to Degrees
#define G_GAIN 0.07										//Gyroscope sensitivity scale factor in units of degrees per second
#define DT 0.02

typedef enum { false, true } bool;

bool pressed = false, pressed2 = false, pressed3 = false;

short gyro_x_low, gyro_y_low, gyro_z_low, gyro_x_high, gyro_y_high, gyro_z_high;
short acc_x_high, acc_x_low, acc_y_high, acc_y_low, acc_z_high, acc_z_low;
float accXAngle, accYAngle, gyroXAngle, gyroYAngle, CFangleX, CFangleY, loopTime, rate_gyr_x, rate_gyr_y;
int adcTest;

void checkButtons()
{
	if(((IORD(BUTTONS, 0) & 0x02) == 0) && !pressed) {
		printf("Button 2 was pressed\n");
				send('A');
		pressed = true;
	}

	else if(((IORD(BUTTONS,0) & 0x02) == 0x02) && pressed ){
		printf("Button 2 was released\n");
		pressed = false;
	}

	else if(((IORD(BUTTONS, 0) & 0x04) == 0) && !pressed2) {
		printf("Button 3 was pressed\n");
				send('B');
		pressed2 = true;
	}
	else if(((IORD(BUTTONS,0) & 0x04) == 0x04) && pressed2){
		printf("Button 3 was released\n");
		pressed2 = false;
	}
	else if(((IORD(BUTTONS, 0) & 0x08) == 0) && !pressed3) {
		printf("Button 4 was pressed\n");
				send('\n');
		pressed3 = true;
	}
	else if(((IORD(BUTTONS,0) & 0x08) == 0x08) && pressed3){
		printf("Button 4 was released\n");
		pressed3 = false;
	}
}

void initializeIMU(){
	I2C_init(I2C_BASE,50000000,100000);

	I2C_start(I2C_BASE, GYROSCOPE_ADDR, 0);				//Writes to data format register of gyroscope
	I2C_write(I2C_BASE, 0x15, 0);						//to configure the sample rate of the gyroscope.
	I2C_write(I2C_BASE, 0x00, 1);

	I2C_start(I2C_BASE, GYROSCOPE_ADDR, 0);				//Writes to operation register to configure the gyroscope
	I2C_write(I2C_BASE, 0x16, 0);						//for normal operation. Also selects a low pass filter
	I2C_write(I2C_BASE, 0x18, 1);						//of 256HZ to reduce noise in gyroscope measurements.

	I2C_start(I2C_BASE, GYROSCOPE_ADDR, 0);				//Writes to power management register of gyroscope
	I2C_write(I2C_BASE, 0x3E, 0);						//to disable sleep mode and enable measurements in all axes.
	I2C_write(I2C_BASE, 0x01, 1);

	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 0);			//Writes to data format register of accelerometer
	I2C_write(I2C_BASE, 0x31, 0);						//to configure the range and presentation of
	I2C_write(I2C_BASE, 0x08, 1);						//accelerometer measurements.

	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 0);			//Writes to tap threshhold register to configure the accelerometer
	I2C_write(I2C_BASE, 0x1D, 0);						//for tap detection.
	I2C_write(I2C_BASE, THRESH_TAP, 1);

	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 0);			//Writes to tap duration register to configure the gyroscope
	I2C_write(I2C_BASE, 0x21, 0);						//for tap detection.
	I2C_write(I2C_BASE, DUR, 1);

	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 0);			//Writes to tap latency register to configure the gyroscope
	I2C_write(I2C_BASE, 0x22, 0);						//for tap detection.
	I2C_write(I2C_BASE, LATENT, 1);

	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 0);			//Writes to tap window register to configure the gyroscope
	I2C_write(I2C_BASE, 0x23, 0);						//for tap detection.
	I2C_write(I2C_BASE, WINDOW, 1);

	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 0);			//Writes to tap duration register to configure the gyroscope
	I2C_write(I2C_BASE, 0x2A, 0);						//for tap detection.
	I2C_write(I2C_BASE, TAP_AXES, 1);

	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 0);			//Writes to tap duration register to configure the gyroscope
	I2C_write(I2C_BASE, 0x2E, 0);						//for tap detection.
	I2C_write(I2C_BASE, INT_ENABLE, 1);

	//Accel. Ctrl 2 settings
	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 0);			//Writes to power management register of accelerometer
	I2C_write(I2C_BASE, 0x2D, 0);						//to disable sleep mode and enable measurements in all axes.
	I2C_write(I2C_BASE, 0x08, 1);
	//Accel. Ctrl 7 settings
	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 0);			//Writes to operation register of accelerometer
	I2C_write(I2C_BASE, 0x2C, 0);						//to configure the device bandwidth and output data rate
	I2C_write(I2C_BASE, 0x0D, 1);						//of accelerometer measurements.
}

void getAngle(){
	/* Reads gyroscope measurements by issuing command to read
	 * from gyroscope register 0x1D. This register value will
	 * automatically increment until a command is received to
	 * end the read. Gyroscope measurements are expressed as
	 * 16 bit signed integers and have 3 degrees of freedom
	 * along the X, Y, and Z axes. Each 16 bit value is stored
	 * in two registers thus a total of 6 reads are required
	 * to receive data from all six registers(0x1D to 0x22).
	 */

	I2C_start(I2C_BASE, GYROSCOPE_ADDR, 0);
	I2C_write(I2C_BASE, 0x1D, 0);
	I2C_start(I2C_BASE, GYROSCOPE_ADDR, 1);

	gyro_x_high = I2C_read(I2C_BASE, 0);			//Reads and stores the most significant byte for the X axis
	gyro_x_low = I2C_read(I2C_BASE, 0);				//Reads and stores the least significant byte for the X axis
	gyro_y_high = I2C_read(I2C_BASE, 0);			//Reads and stores the most significant byte for the Y axis
	gyro_y_low = I2C_read(I2C_BASE, 0);				//Reads and stores the least significant byte for the Y axis
	gyro_z_high = I2C_read(I2C_BASE, 0);			//Reads and stores the most significant byte for the Z axis
	gyro_z_low = I2C_read(I2C_BASE, 1);				//Reads and stores the least significant byte for the Z axis

	/* Reads accelerometer measurements by issuing command to read
	 * from accelerometer register 0x32. This register value will
	 * automatically increment until a command is received to
	 * end the read. Accelerometer measurements are expressed as
	 * 16 bit signed integers and have 3 degrees of freedom
	 * along the X, Y, and Z axes. Each 16 bit value is stored
	 * in two registers thus a total of 6 reads are required
	 * to receive data from all six registers(0x32 to 0x37).
	 */
	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 0);
	I2C_write(I2C_BASE, 0x32, 0);
	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 1);

	acc_x_low = I2C_read(I2C_BASE, 0);				//Reads and stores the least significant byte for the X axis
	acc_x_high = I2C_read(I2C_BASE, 0);				//Reads and stores the most significant byte for the X axis
	acc_y_low = I2C_read(I2C_BASE, 0);				//Reads and stores the least significant byte for the Y axis
	acc_y_high = I2C_read(I2C_BASE, 0);				//Reads and stores the most significant byte for the Y axis
	acc_z_low = I2C_read(I2C_BASE, 0);				//Reads and stores the least significant byte for the Z axis
	acc_z_high = I2C_read(I2C_BASE, 1);				//Reads and stores the most significant byte for the Z axis

	/* Performs bit shift operation to combine higher and lower
	 * bytes of X, Y, and Z angles into a single variable.
	 * The MSB is shifted 8 bits to the left and the LSB
	 * is moved into the lower 8 bits.
	 */
	gyro_x_high <<= 8;
	gyro_x_high |= gyro_x_low;
//	gyro_x_high -= GYRO_X_OFFSET;

	gyro_y_high <<= 8;
	gyro_y_high |= gyro_y_low;
//	gyro_y_high += GYRO_Y_OFFSET;

	gyro_z_high <<= 8;
	gyro_z_high |= gyro_z_low;
//	gyro_z_high += GYRO_Z_OFFSET;

	acc_x_high <<= 8;
	acc_x_high |= acc_x_low;
//	acc_x_high += ACCEL_X_OFFSET;

	acc_y_high <<= 8;
	acc_y_high |= acc_y_low;
//	acc_y_high -= ACCEL_Y_OFFSET;

	acc_z_high <<= 8;
	acc_z_high |= acc_z_low;
//	acc_z_high += ACCEL_Z_OFFSET;

//		printf("Acc X:%d   Y:%d    Z:%d   || Gyro X:%d   Y:%d   Z:%d\n",
//				acc_x_high, acc_y_high, acc_z_high, gyro_x_high, gyro_y_high, gyro_z_high);

	/* Calculates the current angle as measured by the accelerometer
	 * and converts the angle from radians to degrees.
	 * This is done using trigonometry, the atan function, to get the
	 * principal value of the tangent of Y and Z. This value is then
	 * converted to degrees to simplify further calculations.
	 */

	accXAngle = (float) (atan2(acc_y_high, acc_z_high)+M_PI)*RAD_TO_DEG;
	accYAngle = (float) (atan2(acc_x_high, acc_z_high)+M_PI)*RAD_TO_DEG;

	accXAngle -= ACCEL_X_OFFSET;
	accYAngle += ACCEL_Y_OFFSET;

	//Change the rotation value of the accelerometer to -/+ 180
	if (accXAngle > 180)
	{
		accXAngle -= (float)360.0;
	}

	if (accYAngle > 180)
	{
		accYAngle -= (float)360.0;
	}

	/* Calculates the robot's current rotation in degrees per second
	 * This is done by multiplying the raw data acquired from the
	 * X axis of the gyroscope by the sensitivity scale factor
	 */
	rate_gyr_x = (float) gyro_x_high*G_GAIN;
	rate_gyr_y = (float) gyro_y_high*G_GAIN;

    /* Calculate the robot's current angle as measured by the gyroscope.
     * This is done by multiplying the loop period by the rotation rate
     * in the X axis.
     */
	gyroXAngle = rate_gyr_x*DT;
	gyroYAngle = rate_gyr_y*DT;

	/* Combines the two angular measurements by the accelerometer
	 * and the gyroscope into a single value to reduce the effects
	 * of drift on the gyroscope and noise in the accelerometer.
	 * A complementary filter is used which trusts the gyroscope
	 * for short periods of time and the accelerometer for longer
	 * periods of time.
	 */
	CFangleX = 0.98*(CFangleX+gyroXAngle)+(0.02)*accXAngle;
	CFangleY = 0.98*(CFangleY+gyroYAngle)+(0.02)*accYAngle;

	//printf("The angle is: %f        %f       %f\n", CFangleY, gyroYAngle, accYAngle);
}

char checkInterrupt(){
	char int_values;
	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 0);
	I2C_write(I2C_BASE, 0x30, 0);
	I2C_start(I2C_BASE, ACCELEROMETER_ADDR, 1);
	int_values = I2C_read(I2C_BASE, 1);
	return int_values;
}

short readSliders(int channel){
	short adcValues = IORD_32DIRECT(ADC, 0);
	short firstChannel = (adcValues & 0x00FF);
	short secondChannel = (adcValues &0x00FF00) >> 8;
	short error = 0;
	if( channel == 1 ){
		return (firstChannel/25)*10;
	} else if( channel == 2 ) {
		return (secondChannel/25)*10;
	}
	return error;
}
