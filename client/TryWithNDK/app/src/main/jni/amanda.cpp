#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect.hpp>

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
string sign_cascade_folder = "data/";
string sign_cascade_name = "/cascade.xml";
string hand_type = "R";


bool checkIfCorrect(Mat &src, char letter) {

	string sign_cascade_fullpath = sign_cascade_folder + string(1, letter) + hand_type + sign_cascade_name;

	if(!sign_cascade.load(sign_cascade_fullpath)) {
		__android_log_print(ANDROID_LOG_ERROR, "checkIfCorrect", "Error loading cascade file %s", sign_cascade_fullpath.c_str());
		return false;
	}

	vector<Rect> signs;
	Mat gray, bw;

	cvtColor( src, gray, COLOR_BGR2GRAY );
	equalizeHist( gray, gray );

	//threshold(gray, bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	//adaptiveThreshold(gray, bw, 255, ADAPTIVE_THRESH_GAUSSIAN_C , THRESH_BINARY, 11, 2);

	sign_cascade.detectMultiScale( gray, signs, 1.1, 3, 0|CASCADE_SCALE_IMAGE /*,Size(30,30)*/ );
	//sign_cascade.detectMultiScale( bw, signs, 1.1, 3, 0|CASCADE_SCALE_IMAGE /*,Size(30,30)*/ );
	//sign_cascade.detectMultiScale( src, signs, 1.1, 3, 0|CASCADE_SCALE_IMAGE /*,Size(30,30)*/ );


	/* For testing */
	for( int i = 0; i < signs.size(); i++ )
	{
		rectangle( src, signs[i].tl(), signs[i].br(), Scalar(0,0,255) , 3, 8, 0 );
	}

	Mat left;
	cvtColor(gray, left, COLOR_GRAY2BGR);
	Mat img = merge(left,src);
	show("Result", img);
	/* end testing */

	sign_cascade.empty();
	if (signs.size() > 0) return true;

	return false;
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