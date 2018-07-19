package com.qlik.sudoku;

import android.app.Activity;
import android.util.Log;
import android.os.Bundle;

public class AppActivity extends Activity
{
    private static final String TAG = "AppActivity";
    private AppView mView = null;

    @Override
    protected void onCreate(Bundle icicle) {
        Log.d(TAG, "onCreate()");

        super.onCreate(icicle);

        if (mView == null) {
            mView = new AppView(this, getApplication());
            setContentView(mView);
        }
    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestroy()");

        super.onDestroy();
    }

    @Override
    protected void onRestart() {
        Log.d(TAG, "onRestart()");

        super.onRestart();
    }

    @Override
    protected void onStart() {
        Log.d(TAG, "onStart()");

        super.onStart();
    }

    @Override
    protected void onStop() {
        Log.d(TAG, "onStop()");

        super.onStop();
        mView.onStop();
    }

    @Override
    protected void onPause() {
        Log.d(TAG, "onPause()");

        super.onPause();
        mView.onPause();
    }

    @Override
    protected void onResume() {
        Log.d(TAG, "onResume()");

        super.onResume();
        mView.onResume();
    }

    @Override
    public void onBackPressed() {
        if (mView != null)
            mView.onKeyBack();
    }
}
