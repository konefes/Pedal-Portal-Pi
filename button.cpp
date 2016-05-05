////////////////////////////////////////////////////////////////////////////////
//handles button press interrupt and writes the event to file
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
//for file read/write
#include <fstream>
#include <iostream>
//for time functions
#include <sys/time.h>
#include <time.h>
//for GPIO
#include <wiringPi.h>
//for usleep()
#include <unistd.h>

// Use GPIO Pin 17, which is Pin 0 for wiringPi library
#define BUTTON_PIN 0
//button state interrupt variable
volatile bool pressed = false;

// myInterrupt:  called every time an event occurs
void buttonPressed(void) 
{
    //usleep(100000);
    pressed = true;
}

int main()
{
    sleep(45);


    //initialize wiringPi library
    wiringPiSetup();
    //pin 17gpio/0wiringPi set to input
    pinMode(BUTTON_PIN, INPUT);
    // Enable pull-up resistor on button
    pullUpDnControl(BUTTON_PIN, PUD_UP); 
    //initialize interrupt to call buttonPressed method
    wiringPiISR (BUTTON_PIN, INT_EDGE_FALLING, &buttonPressed);
    
    //get path from ready_file
    std::string path = "";
    //check that path contains info
    while (path == "")
    {
        //delay 500ms
        usleep(500000);
        //check if master is ready to be written to
        std::ifstream ready_file;
        ready_file.open("/media/pi/VideoStore/ready_file.txt");
        while(!ready_file.is_open())
        {
            std::cout << "file open error" << std::endl;
            usleep(500000);
            ready_file.close();
            ready_file.open("/media/pi/VideoStore/ready_file.txt");
        }
        //read directory info from ready_file.txt
        getline(ready_file,path);
        ready_file.close();
    }
    
    
    //time vars
    time_t current_time;
    struct timeval time_info;
    char buffer[80];
    
    while(1)
    {
        if (pressed)
        {
            //create text file for time corresponding to video writes
            std::ofstream events;
            events.open("/media/pi/VideoStore/" + path + "/events.txt", std::fstream::out | std::fstream::app);
            
            //get time from system
            gettimeofday(&time_info, NULL); 
            current_time=time_info.tv_sec;
            //convert time to string (up to seconds)
            strftime(buffer,80,"%m-%d-%Y  %T.",localtime(&current_time));
            std::string time_str(buffer);
            
            //write time to timecode file with appended milliseconds
	    std::cout << time_str << std::endl;
            events << time_str + std::to_string(time_info.tv_usec) + "\n";
            events.close();
            
            //reset button press variable
            pressed = false;
        }
        //delay 100ms to relieve cpu
        usleep(100000);
    }
    
    return 0;
}


