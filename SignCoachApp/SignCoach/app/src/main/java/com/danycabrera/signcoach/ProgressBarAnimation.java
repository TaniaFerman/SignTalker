package com.danycabrera.signcoach;

import android.widget.ProgressBar;
import android.view.animation.Animation;
import android.view.animation.Transformation;

public class ProgressBarAnimation extends Animation {
    private ProgressBar progressBar;
    private int to;
    private int from;
    private long stepDuration;

    /**
     * @param fullDuration - time required to fill progress from 0% to 100%
     */
    public ProgressBarAnimation(ProgressBar progressBar, long fullDuration) {
        super();
        this.progressBar = progressBar;
        stepDuration = fullDuration / progressBar.getMax();
    }


    public void setProgress(int progress) {
        if (progress < 0) {
            progress = 0;
        }

        if (progress > progressBar.getMax()) {
            progress = progressBar.getMax();
        }

        to = progress;

        from = progressBar.getProgress();
        setDuration(Math.abs(to - from) * stepDuration);
        progressBar.startAnimation(this);
    }
    public void resetBar(){
        progressBar.setProgress(0);
    }
    @Override
    protected void applyTransformation(float interpolatedTime, Transformation t) {
        float value = from + (to - from) * interpolatedTime;
        progressBar.setProgress((int) value);
    }
}