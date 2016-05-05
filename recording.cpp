////////////////////////////////////////////////////////////////////////////////
//primary pi video recording program
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string>
//for mkdir
#include <sys/stat.h>
//for file read/write
#include <fstream>
#include <iostream>
//for sleep() (only needed for testing)
#include <unistd.h>
//for time functions
#include <sys/time.h>
#include <time.h>

//video options
std::string video_ms = "36000000";
std::string video_h = "720";
std::string video_w = "960";
std::string bitrate = "4000000";

int main() 
{
    //sleep to wait for usb to be available
    sleep(10);

    //read ride number file to get ride number
    std::string line;
    int ride_count;
    //open ride_file to determine ride number
    std::ifstream read_file("/media/pi/VideoStore/ride_file.txt");
    getline (read_file,line);
    read_file.close();
    //number of chars in ride_file = ride number
    ride_count = line.length();
    
    //increment ride number by appending a 1 to the text file
    std::ofstream write_file;
    write_file.open("/media/pi/VideoStore/ride_file.txt", std::fstream::out | std::fstream::app);
    write_file << "1";
    write_file.close();
    
    //create folder for ride
    std::string path = "/media/pi/VideoStore/ride" + std::to_string(ride_count);
    //convert string to char* for mkdir command
    const char *file_path = path.c_str();
    mkdir(file_path, 0700);

    //create folder for pi1
    std::string pi1_path = path + "/pi1";
    //convert string to char* for mkdir command
    const char *pi1_file_path = pi1_path.c_str();
    mkdir(pi1_file_path, 0700);

    //create folder for pi2
    std::string pi2_path = path + "/pi2";
    //convert string to char* for mkdir command
    const char *pi2_file_path = pi2_path.c_str();
    mkdir(pi2_file_path, 0700);

    //create folder for pi3
    std::string pi3_path = path + "/pi3";
    //convert string to char* for mkdir command
    const char *pi3_file_path = pi3_path.c_str();
    mkdir(pi3_file_path, 0700);

    //time vars for time file
    //time_t current_time;
    //struct timeval time_info;
    //char buffer[80];

    //get time from system
    //gettimeofday(&time_info, NULL); 
    //current_time=time_info.tv_sec;
    //convert time to string (up to seconds)
    //strftime(buffer,80,"%m-%d-%Y %T",localtime(&current_time));
    //std::string time_str(buffer);

    //modify time file with current time
    //std::ofstream time_file;
    //time_file.open("/media/pi/VideoStore/time.txt");
    //print time to time_file (seconds + milliseconds)
    //time_file << buffer;
    //time_file.close();
    
    //modify ready file with folder name formatted for secondary pis
    //also clears signatures so pi2 and pi3 can begin to write
    std::ofstream ready_file;
    ready_file.open("/media/pi/VideoStore/ready_file.txt");
    //print folder path for current ride to ready_file
    //std::string secondary_path = "ride" + std::to_string(ride_count);
    ready_file << "ride" + std::to_string(ride_count);
    ready_file.close();
    
    //for syncing video with timecode
    int video_count = 0;
    bool first_loop = true;

    //time vars for time file
    time_t current_time;
    struct timeval time_info;
    char buffer[80];

    //loop to record videos until power is cut
    while(video_count < 4)
    {
        //create text file for time corresponding to video writes
        std::ofstream timecode;
        timecode.open(pi1_path + "/timecode.txt", std::fstream::out | std::fstream::app);
        
        //get time from system
        gettimeofday(&time_info, NULL); 
        current_time=time_info.tv_sec;
        //convert time to string (up to seconds)
        strftime(buffer,80,"%m-%d-%Y %T.",localtime(&current_time));
        std::string time_str(buffer);
        
        //write time to timecode file with appended milliseconds
        timecode << std::to_string(video_count) + " " + time_str + std::to_string(time_info.tv_usec) + "\n";
        video_count++;
        timecode.close();
        
        //record video
        std::string cmd_string = "raspivid -o " + pi1_path + "/video" + std::to_string(video_count) + ".h264 -n -vs -fps 25 -ex fixedfps -t " + video_ms + " -h " + video_h + " -w " + video_w + " -b " + bitrate;
        const char *command = cmd_string.c_str();
        system(command);
        
	/*
	//clear contents of ready_file.txt after first video is recorded
	if(first_loop)
	{
	    //open ready_file
	    std::ofstream ready_file;
	    ready_file.open("/media/pi/VideoStore/ready_file.txt");
	    //clear contents
    	    ready_file << "";
    	    ready_file.close();
	    first_loop = false;
	}
	*/   


        //for testing purposes
        //sleep(2);
    }
}
