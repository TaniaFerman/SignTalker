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
