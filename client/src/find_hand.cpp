//#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
//#include "fourier.cpp"

#include <iostream>
#include  <vector>

#include <stdarg.h> //needed for va_args
#include <time.h> //needed for strftime
#include <fstream>
void print(const char* format, ... );
void show(const char *name, cv::Mat &img);
void show(const char *name, cv::Mat &img1, cv::Mat &img2);
void writeToFile(std::fstream &fp, const char* format, std::vector<cv::Point> &fourier);
std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str);

using namespace std;
using namespace cv;

Mat skinFilter(const Mat& src, Scalar lower, Scalar upper);
void preprocess( Mat src, Mat &dst, Mat &mask );
int cluster(Mat &hsv, Mat &mask, Mat &cluster0, Mat &cluster1);
Mat postprocess(Mat &hsv, Mat &gray, Mat &mask, vector<Point> &shape);
Mat find_edges(Mat &gray);
Mat merge(Mat &img1, Mat &img2);

vector<vector<vector<Point> > > loadTemplates();

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
    
    
    //fstream fp;
    //fp.open ("fourier.csv",std::fstream::out);

    vector<vector<vector<Point> > > templates = loadTemplates();

    Mat src;
    bool loop = true;
    int fnum = -1;
    while(loop)
    {
        Mat frame;
        cap >> src; // get a new frame from camera
        fnum++;
        if (src.empty())
            break;

        vector<Point> fourier;
        Mat original = src.clone();
        findHand(src, fourier);
    
        float bestMatch = 1000;
        int bestIdx = -1; 
        for(int j = 0; j < templates.size(); j++) { 
            double res1 = 10000; //, res2, res3 = 0; 
            vector<vector<Point> > sContour = templates[j];
            for (int i = 0; i < sContour.size(); i++) { 
                double res = matchShapes(fourier, sContour[i], CV_CONTOURS_MATCH_I2, 0);
                if (res < res1) res1 = res;
                //res1 += res;    
                
            }
            
            //res1 /= sContour.size();

            if(res1 < bestMatch && res1 < 2.0) {
                bestMatch = res1;
                bestIdx = j;
            }
            
            print("%d match = %f (best=%d,%f)", j, res1, bestIdx, bestMatch);
        }

        if (bestIdx < 0) 
            putText(src, "No match", Point2f(20,50), FONT_HERSHEY_PLAIN, 4,  Scalar::all(255), 5);
        else if (bestIdx == 0) 
            putText(src, "Match=A", Point2f(20,50), FONT_HERSHEY_PLAIN, 4,  Scalar::all(255), 5);
        else if (bestIdx == 1) 
            putText(src, "Match=B", Point2f(20,50), FONT_HERSHEY_PLAIN, 4,  Scalar::all(255), 5);
        else if (bestIdx == 2) 
            putText(src, "Match=C", Point2f(20,50), FONT_HERSHEY_PLAIN, 4,  Scalar::all(255), 5);
        else if (bestIdx == 3) 
            putText(src, "Match=D", Point2f(20,50), FONT_HERSHEY_PLAIN, 4,  Scalar::all(255), 5);

        
        Mat left;
        cvtColor(src, left, COLOR_GRAY2BGR);
        Mat newImg = merge(left, original);
        show("Result", newImg);

        /*
        char str[200];
        sprintf(str, "demo/frame_%06d.png", fnum); 
        imwrite(str, newImg); 
        */
        int key = 0xff & waitKey(250); 
        
        switch(key) {
            /*case 'a': writeToFile(fp, "a", fourier); break;
            case 'b': writeToFile(fp, "b", fourier); break;
            case 'c': writeToFile(fp, "c", fourier); break;
            case 'd': writeToFile(fp, "d", fourier); break;
            */
            case 27: loop = false; 
            default: break;
        }
       
   }
  
   //fp.close(); 
   return 0;
}

vector<vector<vector<Point> > > loadTemplates()
{
    vector<string> templates = {"a.csv", "b.csv", "c.csv", "d.csv"};
    vector<vector<vector<Point> > >  all_contours;

    for (int j = 0; j < templates.size(); j++) {
        ifstream ifp(templates[j]);
        vector<vector<Point> > sContour;
        string label;
       
        if(ifp.is_open()) {

            for(;;) {
                vector<string> reference = getNextLineAndSplitIntoTokens(ifp);    
                vector<Point> contour;
                if (reference.size() > 0) {
                    label = reference[reference.size()-1]; 
                    for(int i = 0; i < reference.size()-2; i+=2)
                    {
                        contour.push_back(Point(std::stoi(reference[i]), std::stoi(reference[i+1]))); 
                    }
                    sContour.push_back(contour);
                } else {
                    break;
                }
            }
        }

        all_contours.push_back(sContour);
        ifp.close(); 
    }

    return all_contours;
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

#ifdef debug
    for(int i = 0; i < fourier.size(); i++) 
        circle(src, fourier[i], 10, Scalar::all(255), -1, 8, 0);
    show("Fourier", src);
#endif 

    src = result;
}

