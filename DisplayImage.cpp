#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>
#include <stdio.h>
#include <fstream>
#include <sys/resource.h>
#include <string>

using namespace std;
using namespace cv;
using namespace chrono;


// Global variable

int posCaptureButtonX = 0, posCaptureButtonY = 0; // Declare size of window, button, header
int posRecordButtonX = 0, posRecordButtonY = 75;
int widthButton = 200, heightButton = 50;
int widthWindow = 800, heightWindow = 800;
int posTimeHeaderX = 600, posTimeHeaderY = 0;
int widthTimeHeader = 100, heightTimeHeader = 50;
bool isClicked = false; // Declare state varibale
bool isCapture = false;
bool isRecordVideo = false;
bool isStopingRecordVideo = false;
bool isSetStartTimeRecordVideo = false;
bool isSetTotalTimeRecordVideo = false;
auto startRecordVideoTime = high_resolution_clock::now();
auto stopRecordVideoTime = high_resolution_clock::now();

void callbackFunction(int event,int x,int y, int flags,void* param)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		
		if (!isClicked) // If user click mouse
		{
			isClicked = true;
			// Check if capture image
			if (x >= posCaptureButtonX && y >= posCaptureButtonY &&
			x <= (posCaptureButtonX + widthButton) && y <= (posCaptureButtonY + heightButton))
			{
				if (!isRecordVideo)
				{
					isCapture = true;
				}
				
			}
			// check if record video
			if (x >= posRecordButtonX && y >= posRecordButtonY &&
			x <= (posRecordButtonX + widthButton) && y <= (posRecordButtonY + heightButton))
			{
				if (!isRecordVideo) // If video is not recording, start record video
				{
					isRecordVideo = true; // Start record video
					isStopingRecordVideo = false;
					isSetStartTimeRecordVideo = false;
					isSetTotalTimeRecordVideo = false;
				}
				else // If video is recording, stop record video
				{
					isRecordVideo = false; // Stop record video
					isStopingRecordVideo = true;
				}
				
				
				
			}
		}
		
	}
	if (event == EVENT_LBUTTONUP) // Mouse up, set isClicked to false
	{
		isClicked = false;
	}
    
}
int main()
{
    
    struct rusage start1, end1;

    getrusage(RUSAGE_SELF, &start1);
    cout << start1.ru_utime.tv_sec;
    cv::Mat image;
    image = cv::imread("./test.jpeg", 1);
    Mat img(200, 200, CV_8UC4, Scalar(255,255, 255));
    
    Mat background(heightWindow, widthWindow, CV_8UC3, Scalar(255,255, 255)); // Background

    Mat captureButton(heightButton, widthButton, CV_8UC3, Scalar(0,0, 255)); // Set image as button for capture image
    Mat recordButton(heightButton, widthButton, CV_8UC3, Scalar(0, 255, 255)); // Set image as button for record video
    Mat cameraHeader(heightButton, widthButton, CV_8UC3, Scalar(0, 255, 255)); // Camera header
    Mat timeHeader(heightTimeHeader, widthTimeHeader, CV_8UC3, Scalar(0, 255, 255)); // Time header, will show time capture or record video
    
    cv::Rect roi(cv::Rect(posCaptureButtonX,posCaptureButtonY,captureButton.cols, captureButton.rows)); // Copy all header and button to background, after show background to window
    cv::Mat targetROI = background(roi);
    captureButton.copyTo(targetROI);
    targetROI = background(cv::Rect(posRecordButtonX,posRecordButtonY,recordButton.cols, recordButton.rows));
    recordButton.copyTo(targetROI);
    targetROI = background(cv::Rect((widthWindow - widthButton) / 2,150,recordButton.cols, recordButton.rows));
    cameraHeader.copyTo(targetROI);
    targetROI = background(cv::Rect( posTimeHeaderX,posTimeHeaderY,timeHeader.cols, timeHeader.rows));
    timeHeader.copyTo(targetROI);
    
    putText(background,"Capture",Point(0,25),FONT_HERSHEY_COMPLEX_SMALL,1,Scalar(0,0,0),1,false); // Show text to background
    putText(background,"Record",Point(0,100),FONT_HERSHEY_COMPLEX_SMALL,1,Scalar(0,0,0),1,false);
    putText(background,"Camera",Point((widthWindow - widthButton) / 2,175),FONT_HERSHEY_COMPLEX_SMALL,1,Scalar(0,0,0),1,false);
    putText(background,"Time",Point(posTimeHeaderX,posTimeHeaderY + 25),FONT_HERSHEY_COMPLEX_SMALL,1,Scalar(0,0,0),1,false);
    cv::imshow("DisplayImage", background); // Display background to window
    setMouseCallback("DisplayImage", callbackFunction, 0);
    VideoCapture camera(0); // Get camera from list camera
    Mat frame;
    camera >> frame; // Get frame from camera
    //int frame_width = frame.size().width;
    //int frame_height = frame.size().height;
    int frameWidth = frame.size().width; // Weight video
    int frameHeight = frame.size().height; // Height video
    int fps = 24; // Fps video
    VideoWriter video;
    //auto start;
    while (true)
    {
    	
    	
    	getrusage(RUSAGE_SELF, &start1);	
    	if (isCapture) // If user click to capture button
    	{
    		isCapture = false;
    		auto start = high_resolution_clock::now(); // Set start time capture image
    		camera >> frame; // Set current frame of Camera to frame variable
    		imwrite("./001.jpg", frame);
    		auto stop = high_resolution_clock::now(); // Set end time capture image
    		// Now show time capture to screen
    		targetROI = background(cv::Rect( posTimeHeaderX,posTimeHeaderY,timeHeader.cols, timeHeader.rows));
    		timeHeader.copyTo(targetROI);
    		auto duration = duration_cast<microseconds>(stop - start);
    		string textTime = to_string((float)duration.count() / 1000000);
    		putText(background,textTime,Point(posTimeHeaderX,posTimeHeaderY + 25), FONT_HERSHEY_COMPLEX_SMALL,1,Scalar(0,0,0),1,false);
    		//
    	}
    	
    	else if (isRecordVideo) // If user click to record video
    	{

    		if (!isSetStartTimeRecordVideo)
    		{
    			isSetStartTimeRecordVideo = true;
    			startRecordVideoTime = high_resolution_clock::now(); // Set start time record video
    			video = VideoWriter("./001.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(frameWidth, frameHeight), true); // Start write video
    		}
    		camera >> frame; // Set current frame of Camera to frame variable
    		video.write(frame);
    		// Change button "Record video" to button "Stop record"
    		targetROI=background(cv::Rect(posRecordButtonX,posRecordButtonY,recordButton.cols, recordButton.rows));
    		recordButton.copyTo(targetROI);
    		putText(background,"Stop record",Point(0,100),FONT_HERSHEY_COMPLEX_SMALL,1,Scalar(0,0,0),1,false);
    		//
    	}
    	else
    	{
    		camera >> frame; // Set current frame of Camera to frame variable
    	}
    	
    	
    	// Set position of video, I try to set it is in center
    	int posVideoDemoX = (widthWindow - frameWidth) / 2;
    	int posVideoDemoY = 210;
    	//
    	
    	targetROI = background(Rect(posVideoDemoX,posVideoDemoY,frameWidth, frameHeight));
    	frame.copyTo(targetROI);
    	imshow("DisplayImage", background);
   
    	
        if (isStopingRecordVideo && !isSetTotalTimeRecordVideo) // If user stop record, stop program to save video
        {
        	stopRecordVideoTime = high_resolution_clock::now(); // Set stop time record video
        	isSetStartTimeRecordVideo = false;
        	isSetTotalTimeRecordVideo = true;
        	
        	// Show time to screen
        	targetROI = background(cv::Rect( posTimeHeaderX,posTimeHeaderY,timeHeader.cols, timeHeader.rows));
    		timeHeader.copyTo(targetROI);
    		// Get time and show time to screen
        	auto duration = duration_cast<microseconds>(stopRecordVideoTime - startRecordVideoTime);
    		string textTime = to_string((float)duration.count() / 1000000);
    		putText(background,textTime,Point(posTimeHeaderX,posTimeHeaderY + 25), FONT_HERSHEY_COMPLEX_SMALL,1,Scalar(0,0,0),1,false);
    		targetROI=background(cv::Rect(posRecordButtonX,posRecordButtonY,recordButton.cols, recordButton.rows));
    		recordButton.copyTo(targetROI);
    		putText(background,"Start record",Point(0,100),FONT_HERSHEY_COMPLEX_SMALL,1,Scalar(0,0,0),1,false);
        }
        char c = (char)waitKey(33); // if user press esc, close application
        if (c == 27) break;
    }
    //auto stop = high_resolution_clock::now();
    //auto duration = duration_cast<microseconds>(stop - start);
    //cout << (float)duration.count() / 1000000;

    return 0;
}

