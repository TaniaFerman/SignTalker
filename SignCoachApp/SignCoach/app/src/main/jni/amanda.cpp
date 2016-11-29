#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect.hpp>
//#include "opencv2/bgsegm.hpp"
#include <opencv2/video.hpp>

#include <iostream>
#include  <vector>

using namespace std;
using namespace cv;

/* Testing functions*/
void print(const char* format, ... );
void show(const char *name, cv::Mat &img);
void show(const char *name, cv::Mat &img1, cv::Mat &img2);
cv::Mat merge(cv::Mat &img1, cv::Mat &img2);
/* end testing*/

CascadeClassifier sign_cascade;
string sign_cascade_folder = "data/v1/";
//string sign_cascade_name = "/cascade.xml";
string sign_cascade_ext = ".xml";
string hand_type = "R";

Mat fgMask; 
Ptr<BackgroundSubtractor> pMOG2 = createBackgroundSubtractorMOG2();

char loaded_letter = ' '; // Set to the currently loaded cascade file
void rot90(Mat &src, int flag);
void fixRotation(Mat &src, Mat &dst, int rotation);

bool checkIfCorrect(Mat &src, char letter) {

	// Load required cascade file for this letter if not already loaded
	if (loaded_letter != letter) {
		//string sign_cascade_fullpath = sign_cascade_folder + string(1, letter) + hand_type + sign_cascade_name;
		string sign_cascade_fullpath = sign_cascade_folder + string(1, letter) + sign_cascade_ext;

		if(!sign_cascade.load(sign_cascade_fullpath)) {
			__android_log_print(ANDROID_LOG_ERROR, "checkIfCorrect", "Error loading cascade file %s", sign_cascade_fullpath.c_str());
			return false;
		}

		loaded_letter = letter;
	}
    
    //pMOG2->apply(src, fgMask, 0.25);

	vector<Rect> signs;
    Point br(int(src.cols * 0.85), int(src.rows * 0.85));
    Point tl(int(src.cols * 0.15), int(src.rows * 0.15));
    Rect centroid(tl,br);
    rectangle( src, tl, br, Scalar::all(255), 3, 8, 0 );
    

    //Mat gray;
	//cvtColor( src, gray, COLOR_BGR2GRAY );
	//equalizeHist( gray, gray );
	sign_cascade.detectMultiScale( src, signs, 1.1, 3, 0|CASCADE_SCALE_IMAGE ,Size(150,150) );

    
    // sign_cascade.detectMultiScale( fgMask, signs, 1.1, 3, 0|CASCADE_SCALE_IMAGE ,Size(150,150) );
    //Point center(src.cols / 2, src.rows / 2);

    int minIdx = -1;
    float maxArea = 0;
    //float minDist = src.cols*src.rows;
	for( int i = 0; i < signs.size(); i++ )
	{
        Rect r = signs[i];
        //Point p(r.x + r.width/2,  r.y+r.height/2);
        //float res = cv::norm(center-p);
        //float count = countNonZero(Mat(fgMask, r)); 
        
        float area = r.area(); 
        bool large_enough = area > 0.40*centroid.area();
        bool completely_in = (centroid & r).area() >= (area-20); //TODO: this 20 needs to be checked
        if (large_enough && completely_in && area > maxArea) {
            minIdx = i;
            maxArea = area;
        }

        /*        
        if (count > maxArea && res < minDist  && r.contains(center)) {
            minIdx = i;
            minDist = res;
            maxArea = count;
        }
        */
	}
    
    if (minIdx > -1) { 
        rectangle( src, signs[minIdx].tl(), signs[minIdx].br(), Scalar(0,0,255), 3, 8, 0 );
        return true;
    }

	return false;
}

void fixRotation(Mat &src, Mat &dst, int rotation) {
    Size sz = src.size();
    rot90(src, rotation);
    //copyMakeBorder( src, dst, top, bottom, left, right, borderType, value );
    resize(src,dst,sz);
    flip(src,src,1);
}

void rot90(Mat &src, int flag){
  //1=CW, 2=CCW, 3=180
  if (flag == 1){
    transpose(src, src);  
    flip(src, src,1); 
  } else if (flag == 2) {
    transpose(src, src);  
    flip(src, src,0); 
  } else if (flag ==3){
    flip(src, src,-1);    
  } else if (flag != 0){ 
    cout  << "Unknown rotation flag(" << flag << ")" << endl;
  }
}


/*************************DEBUG FUNCTIONS**********************************************************************/


void print(const char* format, ... ) {
#ifdef debug
	va_list args;

    char buff[100]; //Buffer for the time

    time_t now = time (0); //Get current time object

    //Format time object into string using the format provided
    strftime(buff, 100, "%Y-%m-%d %H:%M:%S", localtime (&now));

    //Print time to the screen
    printf ("%s: ", buff);

    //Based on format, read in args from (...)
    va_start(args, format);

    //Fill-in format with args and print to screen
    vprintf(format, args);

    //Release args memory
    va_end(args);

    //Create a newline
    printf("\n");

    //Flush standard out to make sure this gets printed
    fflush(stdout);
#endif
}

void show(const char *name, Mat &img)
{
#ifdef debug
	namedWindow( name, 0 );
    imshow( name, img );
    resizeWindow(name, 800, 600);
#endif
}

Mat merge(Mat &img1, Mat &img2)
{
	Size sz1 = img1.size();
	Size sz2 = img2.size();
	//assert(sz1 == sz2);

	Mat img3(sz1.height, sz1.width+sz2.width, CV_8UC3);
	Mat left(img3, Rect(0, 0, sz1.width, sz1.height));
	img1.copyTo(left);
	Mat right(img3, Rect(sz1.width, 0, sz2.width, sz2.height));
	img2.copyTo(right);
	return img3;
}
