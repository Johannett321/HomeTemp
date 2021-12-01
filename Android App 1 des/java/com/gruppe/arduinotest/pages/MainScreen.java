package com.gruppe.arduinotest.pages;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.gruppe.arduinotest.R;
import com.gruppe.arduinotest.RoomManager;
import com.gruppe.arduinotest.pages.configuration.SearchingForHomeTemp;

import java.text.DecimalFormat;
import java.util.ArrayList;

import no.hometemp.api.ConEstablishedListener;
import no.hometemp.api.HomeTempDevice;
import no.hometemp.api.HomeTempManager;
import no.hometemp.api.TempDataReceivedListener;

public class MainScreen extends Activity {

    private GridView roomList;

    private GridAdapter gridAdapter;

    private TextView homeTemp;

    public static ProgressBar progressBar;
    public static Activity activity;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViews();

        gridAdapter = new GridAdapter();
        roomList.setAdapter(gridAdapter);

        roomList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                Intent intent = new Intent(MainScreen.this, RoomInfo.class);
                intent.putExtra("roomID", RoomManager.getListOfRooms().get(i).getDeviceID());
                startActivity(intent);
            }
        });

        homeTemp.setText("...");

        getDevicesData();
    }

    private void findViews() {
        homeTemp = findViewById(R.id.homeTemp);

        roomList = findViewById(R.id.roomList);

        progressBar = findViewById(R.id.progressBar);
        activity = this;
    }

    private void getDevicesData() {
        scheduleCheckIfAnyDevicesFound();
        requestRoomTemperatureData();
    }

    private void scheduleCheckIfAnyDevicesFound() {
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(10000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                if (gridAdapter.getCount() == 0) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            foundNoDevies();
                        }
                    });
                }
            }
        });
        thread.start();
    }

    private void foundNoDevies() {
        final AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Finner ingen HomeTemp enheter");
        builder.setMessage("Vil du konfigurere en ny en?");
        builder.setPositiveButton("Konfigurer", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Intent intent = new Intent(MainScreen.this, SearchingForHomeTemp.class);
                startActivity(intent);
            }
        });
        builder.setNegativeButton("Nei", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
        builder.show();
    }

    private void requestRoomTemperatureData() {
        progressBar.setVisibility(View.VISIBLE);
        RoomManager.searchForDevicesOnNetwork(new ConEstablishedListener() {
            @Override
            public void connectionEstablished() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        gridAdapter.notifyDataSetChanged();
                    }
                });
            }
        });
    }

    public void leggTilEnhetPressed(View view) {
        Intent intent = new Intent(this, SearchingForHomeTemp.class);
        startActivity(intent);
    }

    float totalTemp;

    class GridAdapter extends BaseAdapter {

        float[] temperatures = new float[1000];;

        @Override
        public int getCount() {
            if (RoomManager.getListOfRooms() != null) {
                return RoomManager.getListOfRooms().size();
            }
            return 0;
        }

        @Override
        public Object getItem(int i) {
            return temperatures[i];
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(final int i, View view, ViewGroup viewGroup) {
            View cellView = LayoutInflater.from(MainScreen.this).inflate(R.layout.room_grid_cell, viewGroup, false);
            TextView roomTitle = cellView.findViewById(R.id.roomTitle);
            final TextView roomTemp = cellView.findViewById(R.id.roomTemp);
            ImageView roomIcon = cellView.findViewById(R.id.roomIcon);

            roomTitle.setText(RoomManager.getListOfRooms().get(i).getDeviceName());
            roomIcon.setImageDrawable(IconManager.getIconFromName(MainScreen.this, RoomManager.getListOfRooms().get(i).getDeviceName()));

            roomTemp.setText("...");
            Thread thread = new Thread(new Runnable() {
                @Override
                public void run() {
                    RoomManager.getListOfRooms().get(i).getTempData(new TempDataReceivedListener() {
                        @Override
                        public void onDataReceived(final float temp, float humid) {
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    roomTemp.setText(temp + "°");
                                    temperatures[i] = temp;
                                    updateHouseTemp();
                                }
                            });
                        }
                    });
                }
            });
            thread.start();
            return cellView;
        }
    }

    private void updateHouseTemp() {
        totalTemp = 0;
        for (int i = 0; i < gridAdapter.getCount(); i++) {
            totalTemp += (float) gridAdapter.getItem(i);
        }

        Float average = totalTemp/RoomManager.getListOfRooms().size();
        DecimalFormat df = new DecimalFormat("#.#");
        homeTemp.setText(df.format(average) + "°");
    }
}