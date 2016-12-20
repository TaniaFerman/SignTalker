#include <jni.h>
#include <sstream>
#include <string>
#include <stdio.h>

#include <android/log.h>

#include "utils.cpp"
#include "amanda.cpp"


int dany_c = 0;

extern "C" {

// Process a frame
//JNIEXPORT jboolean JNICALL
JNIEXPORT jfloat JNICALL
Java_com_danycabrera_signcoach_LearnActivity_processFrame(JNIEnv *env, jobject instance,
																	jlong iAddr1, jlong iAddr2, jchar c) {

	// Prepare source and destination image pointers
	Mat *src = (Mat *) iAddr1;
	Mat *dst = (Mat *) iAddr2;

	flip(*src, *src, 1);
	rotate_90n(*src, *src, 90);

	// SAVE DEMO IMAGE AFTER 30 FRAMES
	/*
	if (dany_c < 30) {
		dany_c++;
	} else if (dany_c == 30) {
		dany_c++;
		Mat png;
		cvtColor(*src, png, CV_RGBA2BGR);
		imwrite(sign_cascade_folder + "check.png", png);
		__android_log_print(ANDROID_LOG_ERROR, "processFrame", "Image written!");
	}
	*/

	float result = checkIfCorrect(*src, c);
	flip(*src, *src, 1);
	Size sz = dst->size();
	cv::resize(*src, *dst, sz);

	return result;
}

// Assigns values to configuration globals
JNIEXPORT void JNICALL
Java_com_danycabrera_signcoach_MainActivity_initGlobals(JNIEnv *env, jobject instance, jstring externalStoragePath) {
	sign_cascade_folder = jstring2string(env, externalStoragePath) + "/";
	__android_log_print(ANDROID_LOG_ERROR, "MyLogs", "sign_cascade_folder: %s", sign_cascade_folder.c_str());
}

}