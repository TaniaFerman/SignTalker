package com.danycabrera.signcoach;

import android.Manifest;
import android.app.Activity;
import android.app.Application;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.drawable.Drawable;
import android.media.Image;
import android.preference.PreferenceManager;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.NavUtils;
import android.util.Log;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewStub;
import android.widget.TextView;
import android.widget.ImageView;
import android.widget.ViewFlipper;
import android.graphics.drawable.Drawable;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.SurfaceView;
import android.view.Menu;
import org.opencv.android.JavaCameraView;
import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.w3c.dom.Text;

import fragment.OptionsMenuFragment;

import junit.framework.Test;

public class LearnActivity extends AppCompatActivity implements CvCameraViewListener2, NavigationView.OnNavigationItemSelectedListener {
    private static final String TAG = "OCV:LearnActivity";
    private static final String question_string = "Show me a", lesson_string = "This is a";
    private static final String vowels = "AEFHILMNORSX";    //Letters which should be preceded by 'an'
    private TextView tv_question, tv_lesson;
    private ImageView iv_lesson;
    private ViewFlipper viewFlipper;
    private static int counter = 0;
    private TestManager testManager;
    private LearnMessage current_message;
    boolean camera_setting, handed_setting;
    // Loads camera view of OpenCV for us to use. This lets us see using OpenCV
    private CameraBridgeViewBase mOpenCvCameraView;
    Mat mGray;
    private static boolean camera_default, handed_default;
    private SharedPreferences prefs;
    private Mat img; // Processed image
    private Mat imgOriginal; // Original image
    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS_STORAGE = {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_learn);

