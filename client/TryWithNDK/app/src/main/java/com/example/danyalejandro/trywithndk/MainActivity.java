package com.example.danyalejandro.trywithndk;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.WindowManager;

import com.google.android.gms.appindexing.AppIndex;
import com.google.android.gms.common.api.GoogleApiClient;

import org.opencv.android.JavaCameraView;
import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.CvType;
import org.opencv.core.Mat;


public class MainActivity extends AppCompatActivity implements CvCameraViewListener2 {
	private CameraBridgeViewBase mOpenCvCameraView;

	private Mat img; // Processed image
	private Mat imgOriginal; // Original image

	static {
		System.loadLibrary("native-lib");
	}


	public MainActivity() {
		Log.i("Constructor", "Instantiated new " + this.getClass());
	}

	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status) {
			switch (status) {
				case LoaderCallbackInterface.SUCCESS:
					Log.i("onManagerConnected", "OpenCV loaded successfully");
					mOpenCvCameraView.enableView();
					break;
				default:
					super.onManagerConnected(status);
					break;
			}
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// Activity initialization
		super.onCreate(savedInstanceState);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		setContentView(R.layout.activity_main);

		// OpencvCamera initialization
		mOpenCvCameraView = (JavaCameraView) findViewById(R.id.show_camera_activity_java_surface_view);
		mOpenCvCameraView.setCameraIndex(1);
		mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
		mOpenCvCameraView.setCvCameraViewListener(this);
	}

	@Override
	public void onPause() {
		super.onPause();
		if (mOpenCvCameraView != null)
			mOpenCvCameraView.disableView();
	}

	@Override
	public void onResume() {
		super.onResume();
		if (!OpenCVLoader.initDebug()) {
			Log.d("onResume", "Internal OpenCV library not found. Using OpenCV Manager for initialization");
			OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
		} else {
			Log.d("onResume", "OpenCV library found inside package. Using it!");
			mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
		}

	}

	public void onDestroy() {
		super.onDestroy();
		if (mOpenCvCameraView != null)
			mOpenCvCameraView.disableView();
	}

	public void onCameraViewStarted(int width, int height) {
		// Initialize Mats
		imgOriginal = new Mat(height, width, CvType.CV_8UC4);
		img = ImageOps.initImage(width, height);

		//initGlobals(FileOps.getStoragePath());
		initGlobals("/sdcard/");

		Log.i("onCameraViewStarted", "Initialized mat with " + img.rows() + " x " + img.cols());
		Log.i("onCameraViewStarted", "Cameraview dims: " + mOpenCvCameraView.getWidth() + " x " + mOpenCvCameraView.getHeight());
	}

	public void onCameraViewStopped() {
		imgOriginal.release();
		img.release();
	}

	public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
		imgOriginal = inputFrame.rgba();
		// Process and return frame

		boolean r = processFrame(imgOriginal.getNativeObjAddr(), img.getNativeObjAddr());
		if (r) {
			Log.i("onCameraFrame", "This is an A!");
		} else {
			Log.i("onCameraFrame", "NOT A...");
		}
		return imgOriginal;
	}

	// Native methods declaration
	public static native boolean processFrame(long iAddr1, long iAddr2);
	public static native void initGlobals(String externalStoragePath);
}
