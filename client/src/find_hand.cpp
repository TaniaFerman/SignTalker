//#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
//#include "fourier.cpp"

#include <iostream>
#include  <vector>

#ifdef debug
#include <stdarg.h> //needed for va_args
#include <time.h> //needed for strftime
void print(const char* format, ... );
#endif

using namespace std;
using namespace cv;

Mat skinFilter(const Mat& src, Scalar lower, Scalar upper);
void preprocess( Mat src, Mat &dst, Mat &mask );
int cluster(Mat &hsv, Mat &mask, Mat &cluster0, Mat &cluster1);
Mat postprocess(Mat &hsv, Mat &gray, Mat &mask, vector<Point> &shape);
Mat find_edges(Mat &gray);
void show(const char *name, Mat &img);

void findHand(Mat &src, vector<Point> &points);

/* Fourier prototypes */

void myFourier(vector<Point> &contours, vector<Point> &fourier);
bool myLess(Point a, Point b, Point center);


int main( int argc, char** argv )
{
   
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <video file>" << endl;
        return -1;
    }

    VideoCapture cap(argv[1]); // open the default camera
    if(!cap.isOpened())  { // check if we succeeded
        cout << "Failed to open " << argv[1] << endl;
        return -1;
    }

    Mat src;
    vector<Point> prev_shape;
    for(;;)
    {
        Mat frame;
        cap >> src; // get a new frame from camera

        if (src.empty())
            break;

        vector<Point> fourier;
        findHand(src, fourier);
       
        /* 
        if (prev_shape.size() > 0) {
            double res = matchShapes(shape, prev_shape, CV_CONTOURS_MATCH_I3, 0);
            print("Shape match = %f", res);
        } 

        prev_shape = shape;
        */
        waitKey(200); 

   }

   return 0;
}

void findHand(Mat &src, vector<Point> &fourier) {
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

    myFourier(shape, fourier);
    for(int i = 0; i < fourier.size(); i++) 
        circle(src, fourier[i], 10, Scalar::all(255), -1, 8, 0);
    show("Fourier", src);

    //print("Fourier points = %d", fourier.size());

    src = result;
}

void fourierDescriptor(Mat &I)
{
    Mat padded;                            //expand input image to optimal size
    int m = getOptimalDFTSize( I.rows );
    int n = getOptimalDFTSize( I.cols ); // on the border add zero values
    copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));

    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat complexI;
    merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

    dft(complexI, complexI);            // this way the result may fit in the source matrix

    // compute the magnitude and switch to logarithmic scale
    // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
    split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
    Mat magI = planes[0];

    magI += Scalar::all(1);                    // switch to logarithmic scale
    log(magI, magI);

    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

    // rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = magI.cols/2;
    int cy = magI.rows/2;

    Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

    Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);

    normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a
                                            // viewable image form (float between values 0 and 1).

    show("Input Image"       , I   );    // Show the result
    show("spectrum magnitude", magI);
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
    double res = norm(a-b);

    //print("Label 0 = %d", sum);
    //print("Label 1 = %d", (labels.rows - sum));

    /*
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

Mat getFourierDescriptor( vector<Point> &points/*, Mat &mask*/)
{

   
    /* 
    std::vector<Point> interpolated(data, data + sizeof(data) / sizeof(double));
    std::vector<Point> fft;

    cv::dft(interpolated, fft);
    */

    /*    
    Mat p_complex = Mat::zeros(points.size()).astype(complex); 
    for (int i = 0; i < points.size(); i++) {
        Point p = points.at<Point>(i);
        p_complex.at<complex>(i).real = p.x;
        p_complex.at<complex>(i).imag = p.y;
    }
    */
    
    //All x coordinates are real
    //All y coordinates are imaginary
    Mat fourierTransform;
    
    //Mat hand = Mat(points);
    //hand.convertTo(hand, CV_32FC2);
    //dft(hand, fourierTransform, DFT_SCALE|DFT_COMPLEX_OUTPUT);
   
    /* 
    int sum = 0;
    for(int i = 0; i < fourierTransform.rows; i++)
    {
        for(int j = 0; j < fourierTransform.cols; j++) {
            const int* FFT = fourierTransform.ptr<int>(i, j);
            cout << FFT[0] << "," << FFT[1] << endl;
        }
    }
   
    double center = points.size() / 2.0;
    float left = (center_index - 18) / 2;
    float right = (center_index + 18) / 2;
    */
    return fourierTransform;
}

