package com.danycabrera.signcoach;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.JavaCameraView;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import java.util.ArrayList;

// Wraps OpenCV Camera functionality
public class CameraWrapper implements CameraBridgeViewBase.CvCameraViewListener2 {
	private CustomCameraView camView;
	public Mat imgInput;
	public Mat imgOutput;
	public char currentCharacter;
	private Activity parentActivity;
	private Counter counter; // Reference to activity's counter

	/* *************** USEFUL FUNCTIONS *************** */

	// Run this for each frame
	public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
		imgInput = inputFrame.rgba();
		float r = LearnActivity.processFrame(imgInput.getNativeObjAddr(), imgOutput.getNativeObjAddr(), currentCharacter);
		Log.i("onCameraFrame", "character " + currentCharacter + ", r = " + r);

		if (r > 0.9) {
			counter.trueCount++;
			Log.i("onCameraFrame", "trueCount++");
		} else {
			counter.falseCount++;
			Log.i("onCameraFrame", "falseCount++");
		}
		counter.updateCount(false);

		return imgOutput;
	}

	// As soon as the camera starts filming
	public void onCameraViewStarted(int width, int height) {
		imgInput = new Mat(height, width, CvType.CV_8UC4);
		imgOutput = new Mat(height, width, CvType.CV_8UC4);
	}

	// When the camera stops filming
	public void onCameraViewStopped() {
		imgOutput.release();
		imgInput.release();
	}

	// Switch flashlight
	public void switchFlashLight() {
		camView.switchFlashLight();;
	}



	/* *************** GTFO DONT MESS W THESE FUNCTIONS *************** */

	public void init(JavaCameraView surface, Activity a, Counter c) {
		parentActivity = a;
		counter = c;

		camView = (CustomCameraView) surface;
		camView.setVisibility(SurfaceView.VISIBLE);
		camView.setCvCameraViewListener(this);

		// Check for camera permissions
		int permission = ContextCompat.checkSelfPermission(parentActivity, Manifest.permission.CAMERA);
		if (permission != PackageManager.PERMISSION_GRANTED) {
			ActivityCompat.requestPermissions(parentActivity,
					new String[]{
							Manifest.permission.CAMERA,
							Manifest.permission.READ_EXTERNAL_STORAGE,
							Manifest.permission.WRITE_EXTERNAL_STORAGE
					},
					1);
		}
	}

	public void enable() {
		camView.enableView();
	}
	public void addTouchables(ArrayList<View> touchables) {
		camView.addTouchables(touchables);
	}
	public void pause() {
		if (camView != null)
			camView.disableView();
	}
	public void disable() {
		if (camView != null)
			camView.disableView();
	}
}
