////////////////////////////////////////////////////////////////////////////////
//writes data from gps to file
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
//for file read/write
#include <fstream>
#include <iostream>
//for usleep() delay
#include <unistd.h>
//serial library included with wiringPi 
#include <wiringSerial.h>
//for time functions
#include <sys/time.h>
#include <time.h>



int main()
{
    sleep(35);    

    int fd = serialOpen("/dev/ttyAMA0",9600);

    bool first = true;

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

    char serialChar;
    std::string serialString = "";

    while(1)
    {
	serialChar = serialGetchar(fd);
	serialString += serialChar;
	if (serialChar == '\n')
	{
	    //create log file for gps data
            std::ofstream gps;
            gps.open("/media/pi/VideoStore/" + path + "/gps.log", std::fstream::out | std::fstream::app);
            //write line to file
	    gps << serialString;
	    std::cout << serialString;
	    //close file and reset string
	    gps.close();
	    serialString = "";

	    if (first)
	    {
		//time vars
		time_t current_time;
		struct timeval time_info;
		char buffer[80];

		//get system time
    		gettimeofday(&time_info, NULL); 
    		current_time=time_info.tv_sec;
    		//convert time to string (up to seconds)
    		strftime(buffer,80,"%m-%d-%Y  %T.",localtime(&current_time));
    		std::string time_str(buffer);

		//create gps_time_ref file
    		std::ofstream time_file;
    		time_file.open("/media/pi/VideoStore/" + path + "/gps_time_ref.txt");
    		//print time to time_file (seconds + milliseconds)
    		time_file << buffer + std::to_string(time_info.tv_usec);
    		time_file.close();
		first = false;
	    }

	}

	usleep(100);
    }

    

    return 0;
}