/************************** Fourier code *********************************************/

class MatchDescriptor {
public :
    vector<Point2d> sContour;
    vector<complex<float> > b;
    vector<complex<float> > a;
    vector<float> frequence;
    vector<float> rho,psi;
    double pi;
    int nbDesFit;

public :
    MatchDescriptor(){nbDesFit=7;pi=acos(-1.0);};;
    float AjustementRtSafe(vector<Point2d> &c,float &alphaMin,float &phiMin,float &sMin);
    float Ajustement(vector<Point2d> &c,float &alphaMin,float &phiMin,float &sMin);
    void falpha(float x,float *fn,float *df);
    void InitFrequence();
    float rtsafe(float x1,float x2,float xacc);
    float Distance(complex<float> r,float alpha)
    {
    long			i;
    complex<float>		j(0,1);
    float 		d=0;

    for (i=1;i<=nbDesFit;i++)
    {
        d += abs(a[i]-b[i]*r*exp(j*float(alpha*frequence[i])))+ abs(a[a.size()-i]-b[a.size()-i]*r*exp(j*float(alpha*frequence[a.size()-i])));
    }
    return d;
    };
   
};

void MatchDescriptor::InitFrequence()
{
    long i;
    int nbElt=sContour.size();
    frequence.resize(sContour.size());

    for (i=0;i<=nbElt/2;i++)
        frequence[i] = 2*pi*(float)i/nbElt;
    for (i=nbElt/2+1;i<nbElt;i++)
        frequence[i] = 2*pi*(float)(i-nbElt)/nbElt;
}


void MatchDescriptor::falpha(float x,float *fn,float *df)
{
    long 	n,nbElt = sContour.size();
    float	s1=0,s2=0,s3=0,s4=0;
    float	ds1=0,ds2=0,ds3=0,ds4=0;

    for (n=1;n<=nbDesFit;n++)
        {
        s1 += 	rho[n] * sin(psi[n]+frequence[n]*x) + 
                rho[nbElt-n] * sin(psi[nbElt-n]+frequence[nbElt-n]*x);
        s2 += 	frequence[n] * rho[n] * cos(psi[n]+frequence[n]*x) +
                frequence[nbElt-n] * rho[nbElt-n] * cos(psi[nbElt-n]+frequence[nbElt-n]*x);
        s3 += 	rho[n] * cos(psi[n]+frequence[n]*x) +
                rho[nbElt-n] * cos(psi[nbElt-n]+frequence[nbElt-n]*x);
        s4 += 	frequence[n] * rho[n] * sin(psi[n]+frequence[n]*x) +
                frequence[nbElt-n] * rho[nbElt-n] * sin(psi[nbElt-n]+frequence[nbElt-n]*x);
        ds1 += 	frequence[n]*rho[n] * cos(psi[n]+frequence[n]*x) +
                frequence[nbElt-n]*rho[nbElt-n] * cos(psi[nbElt-n]+frequence[nbElt-n]*x);
        ds2 += 	-frequence[n]*frequence[n] * rho[n] * sin(psi[n]+frequence[n]*x) - 
                frequence[nbElt-n]*frequence[nbElt-n] * rho[nbElt-n] * sin(psi[nbElt-n]+frequence[nbElt-n]*x);
        ds3 += 	-frequence[n]*rho[n] * sin(psi[n]+frequence[n]*x) -
                frequence[nbElt-n]*rho[nbElt-n] * sin(psi[nbElt-n]+frequence[nbElt-n]*x);
        ds4 += 	frequence[n]*frequence[n] * rho[n] * cos(psi[n]+frequence[n]*x) +
                frequence[nbElt-n]*frequence[nbElt-n] * rho[nbElt-n] * cos(psi[nbElt-n]+frequence[nbElt-n]*x);
        }
    *fn = s1 * s2 - s3 *s4;
    *df = ds1 * s2 + s1 * ds2 - ds3 * s4 -  s3 * ds4;
}

