package com.example.tristan.appattempt;

import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;

/**
 * Holds Image Operations
 */

public final class ImageOps {
	public static int iniWidth;
	public static int iniHeight;
	public static Mat img;

	/*
	Calculates the square size for the Mat
	Initializes Mat variable as a square image
	 */
	public static Mat initImage(int w, int h) {
		iniWidth = w;
		iniHeight = h;
		int side = (h > w) ? w : h;
		return new Mat(w, h, CvType.CV_8UC4);
	}
}
