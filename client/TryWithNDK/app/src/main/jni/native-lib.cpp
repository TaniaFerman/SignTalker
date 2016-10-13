#include <jni.h>
#include <sstream>
#include <string>
#include <vector>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <android/log.h>

#define LOG_TAG "Native_Lib_JNI"

#ifdef debug
#include <stdarg.h> //needed for va_args
#include <time.h> //needed for strftime
void print(const char* format, ... );
#endif

using namespace cv;
using namespace std;

extern "C" {

Mat skinFilter(const Mat& src, Scalar lower, Scalar upper);
void preprocess( Mat src, Mat &dst, Mat &mask );
int cluster(Mat &hsv, Mat &mask, Mat &cluster0, Mat &cluster1);
Mat postprocess(Mat &hsv, Mat &gray, Mat &mask, vector<Point> &shape);
Mat find_edges(Mat &gray);
void show(const char *name, Mat &img);

Mat getFourierDescriptor( vector<Point> &points);

void findHand(Mat &src);


void logTxt(const char *txt);

JNIEXPORT jboolean JNICALL
Java_com_example_danyalejandro_trywithndk_MainActivity_nativeFunction(JNIEnv *env, jobject instance, long iAddr) {

    // Prepare source image
    Mat *src = (Mat*) iAddr;
    flip(*src, *src, 1);

    // Amanda's magic
    logTxt("hola mundo!");
    findHand(*src);


    //Mat imgt = img->t();
    //transpose(*img, *img);

    //Canny(*img, *img, 80, 100, 3);

    return true;
}

void logTxt(const char *txt) {
    __android_log_write(ANDROID_LOG_ERROR, "MyLogs", txt);
}

void findHand(Mat &src) {
    Mat equ, hsv, mask;
    
    preprocess(src, equ, mask);
    
    cvtColor(src, hsv, COLOR_BGR2HSV);
   
    Mat cluster0, cluster1;
    int c = cluster(hsv, mask, cluster0, cluster1);


    Mat cluster_inv;

    if (c == 0) 
        bitwise_not ( cluster0, cluster_inv ); 
    else if (c == 1) 
        bitwise_not ( cluster1, cluster_inv ); 
    else
        return;    

    show("Cluster", cluster_inv);
    vector<Point> shape;
    Mat result = postprocess(hsv, equ, cluster_inv, shape);

    getFourierDescriptor(shape); 

    src = result;
}

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

int cluster(Mat &hsv, Mat &mask, Mat &cluster0, Mat &cluster1) {

    //int hbins = 30, sbins = 32;
    int hbins = 20, sbins = 16;
    int hBinSize = 180 / hbins;
    int sBinSize = 256 / sbins;
    int histSize[] = {hbins, sbins};
    
    float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges };
    MatND hist;
    int channels[] = {0, 1};  //hue, value

    calcHist( &hsv, 1, channels, Mat(), // Mat() do not use mask
             hist, 2, histSize, ranges,
             true, // the histogram is uniform
             false );
    double maxVal=0;
    minMaxLoc(hist, 0, &maxVal, 0, 0);

    vector<Point3f> points;
    
    int idx = 0;
    for( int h = 0; h < hbins; h++ ) {
        for( int s = 0; s < sbins; s++ )
        {
            float binVal = hist.at<float>(h, s);
            int v = cvRound(binVal*255/maxVal);

            if (v > 0) {
                points.push_back(Point3f(h,s,v)); 
            }
        }
    }

    Mat centers(2,1, CV_32FC1);
    Mat labels;
    Mat p = Mat(points);
    p.convertTo(p, CV_32F);
   
    if ( points.size() < 2 ) {
        return -1;
    }

    kmeans( p, 2, labels, 
            TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 10, 1.0),
            10, KMEANS_PP_CENTERS, centers);


    cluster0 = Mat::zeros(hsv.rows, hsv.cols, CV_8UC1);
    cluster1 = Mat::zeros(hsv.rows, hsv.cols, CV_8UC1);
    
    int sum = 0;
    for(int i = 0; i < labels.rows; i++)
    {
        const int* L = labels.ptr<int>(i);
        const float* P = p.ptr<float>(i);
        for(int j = 0; j < labels.cols; j++) {
            Scalar lower((P[j]*hBinSize)-0, (P[j+1]*sBinSize)-20, 127); //P[j+2]-10);
            Scalar upper(((P[j]+1)*hBinSize)+0, ((P[j+1]+1)*sBinSize)+20, 255); //P[j+2]+10 );

            Mat tmp = skinFilter(hsv, lower, upper); 
            if (L[j] == 0) {
                sum++;
                bitwise_or(tmp, cluster0, cluster0);
            } else {
                bitwise_or(tmp, cluster1, cluster1);
            }

        }
    }


    Point2f a(centers.at<float>(0,0), centers.at<float>(0,1));
    Point2f b(centers.at<float>(1,0), centers.at<float>(1,1));

    //print("Label 0 = %d", sum);
    //print("Label 1 = %d", (labels.rows - sum));

    /*
    double res = norm(a-b);
    print("Diff = %d", res);
    if (res < 10) {
        bitwise_or(cluster0, cluster1, cluster0);
        return 0;
    }
    */

    int bestCluster = -1;
    if (sum > (labels.rows - sum)) {
        bestCluster = 0;
    } else {
        bestCluster = 1;
    }

    return bestCluster;
}


