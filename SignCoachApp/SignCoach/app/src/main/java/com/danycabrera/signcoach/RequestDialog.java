package com.danycabrera.signcoach;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.app.DialogFragment;
import android.app.Dialog;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;



public class RequestDialog extends DialogFragment {

    private boolean cancel;
    public RequestDialog() {
        // Required empty public constructor
    }
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        // Use the Builder class for convenient dialog construction
        cancel = false;
        final SharedPreferences prefs = this.getActivity().getSharedPreferences(getString(R.string.prefs_file), Context.MODE_PRIVATE);
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setMessage(R.string.handed_message)
                .setPositiveButton(R.string.handed_left, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        // FIRE ZE MISSILES!
                        SharedPreferences.Editor edit = prefs.edit();
                        edit.putInt(getString(R.string.handed_setting), 1);
                        edit.commit();
                    }
                })
                .setNegativeButton(R.string.handed_right, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        SharedPreferences.Editor edit = prefs.edit();
                        edit.putInt(getString(R.string.handed_setting), 0);
                        edit.commit();
                    }
                })
                .setNeutralButton(R.string.cancel, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        //Cancel
                        cancel = true;
                    }
                });
        // Create the AlertDialog object and return it
        return builder.create();
    }
    @Override
    public void onDismiss(DialogInterface dialog){
        if(!cancel){
            Intent intent = new Intent(getActivity(), LearnActivity.class);
            startActivity(intent);
            getActivity().overridePendingTransition(R.anim.blow_up, R.anim.stay_still);
        }
        super.onDismiss(dialog);
    }


}
