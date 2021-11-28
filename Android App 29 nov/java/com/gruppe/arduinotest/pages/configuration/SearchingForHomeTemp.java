package com.gruppe.arduinotest.pages.configuration;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.widget.EditText;

import androidx.core.app.ActivityCompat;

import com.gruppe.arduinotest.R;

import java.util.List;
import java.util.Random;

import no.hometemp.api.AnswerReceivedListener;
import no.hometemp.api.CommandReqAnswer;
import no.hometemp.api.HomeTempManager;

public class SearchingForHomeTemp extends Activity {

    private EditText ssidField;
    private EditText passwordField;

    private EditText nameField;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.searching_for_hometemp);
        findViews();

        ssidField.setText("JohanH02");
        passwordField.setText("8438936427");
        nameField.setText("Stue");

        startActivity(new Intent(Settings.ACTION_WIFI_SETTINGS));
    }

    private void findViews() {
        ssidField = findViewById(R.id.ssidField);
        passwordField = findViewById(R.id.passwordField);

        nameField = findViewById(R.id.nameField);
    }

    public void configurePressed(View view) {
        Random random = new Random();
        int randomNumber = random.nextInt(100000);
        String deviceID = System.currentTimeMillis() + String.valueOf(randomNumber);

        String ssid = ssidField.getText().toString();
        String pass = passwordField.getText().toString();
        String deviceName = nameField.getText().toString();

        HomeTempManager.configureCurrentDevice(ssid,
                pass, deviceID,
                deviceName,
                new AnswerReceivedListener() {
            @Override
            public void receivedAnswer(CommandReqAnswer commandReqAnswer) {
                finish();
            }
        });
    }
}