Mat skinFilter(const Mat& src, Scalar lower, Scalar upper)
{
    assert(src.type() == CV_8UC3);

    Mat skin_only; 
    inRange(src, lower, upper, skin_only);

    return skin_only;
}

/* brightness/contrast callback function */
void preprocess( Mat src, Mat &dst, Mat &mask )
{
    int brightness = 0;
    int contrast = 0;
    
    Mat image; 
    cvtColor(src, image, COLOR_BGR2GRAY);

    /*
     * The algorithm is by Werner D. Streidt
     * (http://visca.com/ffactory/archives/5-99/msg00021.html)
     */
    double a, b;
    if( contrast > 0 )
    {
        double delta = 127.*contrast/100;
        a = 255./(255. - delta*2);
        b = a*(brightness - delta);
    }
    else
    {
        double delta = -128.*contrast/100;
        a = (256.-delta*2)/255.;
        b = a*brightness + delta;
    }

    image.convertTo(dst, CV_8U, a, b);
    threshold(dst, mask, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

}

Mat find_edges(Mat &gray)
{
    Mat edges;
    float sigma = 0.50; 
    Scalar vs = cv::mean(gray);
    float v = vs[0];
    int lower = int( std::max(0.0, (1.0 - sigma) * v) );
    int upper = int( std::min(255.0, (1.0 + sigma) * v));
    Canny(gray, edges, lower, upper);

    return edges;
}

Mat postprocess(Mat &hsv, Mat &gray, Mat &mask, vector<Point> &shape) 
{
    Mat edges = find_edges(mask);
    show("Canny", edges); 
    
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0) );
    
    int longest = -1; int idx = -1;
    for( int i = 0; i< contours.size(); i++ )
    {
        int len = arcLength(contours[i], true);
        if (len > longest) {
            longest = len;
            idx = i;
        }
    } 
        
    Mat drawing = Mat::zeros( gray.size(), CV_8UC1 );
    drawContours( drawing, contours, idx, 255, 2, 8, hierarchy, 0, Point(0,0) );
    show("Drawing", drawing);
    shape = contours[idx];

    return drawing;
}

Mat getFourierDescriptor( vector<Point> &points)
{
    /*
    p_complex = np.empty(points.shape[:-1], dtype=complex)
    p_complex.real = points[:, 0]
    p_complex.imag = points[:, 1]
    */
    Mat p_complex = Mat::zeros(points.size()).astype(complex); 
    for (int i = 0; i < points.size(); i++) {
        Point p = points.at<Point>(i);
        p_complex.at<complex>(i) = complex(p.x, p.y);
    }

    //All x coordinates are real
    //All y coordinates are imaginary
    Mat fourierTransform;
    dft(Mat(points), fourierTransform, DFT_SCALE|DFT_COMPLEX_OUTPUT);
    return fourierTransform;
}


}
