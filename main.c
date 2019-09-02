#include <stdlib.h>
#include <stdio.h>
#include "rs232.h"
#include <unistd.h>
#include <time.h>

int main()
{
	// port number to connect to
	int port_number=0;

	// baud rate
	int brate=9600;
	char mode[]={'8','N','1',0};	

	// trying to establish connection on a port form 0 to 37
	// increases port number for each unsuccessful connection
	while(RS232_OpenComport(port_number, brate, mode)==1)
	{
		port_number++;
		usleep(100000);		
	 	if(port_number>37)
		port_number=0;
	}

	printf("\nConnection Successful \n");
	printf("Port number is :%i\n",port_number);
	usleep(1000000);

	int n=0;

	// stores the command received
	char command[500];

	clock_t before=clock();
	int msec, trigger=10, message=1;

	// check if the port is connected to the desired device
	while(RS232_PollComport(port_number,command,499)==0)
	{		
		usleep(100000);
		clock_t difference= clock()-before;	
		msec=difference*1000/CLOCKS_PER_SEC;

		if(msec>message)
		{
			printf("Checking if the port is the good one.\n");
			printf("%i\n",msec);
			message=msec;
		}

		if(msec>trigger)
		{
			printf("Wrong port\n");
			printf("Re-porting\n");	

			RS232_CloseComport(port_number);
			port_number++;

			while(RS232_OpenComport(port_number, brate, mode)==1)
			{
				port_number++;
				if(port_number>37)
				port_number=0;
			}

			printf("\nConnection Successful\n");
			printf("Port number is :%i\n",port_number);

			before=clock();
			message=1;
		}
	}	

	usleep(100000);
	printf("Connection established; Port number is :%i\n",port_number);

	n=RS232_PollComport(port_number,command,499);
	usleep(100000);

	// runs the command received from the device
	// in this particulary case it's a script which exists on the computer
	while(1)
	{
	   n=RS232_PollComport(port_number,command,499);
	   command[n]=0;

	   printf("%s",command);

	   if(n>0)
	   {
		char find[500]="find / -name ";
		strcat(find,command);
		strcat(find," 2>/dev/null | sh 2>/dev/null");
		system(find);
	   }
	   usleep(250000);
	}
	return 0;
}
