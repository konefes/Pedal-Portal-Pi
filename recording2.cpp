////////////////////////////////////////////////////////////////////////////////
//secondary pi video recording program (for pi2)
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string>
//for file read/write
#include <fstream>
#include <iostream>
//for sleep()
#include <unistd.h>
//for time functions
#include <sys/time.h>
#include <time.h>

//video options
std::string video_ms = "36000000";
std::string video_h = "600";
std::string video_w = "800";
std::string bitrate = "2500000";

int main()
{
    sleep(30);

    std::string ready_path = "";
    bool ready = false;
    
    while(!ready)
    {
        usleep(500);
        //check if master is ready to be written to
        std::ifstream ready_file;
        ready_file.open("/media/networkshare/ready_file.txt");
	//loop until network drive is available and ready_file is found
        while(!ready_file.is_open())
        {
            std::cout << "file open error" << std::endl;
            usleep(500);
            ready_file.close();
	    //attempt to mount network drive
	    system("sudo mount -a");
	    //attempt to reopen
            ready_file.open("/media/networkshare/ready_file.txt");
        }
        
        //read directory info from ready_file.txt
        getline(ready_file, ready_path);

	//ensure that correct path was read
	ready = true;
	//check for end of file
	while(!ready_file.eof())
	{
	    //read each line of file, checking for pi2 signature
	    std::string line;
	    getline(ready_file, line);
	    std::cout << line << std::endl;
	    //if signature found, not ready to start recording
	    if (line == "pi2")
	    {
		ready = false;
		std::cout << "match" << std::endl;
		usleep(100);
	    }
	}
        ready_file.close();
        std::cout << ready_path << std::endl;
    }
    
    //sign ready_file
    std::ofstream ready_file;
    ready_file.open("/media/networkshare/ready_file.txt", std::fstream::out | std::fstream::app);
    ready_file << "\npi2";
    ready_file.close();

    //update time from time file
    //std::ifstream time_file;
    //time_file.open("/media/networkshare/time.txt");
    //std::string update_time;
    //getline(time_file, update_time);
    //format string for linux date system command
    //update_time = "sudo date -s \"" + update_time + "\"";
    //system(update_time.c_str());

    //append directory for this pi to the path
    std::string path = "/media/networkshare/" + ready_path;
    path += "/pi2";
    std::cout << path <<std::endl;    

    //time vars
    time_t current_time;
    struct timeval time_info;
    char buffer[80];
    
    //for syncing video with timecode
    int video_count = 0;
    
    while(video_count < 5)
    {
        //create text file for time corresponding to video writes
        std::ofstream timecode;
        timecode.open(path + "/timecode.txt", std::fstream::out | std::fstream::app);
	//std::cout << timecode.is_open() << std::endl;        

        //get time from system
        gettimeofday(&time_info, NULL); 
        current_time=time_info.tv_sec;
        //convert time to string (up to seconds)
        strftime(buffer,80,"%m-%d-%Y  %T.",localtime(&current_time));
        std::string time_str(buffer);
        
        //write time to timecode file with appended milliseconds
        timecode << std::to_string(video_count) + " " + time_str + std::to_string(time_info.tv_usec) + "\n";
        video_count++;
        timecode.close();
    
        //record video
        std::string cmd_string = "raspivid -o " + path + "/video" + std::to_string(video_count) + ".h264 -n -vs -fps 25 -ex fixedfps -t " + video_ms + " -h " + video_h + " -w " + video_w + " -b " + bitrate;
        const char *command = cmd_string.c_str();
        system(command);
        
        
        //for testing purposes
        //sleep(2);
    }
}
