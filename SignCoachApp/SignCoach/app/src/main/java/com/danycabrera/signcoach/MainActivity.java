package com.danycabrera.signcoach;

import android.Manifest;
import android.app.Activity;
import android.app.FragmentManager;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.support.design.widget.NavigationView;
import android.support.v4.app.ActivityCompat;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.content.Intent;
import android.view.ViewStub;
import android.content.SharedPreferences;
import android.app.DialogFragment;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;

public class MainActivity extends AppCompatActivity implements FragmentManager.OnBackStackChangedListener, NavigationView.OnNavigationItemSelectedListener {
    public SharedPreferences prefs;

	static {
		System.loadLibrary("native-lib");
	}

    public MainActivity() {

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ViewStub stub = (ViewStub) findViewById(R.id.content_stub);
        stub.setLayoutResource(R.layout.content_main);
        stub.inflate();

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.addDrawerListener(toggle);
        toggle.syncState();
        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.getMenu().findItem(R.id.nav_home).setChecked(true);
        navigationView.setNavigationItemSelectedListener(this);

        //Fragment opt_frag = getFragmentManager().findFragmentByTag("options_menu");
        setTitle("SignCoach");
        /*if(opt_frag == null) {
            opt_frag = new OptionsMenuFragment();
            FragmentTransaction trans = getFragmentManager().beginTransaction();
            trans.add(opt_frag, "options_menu");
            trans.commit();
        }*/
        prefs = getSharedPreferences(getString(R.string.prefs_file), MODE_PRIVATE);

		// initialize FileOps and copy datasets
		if (FileOps.init(this)) {
			if (FileOps.copyDatasetFiles(this)) {
				initGlobals(FileOps.getDatasetPath());
			} else {
				Log.e("MainActivity", "Couldn't copy dataset files to storage.");
			}
		}
    }


    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        // Handle navigation view item clicks here.
        int id = item.getItemId();

        if (id == R.id.nav_home) {
            //Intent intent = new Intent(this, MainActivity.class);
           // startActivity(intent);
        }
        else if(id == R.id.send_feedback){
            Intent intent = new Intent(Intent.ACTION_SEND);
            intent.setType("plain/text");
            intent.putExtra(Intent.EXTRA_EMAIL, new String[] { "signcoachasl@gmail.com" });
            intent.putExtra(Intent.EXTRA_SUBJECT, "RE: SignCoach Feedback");
            startActivity(Intent.createChooser(intent, ""));
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
        }
    }

    @Override
    public void onBackStackChanged() {
        shouldDisplayHomeUp();
    }

    public void shouldDisplayHomeUp(){
        //Enable Up button only  if there are entries in the back stack
        boolean canback = getSupportFragmentManager().getBackStackEntryCount()>0;
        getSupportActionBar().setDisplayHomeAsUpEnabled(canback);
    }

    @Override
    public void onResume() {
        prefs = getSharedPreferences(getString(R.string.prefs_file), MODE_PRIVATE);
        super.onResume();
    }
    public void learnClick(View v){
        int hand = prefs.getInt(getString(R.string.handed_setting), -1);
        //If the user has not chosen their preferred hand, request it
        if(hand == -1){
            DialogFragment dialog = new RequestDialog();
            dialog.show(getFragmentManager(), "handed_dialog");
        }
        else{
            Intent intent = new Intent(this, LearnActivity.class);
            startActivity(intent);
            overridePendingTransition(R.anim.blow_up, R.anim.stay_still);
        }
    }
    public static native void initGlobals(String externalStoragePath);

}
