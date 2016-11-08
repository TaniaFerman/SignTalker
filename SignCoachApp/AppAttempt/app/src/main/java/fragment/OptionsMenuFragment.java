package fragment;

import android.content.Context;
import android.os.Bundle;
import android.app.Fragment;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.support.v4.app.NavUtils;
import android.view.MenuInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import com.example.tristan.appattempt.R;
import android.util.Log;

import java.lang.Boolean;

/**
 * A simple {@link Fragment} subclass.
 */
public class OptionsMenuFragment extends Fragment {
    //private static final String PREF_FILE = "PrefFile";   //Not needed?
    private static final String TAG = "OptMenuFrag::Debug";
    private static SharedPreferences prefs;
    private MenuItem camera_item, handed_item;
    private static boolean camera_setting, handed_setting, camera_default, handed_default;
    public OptionsMenuFragment() {
        // Required empty public constructor
    }
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        prefs = PreferenceManager.getDefaultSharedPreferences(getActivity().getApplicationContext());
        SharedPreferences.Editor pref_edit = prefs.edit();
        camera_setting = getResources().getBoolean(R.bool.camera_default);
        camera_default = camera_setting;
        pref_edit.putBoolean(getString(R.string.camera_setting), camera_setting);
        handed_setting = getResources().getBoolean(R.bool.handed_default);
        handed_default = handed_setting;
        pref_edit.putBoolean(getString(R.string.handed_setting), handed_setting);
        pref_edit.apply();
        setHasOptionsMenu(true);
        Log.d(TAG, "Fragment created");
    }
    @Override
    public boolean onOptionsItemSelected(MenuItem item){
        boolean previous_value;
        SharedPreferences.Editor pref_edit = prefs.edit();
        switch (item.getItemId()) {
            case R.id.camera_switch:
                camera_setting = !camera_setting;
                previous_value = prefs.getBoolean(getString(R.string.camera_setting), camera_default);
                if(previous_value) Log.d(TAG,"Previous hand setting was front");
                else Log.d(TAG,"Previous hand setting was back");
                pref_edit.putBoolean(getString(R.string.camera_setting), camera_setting);
                pref_edit.commit();
                return true;
            case R.id.handed_switch:
                handed_setting = !handed_setting;
                previous_value = prefs.getBoolean(getString(R.string.handed_setting), handed_default);
                if(previous_value) Log.d(TAG,"Previous hand setting was right");
                else Log.d(TAG,"Previous hand setting was left");
                pref_edit.putBoolean(getString(R.string.handed_setting), handed_setting);
                pref_edit.commit();
                return true;
            default:
                break;
        }
        return super.onOptionsItemSelected(item);
    }
    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater){
        // Inflate the layout for this fragment
        inflater.inflate(R.menu.options, menu);
        super.onCreateOptionsMenu(menu, inflater);
        camera_item = menu.findItem(R.id.camera_switch);
        handed_item = menu.findItem(R.id.handed_switch);
        Log.d(TAG, "Menu Inflate");
    }

}
