#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "system.h"
#include "io.h"
#include "i2c_opencores.h"
#include <Math.h>
#include "sys/alt_timestamp.h"
#include <sys/time.h>
#include <time.h>
#include <Windows.h>
#include "helpers.h"

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

void send(char);

typedef enum { false, true } bool;

bool positiveTiltX = false, negativeTiltX = false, positiveTiltY = false, negativeTiltY = false, tilted = false;
bool singleTap = false, doubleTap = false;

char int_source;
short firstChannel, prevFirstChannel, secondChannel, prevSecondChannel;
float CFangleX, CFangleY, loopTime;

int main() {


	printf("Program started\n");
	int start, end;

	initializeIMU();
	prevFirstChannel = readSliders(1);
	prevSecondChannel = readSliders(2);
	while(1){

		start = alt_timestamp_start();

		checkButtons();

		firstChannel = readSliders(1);
		secondChannel = readSliders(2);
//		printf("First slider:  %d     Second slider:  %d\n", firstChannel, secondChannel);

		if(firstChannel != prevFirstChannel) {
			send('A');
			send(firstChannel);
//			printf("Value changed. Old:   %d    New:   %d\n", prevFirstChannel, firstChannel);
		} else if (secondChannel != prevSecondChannel) {
			send('B');
			send(secondChannel);
//			printf("Value changed. Old:   %d    New:   %d\n", prevSecondChannel, secondChannel);
		}

		prevFirstChannel = firstChannel;
		prevSecondChannel = secondChannel;
		getAngle();

//		int_source = checkInterrupt();
//
//		if(((int_source & 0x20) == 0x20) && !tilted){
//			printf("Double tap detected\n");
//			send('Y');
//		} else if(((int_source & 0x40) == 0x40) && !tilted) {
//			printf("Single tap detected\n");
//			send('Z');
//		}

		if((CFangleX > 45) && !positiveTiltX && !tilted){
			printf("Went over 45 in X\n");
			positiveTiltX = true;
			tilted = true;
		} else if((CFangleX < 30) && positiveTiltX && tilted) {
			printf("Back under 30 in X\n");
			send('C');
			positiveTiltX = false;
			tilted = false;
		} else if((CFangleX < -45) && !negativeTiltX && !tilted){
			printf("Went under -45 in X\n");
			negativeTiltX = true;
			tilted = true;
		} else if((CFangleX > -30) && negativeTiltX && tilted) {
			printf("Back over -30 in X\n");
//			send('D');
			negativeTiltX = false;
			tilted = false;
		}
//
//		if((CFangleY > 45) && !positiveTiltY && !tilted){
//			printf("Went over 45 in Y\n");
//			positiveTiltY = true;
//			tilted = true;
//		} else if((CFangleY < 40) && positiveTiltY && tilted) {
//			printf("Back under 40 in Y\n");
//			send('G');
//			CFangleY = 0;
//			positiveTiltY = false;
//			tilted = false;
//		} else if((CFangleY < -45) && !negativeTiltY && !tilted){
//			printf("Went under -45 in Y\n");
//			negativeTiltY = true;
//			tilted = true;
//		} else if((CFangleY > -40) && negativeTiltY && tilted) {
//			printf("Back over -40 in Y\n");
//			send('H');
//			CFangleY = 0;
//			negativeTiltY = false;
//			tilted = false;
//		}


		do {
		end = alt_timestamp();
		loopTime = (float)(end-start)/(float)alt_timestamp_freq();
		} while(loopTime < DT);
	}

  return 0;
}

void send(char toSend){

	IOWR(DE2_FLAG, 0, 0x00);
	IOWR(BUFFER_OUT, 0, toSend);
	IOWR(DE2_FLAG, 0, 0x01);
	while(IORD(PI_FLAG, 0) == 0x00);
	IOWR(DE2_FLAG, 0, 0x00);
	while(IORD(PI_FLAG, 0) == 0x01);
}

//	*DE2_FLAG = 0;
//	*BUFFER_OUT = toSend;
//	*DE2_FLAG = 1;
//	while(PI_FLAG == 0x0);
//	*DE2_FLAG = 0;
//	while(PI_FLAG == 0x1);


//
//char receive(){
//	PI_FLAG = 0;
//
//	while(DE2_FLAG = 0);
//	Received = BUFFER_IN;
//	PI_FLAG = 1;
//	while(DE2_FLAG = 1);
//	PI_FLAG = 0;
//
//}



