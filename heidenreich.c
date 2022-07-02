/*
 * Ett seriöst försök nu då att få till det nån gång.
 * Branch pir
 */

#include <wiringPi.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

// gpio pins BCM numbering.
#define RED_BUTTON 16
#define GREEN_BUTTON 20
#define BLUE_BUTTON 21
#define PIR 6
#define RED_LED 13
#define GREEN_LED 19
#define BLUE_LED 26
#define YELLOW_LED 18 

void cleanup(int signo) {
	digitalWrite(RED_LED, LOW);
	digitalWrite(GREEN_LED, LOW);
	digitalWrite(BLUE_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);
	pinMode(RED_LED, INPUT);
	pinMode(GREEN_LED, INPUT);
	pinMode(BLUE_LED, INPUT);
	pinMode(YELLOW_LED, INPUT);
	pullUpDnControl(RED_BUTTON, PUD_DOWN);
	pullUpDnControl(GREEN_BUTTON, PUD_DOWN);
	pullUpDnControl(BLUE_BUTTON, PUD_DOWN);
	pullUpDnControl(PIR, PUD_DOWN);
		
	exit(0);
}

void goRed() {
	for(;;) {
		digitalWrite(RED_LED, !digitalRead(RED_BUTTON));
		delay(50);
	}
}

// Debounce for buttons.
unsigned short int isPressed(unsigned short int button) {
	static struct timespec lastCall;
	struct timespec thisCall;
	float timeDiff;
	
	clock_gettime(CLOCK_REALTIME, &thisCall);
	timeDiff = (thisCall.tv_sec + thisCall.tv_nsec/1E9 - lastCall.tv_sec - lastCall.tv_nsec/1E9)*5;
	lastCall = thisCall;

	return timeDiff > 1 ? 1 : 0;
}

void goGreen() {
	if(isPressed(GREEN_BUTTON)) {
		digitalWrite(GREEN_LED, !digitalRead(GREEN_BUTTON));
			}
}

void blink(unsigned short int led, unsigned short int intervall){
	for(;;) {
	digitalWrite(led, HIGH);
	delay(intervall);
	digitalWrite(led, LOW);
	delay(intervall);
	}	
}

void *callback(void *args) {
	blink(BLUE_LED, 250);
	return 0;
	}

void goBlue() {
	static pthread_t threadID;
	if(isPressed(BLUE_BUTTON)) {
		if(threadID) {
			pthread_cancel(threadID);
			threadID = 0;
			digitalWrite(BLUE_LED, LOW);
		}
		
		else {
		pthread_create(&threadID, NULL, callback, NULL);
		}
	}

}

//PIR
void goPir() {
	digitalWrite(YELLOW_LED, HIGH);
	delay(100);
	digitalWrite(YELLOW_LED, LOW);
}

int main(void) {
	signal(SIGINT , cleanup); 
	signal(SIGTERM , cleanup); 
	signal(SIGHUP , cleanup); 

	wiringPiSetupGpio();

	pinMode(RED_LED ,INPUT);
	pinMode(GREEN_LED ,INPUT);
	pinMode(BLUE_LED ,INPUT);
	pinMode(YELLOW_LED ,INPUT);
	pullUpDnControl(RED_BUTTON, PUD_UP);
	pullUpDnControl(GREEN_BUTTON, PUD_UP);
	pullUpDnControl(BLUE_BUTTON, PUD_UP);
	pullUpDnControl(PIR, PUD_UP);
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	digitalWrite(RED_LED, LOW);
	digitalWrite(GREEN_LED, LOW);
	digitalWrite(BLUE_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);

	wiringPiISR(RED_BUTTON, INT_EDGE_BOTH, goRed);
	wiringPiISR(GREEN_BUTTON, INT_EDGE_FALLING, goGreen);
	wiringPiISR(BLUE_BUTTON, INT_EDGE_FALLING, goBlue);
	wiringPiISR(PIR, INT_EDGE_FALLING, goPir);

	pause();

	return 0;

}

