#include <jni.h>
#include <sstream>
#include <string>
#include <stdio.h>

#include <android/log.h>

#include "utils.cpp"
#include "amanda.cpp"


extern "C" {

// Process a frame
JNIEXPORT jboolean JNICALL
Java_com_example_danyalejandro_trywithndk_MainActivity_processFrame(JNIEnv *env, jobject instance,
																	jlong iAddr1, jlong iAddr2) {

	// Prepare source and destination image pointers
	Mat *src = (Mat *) iAddr1;
	Mat *dst = (Mat *) iAddr2;
	flip(*src, *src, 1);
	//__android_log_print(ANDROID_LOG_ERROR, "MyLogs", "GlobalN address: %p", &globalN);

	//cropImage(*src, *dst);

	return checkIfCorrect(*src, 'C');
}

// Assigns values to configuration globals
JNIEXPORT void JNICALL
Java_com_example_danyalejandro_trywithndk_MainActivity_initGlobals(JNIEnv *env, jobject instance,
																   jstring externalStoragePath) {

	sign_cascade_folder = jstring2string(env, externalStoragePath) + "/signcoach/data/";
	__android_log_print(ANDROID_LOG_ERROR, "initGlobals", "sign_cascade_folder: %s", sign_cascade_folder.c_str());

	FILE* file = fopen("/sdcard/textTest.txt","w+");
	if (file == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "initGlobals", "Error fopen: %d (%s)", errno, strerror(errno));
	} else {
		__android_log_print(ANDROID_LOG_ERROR, "initGlobals", "File open!");
	}
	fputs("hello, world\n", file);
	fclose(file);
}

}