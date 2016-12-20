#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;


// Returns the C++ equivalent of a jstring
std::string jstring2string(JNIEnv *env, jstring s) {
	const char *nativeString = env->GetStringUTFChars(s, JNI_FALSE);
	return string(nativeString, strlen(nativeString));
}

// Crop image as a square with the minimal side size
void cropImage(Mat &src, Mat &dst) {
	int w = src.cols;
	int h = src.rows;
	int x, y, side;

	__android_log_print(ANDROID_LOG_ERROR, "cropImage", "Received Cols: %d Rows: %d.", w, h);

	if (w > h) {
		side = h;
		x = (w - h) / 2;
		y = 0;
	} else {
		side = w;
		y = (h - w) / 2;
		x = 0;
	}

	Mat ROI = src(Rect(x, y, side, side));
	ROI.copyTo(dst);
}

/*
 *@brief rotate image by multiple of 90 degrees
 *
 *@param source : input image
 *@param dst : output image
 *@param angle : factor of 90, even it is not factor of 90, the angle
 * will be mapped to the range of [-360, 360].
 */
void rotate_90n(cv::Mat const &src, cv::Mat &dst, int angle) {
	CV_Assert(angle % 90 == 0 && angle <= 360 && angle >= -360);
	if(angle == 270 || angle == -90){
		// Rotate clockwise 270 degrees
		cv::transpose(src, dst);
		cv::flip(dst, dst, 0);
	}else if(angle == 180 || angle == -180){
		// Rotate clockwise 180 degrees
		cv::flip(src, dst, -1);
	}else if(angle == 90 || angle == -270){
		// Rotate clockwise 90 degrees
		cv::transpose(src, dst);
		cv::flip(dst, dst, 1);
	}else if(angle == 360 || angle == 0 || angle == -360){
		if(src.data != dst.data){
			src.copyTo(dst);
		}
	}
}