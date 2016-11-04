/*
 * detect_any.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: Nora Huang
 */


#include <opencv2/core/core_c.h>
#include <opencv2/core/cvstd.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <stddef.h>
#include <cstdio>
#include <iostream>
#include <vector>

 using namespace std;
 using namespace cv;

 /** Function Headers */
 void detectAndDisplay( Mat frame );

 /** Global variables */
 string sign_cascade_fullpath;
 string sign_cascade_folder = "../haar_training/data/";
 string sign_cascade_name = "/cascade.xml";
 string sign_hand;
 CascadeClassifier sign_cascade;
 string window_name = "American Sign Language Learning";
 int frameDetectNum = 0;
 RNG rng(12345);

 /** @function main */
 int main( int argc, const char** argv )
 {
   CvCapture* capture;
   Mat frame;
   IplImage* iplImg = cvQueryFrame( capture );



   if (argc == 2)
   {
	   printf("Have correct number of arguments: %s", argv[1]);
	   sign_hand = argv[1];
	   sign_cascade_fullpath = sign_cascade_folder + sign_hand + sign_cascade_name;

   }
   else if(argc<2)
   {
	   printf("--(!)Error Please input signAndHand\n Example: AR (means sign A with Right hand)");
	   return -1;
   }else
   {
	   printf("--(!)Error Too much argument");
	   return -1;
   }


   //-- 1. Load the cascades
   if( !sign_cascade.load( sign_cascade_fullpath ) ){ printf("--(!)Error loading\n"); return -1; };

   //-- 2. Read the video stream
   capture = cvCaptureFromCAM( -1 );
   if( capture )
   {
     while( true )
     {
       iplImg = cvQueryFrame( capture );
       frame = cv::cvarrToMat(iplImg);

   //-- 3. Apply the classifier to the frame
       if( !frame.empty() )
       {
    	   detectAndDisplay( frame );
    	   /*if (frameDetectNum > 35)
    	   {
    		   printf("You have finish the task!!!!"); break;
    	   }*/

       }
       else
       { printf(" --(!) No captured frame -- Break!"); break; }

       int c = waitKey(10);
       if( (char)c == 'c' ) { break; }
      }
   }
   return 0;
 }

/** @function detectAndDisplay */
void detectAndDisplay( Mat frame )
{
  std::vector<Rect> signs;
  Mat frame_gray;
  bool signDetected = false;

  cvtColor( frame, frame_gray, CV_BGR2GRAY );
  equalizeHist( frame_gray, frame_gray );

  //-- Detect "a sign"
  sign_cascade.detectMultiScale( frame_gray, signs, 1.1, 3, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

  //printf("sign number:%lu", signs.size());

  if (signs.size() > 0)
  {
  Rect maxsign = signs[0];
  int maxsignwidth = maxsign.width;

  for( size_t i = 0; i < signs.size(); i++ )
  {
	     Point center( signs[i].x + signs[i].width*0.5, signs[i].y + signs[i].height*0.5 );
	     ellipse( frame, center, Size( signs[i].width*0.5, signs[i].height*0.5), 0, 0, 360, Scalar( 200, 200, 200 ), 4, 8, 0 );
	if (signs[i].size().width > maxsignwidth)
	{
       maxsign = signs[i];
	}
  }
  //if (maxsign.height > frame.size().height/3)
  {
     Point center( maxsign.x + maxsign.width*0.5, maxsign.y + maxsign.height*0.5 );
     ellipse( frame, center, Size( maxsign.width*0.5, maxsign.height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
     signDetected = true;
  }
  }
  if (signDetected)
  {
     frameDetectNum++;
  }

  if (frameDetectNum > 20)
  {
	  //putText(frame, "Great job! You make it", cvPoint(30,90), FONT_HERSHEY_TRIPLEX, 0.8, cvScalar(255, 0, 255), 1, CV_AA);
  }

  string sign = sign_hand.substr(0,1);
  string hand = sign_hand.substr(1,1);
  if (hand == "r" or hand =="R")
  {
	  hand = "Right hand";
  }else
  {
	  hand = "Left hand";
  }

  putText(frame, "Please give me a sign of " + sign + " with your " + hand, cvPoint(30,30), FONT_HERSHEY_TRIPLEX, 0.8, cvScalar(255, 0, 255), 1, CV_AA);
  //-- Show what you got
  imshow( window_name, frame );
 }
