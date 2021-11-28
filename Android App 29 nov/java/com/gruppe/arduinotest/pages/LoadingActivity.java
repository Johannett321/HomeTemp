package com.gruppe.arduinotest.pages;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import com.gruppe.arduinotest.R;
import com.gruppe.arduinotest.pages.configuration.SearchingForHomeTemp;

import no.hometemp.api.ConEstablishedListener;
import no.hometemp.api.HomeTempDevice;
import no.hometemp.api.HomeTempManager;

public class LoadingActivity extends Activity {

    TextView statusText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.loading_screen);

        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                gotoHome();
            }
        });
        thread.start();
    }

    private void gotoHome() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Intent intent = new Intent(LoadingActivity.this, MainScreen.class);
                startActivity(intent);
                finish();
            }
        });
    }
}
