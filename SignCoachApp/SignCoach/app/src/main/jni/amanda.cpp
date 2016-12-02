#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect.hpp>
//#include "opencv2/bgsegm.hpp"
#include <opencv2/video.hpp>

#include <iostream>
#include  <vector>
#include <string>

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
float getScaleFactor(char letter);

void darken(Mat &src, int maxInt, float phi, float theta);


float checkIfCorrect(Mat &src, char letter) {

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
    
    //pMOG2->apply(src, fgMask, 0.09);

	vector<Rect> signs;
    Point br(int(src.cols * 0.85), int(src.rows * 0.85));
    Point tl(int(src.cols * 0.15), int(src.rows * 0.15));
    Rect centroid(tl,br);
    rectangle( src, tl, br, Scalar::all(255), 3, 8, 0 );
    Point center(src.cols/2, src.rows/2);

    Point br2(int(src.cols * 0.90), int(src.rows * 0.90));
    Point tl2(int(src.cols * 0.10), int(src.rows * 0.10));
    Rect r_max(tl2,br2);
    //rectangle( src, tl2, br2, Scalar::all(255), 3, 8, 0 );
    int buffer = r_max.area() - centroid.area();

    Point br3(int(src.cols * 0.75), int(src.rows * 0.75));
    Point tl3(int(src.cols * 0.25), int(src.rows * 0.25));
    Rect r_min(tl3,br3);
    //rectangle( src, tl3, br3, Scalar::all(255), 3, 8, 0 );

    //Mat gray;
	//cvtColor( src, gray, COLOR_BGR2GRAY );
	//equalizeHist( gray, gray );
    //darken(gray, 255, 1, 1);
    //sign_cascade.detectMultiScale( src, signs, 1.1, 3, 0|CASCADE_SCALE_IMAGE ,Size(150,150));

    float scaleFactor = getScaleFactor(letter);
    vector<int> rejectLevels;
    vector<double> levelWeights;
    sign_cascade.detectMultiScale( src, signs, rejectLevels, levelWeights, scaleFactor, 3, 0|CASCADE_SCALE_IMAGE , r_min.size() /*Size(150,150)*/, r_max.size(), true);
    //Point center(src.cols / 2, src.rows / 2);

    int minIdx = -1;
    float maxArea = 0;
    int validCount = 0;
    float activities[signs.size()];
    //float minDist = src.cols*src.rows;
	for( int i = 0; i < signs.size(); i++ )
	{
        Rect r = signs[i];
        Point p(r.x + r.width/2,  r.y+r.height/2);
        float res = cv::norm(center-p);
        //float activity = countNonZero(Mat(fgMask, r)) / float(r.width*r.height);

        float area = r.area(); 
        bool large_enough = area > 0.40*centroid.area(); //TODO: 40 needs to be checked
        bool completely_in = (centroid & r).area() >= (area-buffer);

        /*
        if (large_enough && completely_in && area > maxArea) {
            minIdx = i;
            maxArea = area;
        }
        */

        __android_log_print(ANDROID_LOG_ERROR, "checkIfCorrect", "res = %f, Reject = %d, Weight = %f", res, rejectLevels[i], levelWeights[i]);
        char txt[20];
        sprintf(txt, "%0.2f", levelWeights[i]);

        //if (rejectLevels[i] > 2.0) {
            //putText(src, txt, r.tl(), 1, 3, Scalar::all(255), 5);
            //rectangle(src, r.tl(), r.br(), Scalar(255, 0, 0), 5, 8, 0); //RED
        //}

        //if (completely_in && r.contains(center)) {
        if(/*completely_in && res < 50*/ r.contains(center) && completely_in && levelWeights[i] > 2.5) {
            putText(src, txt, r.tl(), 1, 3, Scalar::all(255), 5);
            rectangle( src, r.tl(), r.br(), Scalar(0,255,0), 5, 8, 0 ); //GREEN
            validCount++;
            //__android_log_print(ANDROID_LOG_ERROR, "checkIfCorrect", "Activity = %f", activity);
            /*
            if (area > maxArea && large_enough) {
                minIdx = i;
                maxArea = area;
                rectangle( src, r.tl(), r.br(), Scalar(255,0,0), 5, 8, 0 ); //RED
            } else {
                rectangle( src, r.tl(), r.br(), Scalar(0,255,0), 5, 8, 0 ); //GREEN
            }
            */
        }

        //if (large_enough) rectangle( src, r.tl(), r.br(), Scalar(255,0,0), 8, 8, 0 ); //RED
        //if (!completely_in) rectangle( src, r.tl(), r.br(), Scalar(0,255,0), 5, 8, 0 ); //GREEN

        /*        
        if (count > maxArea && res < minDist  && r.contains(center)) {
            minIdx = i;
            minDist = res;
            maxArea = count;
        }
        */
	}

    flip(src,src,1);

    if (validCount == 0) return 0.0;

    return 1.0 / validCount;

    /*
    if (minIdx > -1) { 
        rectangle( src, signs[minIdx].tl(), signs[minIdx].br(), Scalar(0,0,255), -1, 8, 0 );
        return true;
    }

	return false;
    */
}

float getScaleFactor(char letter)
{
    float scale = 1.1;
    switch(letter) {
        case 'A':
        case 'C':
        case 'E':
        case 'I':
        case 'J':
        case 'L':
        case 'M':
        case 'N':
        case 'X': scale = 1.9; break;
        case 'K': scale = 2.7; break;
        case 'B':
        case 'D':
        case 'F':
        case 'O':
        case 'R':
        case 'U':
        case 'V':
        case 'W': scale = 1.9; break;
        case 'G':
        case 'H':
        case 'P':
        case 'Q':
        case 'S':
        case 'T':
        case 'Y':
        case 'Z': scale = 1.9; break;
        default: break;
    }

    return scale;
}

void fixRotation(Mat &src, Mat &dst, int rotation) {
    Size sz = src.size();
    rot90(src, rotation);
    //copyMakeBorder( src, dst, top, bottom, left, right, borderType, value );
    resize(src,dst,sz);
    //flip(src,src,1);
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

void darken(Mat &src, int maxInt, float phi, float theta)
{
    //src = exp((maxInt / phi) * (src / (maxInt / theta)), 2);
    int i,j;
    uchar* p;
    for( i = 0; i < src.rows; ++i)
    {
        p = src.ptr<uchar>(i);
        for ( j = 0; j < src.cols; ++j)
        {
            float val = p[j] / (maxInt / theta);
            val = pow(val, 2);
            uchar final_val = uchar((maxInt / phi) * val);
            src.at<uchar>(i,j) = final_val;
        }
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
