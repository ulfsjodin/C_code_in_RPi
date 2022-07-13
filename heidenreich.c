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
#include <time.h>

// gpio pins BCM numbering.
#define PIR 6
#define YELLOW_LED 18 

void cleanup(int signo) {
	digitalWrite(YELLOW_LED, LOW);
	pinMode(YELLOW_LED, INPUT);
	pullUpDnControl(PIR, PUD_DOWN);
		
	exit(0);
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


//PIR
void goPir() {
	digitalWrite(YELLOW_LED, HIGH);
	delay(100);
	digitalWrite(YELLOW_LED, LOW);
}


const char* filnamn() {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char contents[115];
	sprintf(contents, "%d-%02d-%02d_%02d:%02d:%02d.jpg\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	
	return(contents);

}


static pid_t pid = 0;

void takePic(char* filename) {

	if((pid = fork()) == 0) {
		execl("/usr/bin/raspistill",
			"/usr/bin/raspistill",
			"-n",
			"-w", "640",
			"-h", "480",
			"-o",
			filename,
			NULL);
	}

}

void shoot() {
	char filename[115];
	filename = filnamn();
	takePic(contents);
	
}

int main(void) {
	signal(SIGINT , cleanup); 
	signal(SIGTERM , cleanup); 
	signal(SIGHUP , cleanup); 

	wiringPiSetupGpio();

	pinMode(YELLOW_LED ,INPUT);
	pullUpDnControl(PIR, PUD_UP);
	pinMode(YELLOW_LED, OUTPUT);
	digitalWrite(YELLOW_LED, LOW);

	//wiringPiISR(PIR, INT_EDGE_FALLING, goPir);
	wiringPiISR(PIR, INT_EDGE_FALLING, shoot);

	pause();

	return 0;

}

