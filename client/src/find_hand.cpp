//#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"

#include <iostream>
#include  <vector>

using namespace std;
using namespace cv;

Mat skinFilter(const Mat& src, Scalar lower, Scalar upper);
void preprocess( Mat src, Mat &dst, Mat &mask );
int cluster(Mat &hsv, Mat &mask, Mat &cluster0, Mat &cluster1);
void postprocess(Mat &hsv, Mat &gray, Mat &mask);
Mat find_edges(Mat &gray);


int main( int argc, char** argv )
{
    Mat src, equ, hsv, mask, color_codes;
    if( argc != 2 || !(src=imread(argv[1], 1)).data )
        return -1;
    

    preprocess(src, equ, mask);
    
    cvtColor(src, hsv, COLOR_BGR2HSV);
   
    Mat cluster0, cluster1; 
    int c = cluster(hsv, mask, cluster0, cluster1);
   
    
    namedWindow( "Cluster", 0 );
    if (c == 0 )
        imshow( "Cluster", cluster0 );
    else
        imshow( "Cluster", cluster1 );

    resizeWindow("Cluster", 800, 600);
    waitKey(0); 
    /* 
    if (c == 0) 
        postprocess(hsv, equ, cluster0);
    else
        postprocess(hsv, equ, cluster1);
    */
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

    cout << "Label 0 = " << sum << endl;
    cout << "Label 1 = " << (labels.rows - sum) << endl;
    cout << "Diff = " << res << endl;

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

void postprocess(Mat &hsv, Mat &gray, Mat &mask) 
{
    Mat edges = find_edges(mask);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point() );
    
    /// Draw contours
    Mat drawing = Mat::zeros( gray.size(), CV_8UC1 );
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    vector<Point2f>center( contours.size() );
    vector<float>radius( contours.size() );
    
    for( int i = 0; i< contours.size(); i++ )
    { 
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
        rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), Scalar(255), 2, 8, 0 );
        //drawContours( drawing, contours_poly, i, 255, 1, 8, vector<Vec4i>(), 0, Point() );
        
        /*next, previous, child, parent
        if (hierarchy[0][1] == -1) { 
            drawContours( drawing, contours, i, 255, 2, 8, hierarchy, 0, Point() );
        }
        */
    }     

    Point seed = Point(center[0]);
    circle(drawing, seed, 10, Scalar(255), -1, LINE_8, 0 );

    Mat im_floodfill = hsv.clone(); //Mat::zeros( gray.size(), CV_8UC1 );
    Mat mask_floodfill;
    copyMakeBorder( drawing.clone(), mask_floodfill, 1, 1, 1, 1, BORDER_CONSTANT, 0 );

    Mat kernel = getStructuringElement(MORPH_ELLIPSE,Point(3,3));
    morphologyEx(mask_floodfill.clone(), mask_floodfill, MORPH_OPEN, kernel);
    dilate(mask_floodfill, mask_floodfill, kernel, Point(), 5); 

    Rect ccomp; 
    int flags = 4 + FLOODFILL_FIXED_RANGE;
    //int area = floodFill(im_floodfill, mask_floodfill, Point(center), 255, &ccomp, Scalar::all(0), Scalar::all(255),  flags );
    int area = floodFill(im_floodfill, mask_floodfill, seed, 255, &ccomp, Scalar(), flags );
    
    namedWindow( "Floodfill window", 0 );
    imshow( "Floodfill window", im_floodfill );
    resizeWindow("Floodfill window", 800, 600);
    
    namedWindow( "drawing", 0 );
    imshow( "drawing", drawing );
    resizeWindow("drawing", 800, 600);
    waitKey(0); 

}