        //---------Do drawer stuff-----------------------
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        ViewStub stub = (ViewStub) findViewById(R.id.content_stub);
        stub.setLayoutResource(R.layout.content_learn);
        stub.inflate();
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.addDrawerListener(toggle);
        toggle.syncState();
        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);
        //-----------------------------------------------

        //--------Add options menu fragment-------------------
        Fragment opt_frag = getFragmentManager().findFragmentByTag("options_menu");
        if (opt_frag == null) {
            opt_frag = new OptionsMenuFragment();
            FragmentTransaction trans = getFragmentManager().beginTransaction();
            trans.add(opt_frag, "options_menu");
            trans.commit();
        }
        //-----------------------------------------------------

        // current_char = "A";
        tv_question = (TextView) findViewById(R.id.tv_question); //Handles for the two messages we'll be changing
        tv_lesson = (TextView) findViewById(R.id.tv_lesson);
        iv_lesson = (ImageView) findViewById(R.id.iv_lesson);
        viewFlipper = (ViewFlipper) findViewById(R.id.learn_flipper);
        viewFlipper.setInAnimation(this, R.anim.slide_in);
        viewFlipper.setOutAnimation(this, R.anim.slide_out);
        testManager = new TestManager();
        nextQuestion(null);
        //   setMessage();
        mOpenCvCameraView = (JavaCameraView) findViewById(R.id.show_camera_activity_java_surface_view);
        getCurrentPreferences();
        setCamera();
        setTitle("");
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setLayoutDirection(View.LAYOUT_DIRECTION_INHERIT);
        mOpenCvCameraView.setCvCameraViewListener(this);
    }

    private void updateCount(boolean val) {
        if (val) counter += 2;
        else {
            if (counter > 0) counter--;
        }
    }

    private void setMessage() {
        if (current_message.isLesson()) {
            tv_lesson.setText(doGrammarCat(lesson_string, current_message.getString()));
            iv_lesson.setImageDrawable(findDrawable(current_message.getString()));
            viewFlipper.setDisplayedChild(viewFlipper.indexOfChild(findViewById(R.id.lesson_view)));
        } else {
            tv_question.setText(doGrammarCat(question_string, current_message.getString()));
            viewFlipper.setDisplayedChild(viewFlipper.indexOfChild(findViewById(R.id.question_view)));
        }
    }

    private Drawable findDrawable(String sign) {
        switch (sign) {
            case "A":
                return getResources().getDrawable(R.drawable.a);
            case "B":
                return getResources().getDrawable(R.drawable.b);
            case "C":
                return getResources().getDrawable(R.drawable.c);
            case "D":
                return getResources().getDrawable(R.drawable.d);
            case "E":
                return getResources().getDrawable(R.drawable.e);
            case "F":
                return getResources().getDrawable(R.drawable.f);
            case "G":
                return getResources().getDrawable(R.drawable.g);
            case "H":
                return getResources().getDrawable(R.drawable.h);
            case "I":
                return getResources().getDrawable(R.drawable.i);
            case "J":
                return getResources().getDrawable(R.drawable.j);
            case "K":
                return getResources().getDrawable(R.drawable.k);
            case "L":
                return getResources().getDrawable(R.drawable.l);
            case "M":
                return getResources().getDrawable(R.drawable.m);
            case "N":
                return getResources().getDrawable(R.drawable.n);
            case "O":
                return getResources().getDrawable(R.drawable.o);
            case "P":
                return getResources().getDrawable(R.drawable.p);
            case "Q":
                return getResources().getDrawable(R.drawable.q);
            case "R":
                return getResources().getDrawable(R.drawable.r);
            case "S":
                return getResources().getDrawable(R.drawable.s);
            case "T":
                return getResources().getDrawable(R.drawable.t);
            case "U":
                return getResources().getDrawable(R.drawable.u);
            case "V":
                return getResources().getDrawable(R.drawable.v);
            case "W":
                return getResources().getDrawable(R.drawable.w);
            case "X":
                return getResources().getDrawable(R.drawable.x);
            case "Y":
                return getResources().getDrawable(R.drawable.y);
            case "Z":
                return getResources().getDrawable(R.drawable.z);
            default:
                return getResources().getDrawable(R.drawable.a);
        }
    }

    public void setCamera() {
        Log.d(TAG, "setCamera: Current camera setting is " + ((camera_setting) ? "front" : "back"));
        mOpenCvCameraView.disableView();
        if (camera_setting) mOpenCvCameraView.setCameraIndex(mOpenCvCameraView.CAMERA_ID_FRONT);
        else mOpenCvCameraView.setCameraIndex(mOpenCvCameraView.CAMERA_ID_BACK);
        mOpenCvCameraView.enableView();
    }

    public void prevQuestion(View v) {
     /*   byte[] temp = new byte[1];
        temp[0] = (byte)(current_char.getBytes()[0] - 1);
        if(temp[0] < (byte)'A'){
            temp[0] = (byte)'A';
        }
        current_char = new String(temp);
        nextQuestion();*/
    }

    //Concatenates and does appropriate grammar for provided message and character
    private String doGrammarCat(String message, String c) {
        String vowel_grammar = " ";
        if (isVowel(c)) {
            vowel_grammar = "n ";
        }
        return message + vowel_grammar + c;
    }

    public void nextQuestion(View v) {
        current_message = testManager.getNextMessage();
        if (current_message == null) {
            getNextSet();
        }
        setMessage();
        if (current_message.isLesson()) {
            moveToLessonView(null); //The sending view is not used to move views anyway, so null
        } else {
            moveToQuestionView(null);
        }
    }

    private void getNextSet() {
        //TODO: Show success screen
        if (testManager.moveToNextSet())
            current_message = testManager.getNextMessage();
    }

    private void returnResult(boolean result) {
        testManager.sendResult(result);
        if (testManager.currentSetCompleted())
            getNextSet();
        else
            nextQuestion(null);
    }

    private boolean isVowel(String c) {
        return vowels.contains(c);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        SharedPreferences.Editor pref_edit;
        switch (item.getItemId()) {
            case android.R.id.home:
                NavUtils.navigateUpFromSameTask(this);
                break;
            case R.id.camera_switch:            //This overrides the case in OptionsMenu fragment
                camera_setting = !camera_setting;
                pref_edit = prefs.edit();
                pref_edit.putBoolean(getString(R.string.camera_setting), camera_setting);
                if (!pref_edit.commit()) Log.d(TAG, "prefs not saved wtf");
                Log.d(TAG, "New setting is " + ((camera_setting) ? "front" : "back"));
                // setCamera();
                break;
            default:
                return false;
        }
        return true;
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        // Handle navigation view item clicks here.
        int id = item.getItemId();

        if (id == R.id.nav_home) {
            // Handle the camera action
            Intent intent = new Intent(this, MainActivity.class);
            startActivity(intent);
        }

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }

    @Override
    public void onBackPressed() {
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
            overridePendingTransition(R.anim.stay_still, R.anim.blow_down);
        }
    }

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS: {
                    Log.i(TAG, "OpenCV loaded successfully aaa");
                    //  setCamera();
                }
                break;
                default: {
                    super.onManagerConnected(status);
                }
                break;
            }
        }
    };

    @Override
    public void onPause() {
        Log.d(TAG, "Pausing activity. Current camera setting is " + ((camera_setting) ? "front" : "back"));
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onResume() {
        super.onResume();
        camera_setting = getCurrentCameraSetting();
        if (!OpenCVLoader.initDebug()) {
            Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
        } else {
            Log.d(TAG, "OpenCV library found inside package. Using it!");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
        getCurrentPreferences();
        Log.d(TAG, "Resuming activity. Current camera setting is " + ((camera_setting) ? "front" : "back"));
    }

    @Override
    public void onStart() {
        super.onStart();
        getCurrentPreferences();
        setCamera();
    }

    @Override
    public void onStop() {
        Log.d(TAG, "Stopping activity. Current camera setting is " + ((camera_setting) ? "front" : "back"));
        super.onStop();
    }

    public void getCurrentPreferences() {
        prefs = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        Log.d(TAG, "STARTING ACTIVITY");
        camera_default = getResources().getBoolean(R.bool.camera_default);
        handed_default = getResources().getBoolean(R.bool.handed_default);
        camera_setting = getCurrentCameraSetting();
        handed_setting = prefs.getBoolean(getString(R.string.handed_setting), handed_default);
    }

    private boolean getCurrentCameraSetting() {
        return prefs.getBoolean(getString(R.string.camera_setting), camera_default);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        //  if (mOpenCvCameraView != null)
        //     mOpenCvCameraView.disableView();
    }

    public void onCameraViewStarted(int width, int height) {
        imgOriginal = new Mat(height, width, CvType.CV_8UC4);
        img = ImageOps.initImage(width, height);

        //initGlobals(FileOps.getStoragePath());
        Log.i("onCameraViewStarted", "Initialized mat with " + img.rows() + " x " + img.cols());
        Log.i("onCameraViewStarted", "Cameraview dims: " + mOpenCvCameraView.getWidth() + " x " + mOpenCvCameraView.getHeight());
    }

    public void onCameraViewStopped() {
        imgOriginal.release();
        img.release();
    }

    private void showSuccess() {
      /*  AlertDialog alertDialog = new AlertDialog.Builder(this).create();
        alertDialog.setTitle("Noice");
        alertDialog.setMessage("You are good");
        alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, "OK",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                });
        alertDialog.show();*/
    }

    public void fakeSuccess(View v) {
        returnResult(true);
    }

    public void fakeFailure(View v) {
        returnResult(false);
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
        imgOriginal = inputFrame.rgba();
        // Process and return frame

        boolean r = processFrame(imgOriginal.getNativeObjAddr(), img.getNativeObjAddr(), (char) current_message.getString().getBytes()[0]);
        if (r) {
            Log.i("onCameraFrame", "This is an C!");
        } else {
            Log.i("onCameraFrame", "NOT C...");
        }
        updateCount(r);
        if (counter > 5) {
            counter = 0;
            Log.i(TAG, "SUCCESSSSSSSSSSSSS");
            showSuccess();
        }
        return imgOriginal;
    }

    public void moveToLessonView(View v) {
        viewFlipper.setDisplayedChild(viewFlipper.indexOfChild(findViewById(R.id.lesson_view)));
    }

    public void moveToQuestionView(View v) {
        viewFlipper.setDisplayedChild(viewFlipper.indexOfChild(findViewById(R.id.question_view)));
    }

    public static native boolean processFrame(long iAddr1, long iAddr2, char c);
}