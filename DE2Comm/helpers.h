/*
 * helpers.h
 *
 *  Created on: 2015-03-24
 *      Author: Oliver
 */

#ifndef HELPERS_H_
#define HELPERS_H_

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

void checkButtons();
char checkInterrupt();
void initializeIMU();
void getAngle();
short readSliders(int channel);

extern short gyro_x_low, gyro_y_low, gyro_z_low, gyro_x_high, gyro_y_high, gyro_z_high;
extern short acc_x_high, acc_x_low, acc_y_high, acc_y_low, acc_z_high, acc_z_low;
extern float accXAngle, accYAngle, gyroXAngle, gyroYAngle, CFangleX, CFangleY, loopTime, rate_gyr_x, rate_gyr_y;
extern int adcTest;

#endif /* HELPERS_H_ */