float MatchDescriptor::AjustementRtSafe(vector<Point2d> &c,float &alphaMin,float &phiMin,float &sMin)
{
    long		    n,nbElt = sContour.size();
    float 		    s1,s2,sign1,sign2,df,x1=nbElt,x2=nbElt,dx;
    float		    dist,distMin = 10000,alpha,s,phi;
    complex<float> 	j(0,1),zz;

    InitFrequence();
    rho.resize(nbElt);
    psi.resize(nbElt);

    b.resize(nbElt);
    a.resize(nbElt);
    
    if (nbElt!=c.size())
        return -1;
    for (n=0;n<nbElt;n++)
    {
        b[n] = complex<float>(sContour[n].x,sContour[n].y);
        a[n]=complex<float>(c[n].x,c[n].y);
        zz = conj(a[n])*b[n];
        rho[n] = abs(zz);
        psi[n] = arg(zz);
    }
    float xp=-nbElt,fnp,dfp;
    falpha(xp,&fnp,&dfp);

    x1=nbElt,x2=nbElt;
    sMin =1;
    alphaMin = 0;
    phiMin = arg(a[1]/b[1]);
    
    do 
    {
        x2 = x1;
        falpha(x2,&sign2,&df);
        dx = 1;
        x1 = x2;
        do
        {
            x2=x1;
            x1 -= dx;
            falpha(x1,&sign1,&df);
        }
        while ((sign1*sign2>0)&&(x1>-nbElt));
        
        if (sign1*sign2<0)
        { 
            alpha = rtsafe(x1,x2,1e-8);
            falpha(alpha,&sign1,&df);
            alpha = alpha;
            s1 = 0;
            s2 = 0;
            for (n=1;n<nbElt;n++)
            {
                s1 += 	rho[n] * sin(psi[n]+frequence[n]*alpha); 
                s2 += 	rho[n] * cos(psi[n]+frequence[n]*alpha);
            }
            phi = -atan(s1/s2);
            phi =-atan2(s1,s2);
            s1 = 0;
            s2 = 0;
            
            for (n = 1; n < nbElt; n++)
            {
                s1 += 	rho[n] * cos(psi[n]+frequence[n]*alpha+phi) ; 
                s2 +=  abs(b[n] * conj(b[n]));
            }
            s = s1/s2;
            zz = s*exp(j*phi);
            
            if (s>0)
                dist = Distance(zz,alpha);
            else
                dist = 10000;
            
            if (dist<distMin)
            {
                distMin = dist;
                alphaMin = alpha;
                phiMin = phi;
                sMin = s;
            }
        }
    }
    while ((x1>-nbElt));
    return distMin;
}


#define MAXIT 100

float MatchDescriptor::rtsafe(float x1,float x2,float xacc)
{
	long j;
	float df,dx,dxold,f,fh,fl;
	float temp,xh,xl,rts;

	falpha(x1,&fl,&df);
	falpha(x2,&fh,&df);
	if (fl < 0.0) {
		xl=x2;
		xh=x1;
	} else {
		xh=x2;
		xl=x1;
	}
	rts=0.5*(x1+x2);
	dxold=fabs(x2-x1);
	dx=dxold;
	falpha(rts,&f,&df);
	for (j=1;j<=MAXIT;j++) 
	{
		if ((((rts-xh)*df-f)*((rts-xl)*df-f) >= 0.0)
			|| (fabs(2.0*f) > fabs(dxold*df))) 
		{
			dxold=dx;
			dx=0.5*(xh-xl);
			rts=xl+dx;
			if (xl == rts) return rts;
		} 
		else 
		{
			dxold=dx;
			dx=f/df;
			temp=rts;
			rts -= dx;
			if (temp == rts) 
				return rts;
	    }
		if (fabs(dx) < xacc) 
			return rts;
		falpha(rts,&f,&df);
		if (f < 0.0)
			xl=rts;
		else
			xh=rts;
		}
    return 0.0;
}



