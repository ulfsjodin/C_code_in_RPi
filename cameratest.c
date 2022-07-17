#include <wiringPi.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

char *tidstampel(void) {
           time_t t = time(NULL);
           struct tm tm = *localtime(&t);
           static char contents[115];
           sprintf(contents, "/home/ulf/c_pins/wiringPi/heiden/pictures/%d-%02d_%02d_%02d_%02d_%02d.jpg\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,tm.tm_min, tm.tm_sec);
 
           return(contents);
  }
 


static pid_t pid = 0;

void takePic(char *filename) {
 
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

int main() {
	//takePic("/home/ulf/c_pins/wiringpi/heiden_c/pictures/pic4.jpg");
	
	char *contentan;
        contentan = tidstampel();
	takePic(contentan);
	printf("<<>>%s\n", contentan);
	
	return 0;
}