int cluster(Mat &hsv, Mat &mask, Mat &cluster0, Mat &cluster1) {

    int hbins = 30, sbins = 32;
    //int hbins = 20, sbins = 16;
    int hBinSize = 180 / hbins;
    int sBinSize = 256 / sbins;
    int histSize[] = {hbins, sbins};
    
    float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges };
    MatND hist;
    int channels[] = {0, 1};  //hue, saturation 

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

    cout << centers << endl;

    Point2f a(centers.at<float>(0,0), centers.at<float>(0,1));
    Point2f b(centers.at<float>(1,0), centers.at<float>(1,1));
    double res = norm(a-b);
    print("Diff = %d", res);
    print("Label 0 = %d", sum);
    print("Label 1 = %d", (labels.rows - sum));

    if (res < 3) {
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

Mat postprocess(Mat &hsv, Mat &gray, Mat &mask, vector<Point> &shape) 
{
    //Mat edges = find_edges(mask);
    //show("Canny", edges); 
    
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

std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str)
{
    std::vector<std::string>   result;
    std::string                line;
    std::getline(str,line);

    std::stringstream          lineStream(line);
    std::string                cell;

    while(std::getline(lineStream,cell, ','))
    {
        result.push_back(cell);
    }
    
    return result;
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

    /*
    ifstream fp("a.csv");
    vector<string> reference = getNextLineAndSplitIntoTokens(fp);    
    vector<Point2d> sContour;
    for(int i = 0; i < reference.size()-2; i+=2)
    {
        sContour.push_back(Point(std::stoi(reference[i]), std::stoi(reference[i+1]))); 
    }
    f.close(); 
    vector<Point2d> zref;
    vector<Point2d> Zref;
    for (int j=0;j<sContour.size();j++)
        zref.push_back(sContour[(j*10)%sContour.size()]);
    dft(zref,Zref,DFT_SCALE|DFT_REAL_OUTPUT);
     
    */
    //This would be the reference contour    
    md.sContour=Z; //Zref;
    md.nbDesFit=20;
    float alpha,phi,s;

    md.AjustementRtSafe(Z,alpha,phi,s);
    complex<float> expitheta=s*complex<float>(cos(phi), sin(phi));
    cout<<"Fourier distance " << md.Distance(expitheta,alpha) << endl;
    for (int j=1;j<Z.size();j++)
    {
        //complex<float> zr(Zref[j].x,Zref[j].y);
        complex<float> zr(Z[j].x,Z[j].y);
        zr= zr*expitheta*exp(alpha*md.frequence[j]*complex<float>(0,1));
        Z[j].x = zr.real();
        Z[j].y = zr.imag();
    }
    dft(Z,z,DFT_INVERSE);
    for (int j = 0; j<z.size();j++)
        fourier.push_back(Point(z[j].x, z[j].y));
  
    Point center(0,0);
    for( int i = 0; i < fourier.size(); i++ )
        center += fourier[i];
    center.x /= fourier.size();
    center.y /= fourier.size();
    std::sort(fourier.begin()+4, fourier.end(),[center](Point i, Point j){ return myLess(i,j,center); });
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


/*************************DEBUG FUNCTIONS**********************************************************************/

void writeToFile(fstream &fp, const char* label, vector<Point> &fourier)
{
#ifdef debug
    for(int i = 0; i < fourier.size(); i++)
    {
        Point p = fourier[i];
        fp << p.x << "," << p.y << ",";
    }
    fp << label << "\n"; 
#endif
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

Mat merge(Mat &img1, Mat &img2)
{
    Size sz1 = img1.size();
    Size sz2 = img2.size();
    Mat img3(sz1.height, sz1.width+sz2.width, CV_8UC3);
    Mat left(img3, Rect(0, 0, sz1.width, sz1.height));
    img1.copyTo(left);
    Mat right(img3, Rect(sz1.width, 0, sz2.width, sz2.height));
    img2.copyTo(right);
    return img3;    
} 
