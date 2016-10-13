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

using namespace cv;
using namespace std;

extern "C" {

Mat skinFilter(const Mat& src, Scalar lower, Scalar upper);
void preprocess( Mat src, Mat &dst, Mat &mask );
int cluster(Mat &hsv, Mat &mask, Mat &cluster0, Mat &cluster1);
void postprocess(Mat &hsv, Mat &gray, Mat &mask);
Mat find_edges(Mat &gray);
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

void findHand(Mat &src)
{
    Mat equ, hsv, mask;
    preprocess(src, equ, mask);

    cvtColor(src, hsv, COLOR_BGR2HSV);

    Mat cluster0, cluster1;
    int c = cluster(hsv, mask, cluster0, cluster1);

    if (c == 0 )
        src = cluster0;
    else
        src = cluster1;

}


int cluster(Mat &hsv, Mat &mask, Mat &cluster0, Mat &cluster1) {

    int hbins = 30, sbins = 32;
    int hBinSize = 180 / hbins;
    int sBinSize = 256 / 32;
    int histSize[] = {hbins, sbins};

    float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges };
    MatND hist;
    int channels[] = {0, 2};  //hue, value

    calcHist( &hsv, 1, channels, mask, // Mat() do not use mask
              hist, 2, histSize, ranges,
              true, // the histogram is uniform
              false );
    double maxVal=0;
    minMaxLoc(hist, 0, &maxVal, 0, 0);

    int scale = 10;
    vector<Point3f> points;

    //namedWindow( "test", 1 );
    int idx = 0;
    for( int h = 0; h < hbins; h++ ) {
        for( int s = 0; s < sbins; s++ )
        {
            float binVal = hist.at<float>(h, s);
            int intensity = cvRound(binVal*255/maxVal);

            if (intensity > 27) {
                points.push_back(Point3f(h,s,intensity));
                //cout << "Adding: " << h << "," << s << "," << intensity << endl;
            }
        }
    }

    Mat centers(2,1, CV_32FC1);
    Mat labels;
    Mat p = Mat(points);
    p.convertTo(p, CV_32F);


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
            //cout << L[j] << "," << P[j] << P[j+1] << P[j+2] << endl;
            Mat tmp = skinFilter(hsv, Scalar(P[j]*hBinSize, P[j+2], P[j+1]*sBinSize), Scalar((P[j]+1)*hBinSize, 256, (P[j+1]+1)*sBinSize));
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
    double res = norm(a-b);

    ostringstream out;
    out << "Label 0 = " << sum;
    __android_log_write(ANDROID_LOG_ERROR, "MyLogs", out.str().c_str());
    out << "Label 1 = " << labels.rows - sum;
    __android_log_write(ANDROID_LOG_ERROR, "MyLogs", out.str().c_str());
    out << "Diff = " << res;
    __android_log_write(ANDROID_LOG_ERROR, "MyLogs", out.str().c_str());

    if (res < 10) {
        bitwise_or(cluster0, cluster1, cluster0);
        return 0;
    }

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

}