package com.gruppe.arduinotest.pages;

import android.animation.Animator;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.AxisBase;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.formatter.ValueFormatter;
import com.gruppe.arduinotest.R;
import com.gruppe.arduinotest.RoomManager;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;

import no.hometemp.api.HistoricalTempDataReceivedListener;
import no.hometemp.api.HomeTempDevice;
import no.hometemp.api.TempData;
import no.hometemp.api.TempDataReceivedListener;

public class RoomInfo extends Activity {

    HomeTempDevice homeTempDevice;

    TextView roomName;
    TextView roomNameLongCard;
    TextView roomTempLarge;
    TextView smallCardTempText;
    TextView smallCardHumidText;
    ImageView roomIcon;

    TextView reloadButtonText;
    ProgressBar progressBar;

    LineChart lineChart;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_room_info);

        homeTempDevice = RoomManager.getRoom(getIntent().getStringExtra("roomID"));
        homeTempDevice.getTempData(new TempDataReceivedListener() {
            @Override
            public void onDataReceived(float temperatur, float humidity) {

            }
        });

        findViews();
        fillOutData();
    }

    private void findViews() {
        roomName = findViewById(R.id.roomName);
        roomNameLongCard = findViewById(R.id.roomNameLongCard);
        roomTempLarge = findViewById(R.id.roomTempLarge);
        smallCardTempText = findViewById(R.id.smallCardTempText);
        smallCardHumidText = findViewById(R.id.smallCardHumidText);
        roomIcon = findViewById(R.id.roomIcon);

        reloadButtonText = findViewById(R.id.reloadButtonText);

        progressBar = findViewById(R.id.progressBar);
        progressBar.setVisibility(View.GONE);

        lineChart = findViewById(R.id.lineChart);
    }

    private void fillOutData() {
        progressBar.setVisibility(View.VISIBLE);
        reloadButtonText.setVisibility(View.GONE);

        roomName.setText(homeTempDevice.getDeviceName());
        roomNameLongCard.setText(homeTempDevice.getDeviceName());
        roomIcon.setImageDrawable(IconManager.getIconFromName(this, homeTempDevice.getDeviceName()));
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                homeTempDevice.getTempData(new TempDataReceivedListener() {
                    @Override
                    public void onDataReceived(final float temperature, final float humidity) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                reloadButtonText.setVisibility(View.VISIBLE);
                                progressBar.setVisibility(View.GONE);

                                roomTempLarge.setText(temperature + "??");
                                smallCardTempText.setText(temperature + "??");
                                smallCardHumidText.setText(String.valueOf(humidity) + "%");
                            }
                        });
                    }
                });
            }
        });
        thread.start();

        loadHistory();
    }

    private void loadHistory() {
        final long unixTime = System.currentTimeMillis()/1000;
        final long start = unixTime-(60*60*24);
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                homeTempDevice.getHistoricalData(start, unixTime, new HistoricalTempDataReceivedListener() {
                    @Override
                    public void onReceive(ArrayList<TempData> arrayList) {
                        final ArrayList<Entry> entries = new ArrayList<>();
                        for (int i = 0; i < arrayList.size(); i++) {
                            Entry entry = new Entry();

                            entry.setX(arrayList.get(i).getUnixTime());
                            entry.setY(arrayList.get(i).getTemperature());
                            entries.add(entry);
                        }
                        LineDataSet lineDataSet = new LineDataSet(entries, "Historikk");

                        final LineData lineData = new LineData(lineDataSet);
                        ValueFormatter valueFormatter = new ValueFormatter() {
                            @Override
                            public String getAxisLabel(float value, AxisBase axis) {
                                Calendar calendar = Calendar.getInstance();
                                calendar.setTimeInMillis((long) value*1000);

                                int hour = calendar.get(Calendar.HOUR_OF_DAY);
                                int minute = calendar.get(Calendar.MINUTE);

                                String hourString = String.valueOf(hour);
                                String minuteString = String.valueOf(minute);

                                if (hourString.length() == 1) {
                                    hourString = "0" + hourString;
                                }

                                if (minuteString.length() == 1) {
                                    minuteString = "0" + minuteString;
                                }

                                return hourString + ":" + minuteString;
                            }
                        };
                        lineData.setValueFormatter(valueFormatter);
                        lineChart.getXAxis().setValueFormatter(valueFormatter);
                        lineData.setDrawValues(false);
                        lineData.setValueTextColor(getColor(android.R.color.black));
                        lineData.setValueTextSize(12f);

                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                lineChart.setData(lineData);
                                lineChart.invalidate();
                            }
                        });
                    }
                });
            }
        });
        thread.start();
    }

    public void roomSettingsPressed(View view) {
        Intent intent = new Intent(this, RoomSettings.class);
        Log.e("INTENT", "Adding " + homeTempDevice.getDeviceID() + " to intent.");
        intent.putExtra("roomID", homeTempDevice.getDeviceID());
        startActivity(intent);
    }

    public void refreshRoom(final View view) {
        view.animate().scaleX((float) 1.1).scaleY((float) 1.1).setDuration(100).setListener(new Animator.AnimatorListener() {
            @Override
            public void onAnimationStart(Animator animation) {}
            @Override
            public void onAnimationEnd(Animator animation) {
                view.animate().scaleX((float) 1).scaleY((float) 1).setDuration(100).setListener(null).start();
            }
            @Override
            public void onAnimationCancel(Animator animation) {}
            @Override
            public void onAnimationRepeat(Animator animation) {}
        }).start();
        fillOutData();
    }
}
