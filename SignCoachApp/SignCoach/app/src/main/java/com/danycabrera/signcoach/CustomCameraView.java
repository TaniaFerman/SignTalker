package com.danycabrera.signcoach;

import android.content.Context;
import android.hardware.Camera;
import android.util.AttributeSet;

import org.opencv.android.JavaCameraView;
import org.opencv.core.Size;

import java.util.List;

public class CustomCameraView extends JavaCameraView {
	private static boolean isFlashLightON = false;
	public CustomCameraView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public List<String> getEffectList() {
		return mCamera.getParameters().getSupportedColorEffects();
	}

	public boolean isEffectSupported() {
		return (mCamera.getParameters().getColorEffect() != null);
	}

	public String getEffect() {
		return mCamera.getParameters().getColorEffect();
	}

	public void setEffect(String effect) {
		Camera.Parameters params = mCamera.getParameters();
		params.setColorEffect(effect);
		mCamera.setParameters(params);
	}

	public List<Camera.Size> getResolutionList() {
		return mCamera.getParameters().getSupportedPreviewSizes();
	}

	public void setResolution(Size resolution) {
		disconnectCamera();
		mMaxHeight = (int)resolution.height;
		mMaxWidth = (int)resolution.width;
		connectCamera(getWidth(), getHeight());
	}

	public Camera.Size getResolution() {
		return mCamera.getParameters().getPreviewSize();
	}

	// Switch camera flashlight
	public void switchFlashLight() {
		Camera camera = mCamera;
		if (camera != null) {
			Camera.Parameters params = camera.getParameters();

			if (params != null) {
				if (isFlashLightON) {
					isFlashLightON = false;
					params.setFlashMode(Camera.Parameters.FLASH_MODE_OFF);
					camera.setParameters(params);
					camera.startPreview();
				} else {
					isFlashLightON = true;
					params.setFlashMode(Camera.Parameters.FLASH_MODE_TORCH);
					camera.setParameters(params);
					camera.startPreview();

				}
			}
		}

	}
}