Point2d Echantillon(vector<Point> &c,long i,float l1,float l2,float s)
{
    Point2d d = c[(i+1) % c.size()] - c[i % c.size()];
    Point2d p = Point2d(c[i % c.size()]) + d * (s-l1)/(l2-l1);
    return p;
}


vector<Point2d> ReSampleContour(vector<Point> &c, int nbElt)
{
    long 		nb=c.size();
    float		l1=0,l2,p,d,s;
    vector<Point2d> r;
    int j=0;
    p = arcLength(c,true);

    l2 = norm(c[j]-c[j+1])/p;
    for(int i=0;i<nbElt;i++)
    {
        s = (float)i/(float)nbElt;
        while (s>=l2)
            {
            j++;
            l1 = l2;
            d = norm(c[j % nb]-c[(j+1) % nb]);
            l2 = l1+d/p;
            }
        if ((s>=l1)&&(s<l2))
            r.push_back( Echantillon(c,j,l1,l2,s));
    }
    return r;
}

void myFourier(vector<Point> &contours, vector<Point> &fourier)
{
    
    int min_descriptors = 64;
    vector<Point2d>  z;
    vector<Point2d>  Z;

    vector<Point2d> c = ReSampleContour(contours,min_descriptors);
    for (int j=0;j<c.size();j++)
        z.push_back(c[(j*10)%c.size()]);
    dft(z,Z,DFT_SCALE|DFT_REAL_OUTPUT);
   
    MatchDescriptor md;

    //This would be the reference contour    
    md.sContour=Z;
    md.nbDesFit=20;
    float alpha,phi,s;

    md.AjustementRtSafe(Z,alpha,phi,s);
    
    complex<float> expitheta=s*complex<float>(cos(phi), sin(phi));
    for (int j=1;j<Z.size();j++)
    {
        complex<float> zr(Z[j].x,Z[j].y);
        zr= zr*expitheta*exp(alpha*md.frequence[j]*complex<float>(0,1));
        Z[j].x = zr.real();
        Z[j].y = zr.imag();
    }
    dft(Z,z,DFT_INVERSE);
    for (int j = 0; j<z.size();j++)
        fourier.push_back(Point(z[j].x, z[j].y));
  
    /* 
    Mat mc=Mat::zeros(result.size(),CV_8UC3);
    Point baricenter(0,0);
    for( int i = 0; i < ctrRotated.size(); i++ )
        baricenter += ctrRotated[i];
    baricenter.x /= ctrRotated.size();
    baricenter.y /= ctrRotated.size();
    std::sort(ctrRotated.begin()+4, ctrRotated.end(),[baricenter](Point i, Point j){ return myLess(i,j,baricenter); });
    */
}

bool myLess(Point a, Point b, Point center)
{
    if (a.x - center.x >= 0 && b.x - center.x < 0)
        return true;
    if (a.x - center.x < 0 && b.x - center.x >= 0)
        return false;
    if (a.x - center.x == 0 && b.x - center.x == 0) {
        if (a.y - center.y >= 0 || b.y - center.y >= 0)
            return a.y > b.y;
        return b.y > a.y;
    }

    // compute the cross product of vectors (center -> a) x (center -> b)
    int det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
    if (det < 0)
        return true;
    if (det > 0)
        return false;

    // points a and b are on the same line from the center
    // check which point is closer to the center
    int d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
    int d2 = (b.x - center.x) * (b.x - center.x) + (b.y - center.y) * (b.y - center.y);
    return d1 > d2;
}

