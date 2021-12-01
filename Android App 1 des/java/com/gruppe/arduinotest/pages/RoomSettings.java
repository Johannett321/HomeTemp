package com.gruppe.arduinotest.pages;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.gruppe.arduinotest.R;
import com.gruppe.arduinotest.RoomManager;

import java.util.ArrayList;

import no.hometemp.api.AnswerReceivedListener;
import no.hometemp.api.CommandReqAnswer;
import no.hometemp.api.HomeTempDevice;

public class RoomSettings extends Activity {

    ListView listView;
    BaseAdapter listAdapter;
    ArrayList<SettingsItem> settingsItems;

    HomeTempDevice homeTempDevice;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.room_settings);
        findViews();

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                settingsItems.get(position).getItemPressedListener().onItemPressed();
            }
        });

        addSettings();

        listAdapter = new MyAdapter(this, settingsItems);
        listView.setAdapter(listAdapter);
    }

    private void findViews() {
        listView = findViewById(R.id.listView);
        homeTempDevice = RoomManager.getRoom(getIntent().getStringExtra("roomID"));
    }

    private void addSettings() {
        settingsItems = new ArrayList<>();
        settingsItems.add(new SettingsItem("Endre navn", new ItemPressedListener() {
            @Override
            public void onItemPressed() {
                AlertDialog.Builder builder = new AlertDialog.Builder(RoomSettings.this);
                builder.setTitle("Endre navn");
                builder.show();
            }
        }));

        settingsItems.add(new SettingsItem("Endre ikon", new ItemPressedListener() {
            @Override
            public void onItemPressed() {
                Log.e("YUP", "Change icon");
            }
        }));

        settingsItems.add(new SettingsItem("Endre frekvens", new ItemPressedListener() {
            @Override
            public void onItemPressed() {
                AlertDialog.Builder builder = new AlertDialog.Builder(RoomSettings.this);
                builder.setTitle("Endre frekvens");
                final String[] items = new String[]{"1 minutt", "2 minutter", "5 minutter", "10 minutter", "60 minutter"};
                builder.setSingleChoiceItems(items, 3, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(final DialogInterface dialog, int which) {
                        String[] minutesString = items[which].split(" ");
                        final int minutes = Integer.parseInt(minutesString[0]);
                        Thread thread = new Thread(new Runnable() {
                            @Override
                            public void run() {
                                homeTempDevice.setRefreshFreq(minutes, new AnswerReceivedListener() {
                                    @Override
                                    public void receivedAnswer(CommandReqAnswer commandReqAnswer) {
                                        Log.e("DONE", "SUCCESS");
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                dialog.dismiss();
                                                Toast.makeText(RoomSettings.this, "Suksess!", Toast.LENGTH_SHORT).show();
                                            }
                                        });
                                    }
                                });
                            }
                        });
                        thread.start();
                    }
                });
                builder.show();
            }
        }));
    }

    private interface ItemPressedListener {
        void onItemPressed();
    }

    private class SettingsItem {
        private String title;
        private ItemPressedListener itemPressedListener;

        public SettingsItem(String title, ItemPressedListener itemPressedListener) {
            this.title = title;
            this.itemPressedListener = itemPressedListener;
        }

        public String getTitle() {
            return title;
        }

        public ItemPressedListener getItemPressedListener() {
            return itemPressedListener;
        }
    }

    private class MyAdapter extends BaseAdapter {

        private Context context;
        private ArrayList<SettingsItem> items;

        public MyAdapter(Context context, ArrayList<SettingsItem> items) {
            this.context = context;
            this.items = items;
        }

        @Override
        public int getCount() {
            return items.size();
        }

        @Override
        public Object getItem(int position) {
            return items.get(position);
        }

        @Override
        public long getItemId(int position) {
            return 0;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            View view = LayoutInflater.from(context).inflate(R.layout.settings_item, parent, false);
            TextView title = view.findViewById(R.id.settingsTitle);
            title.setText(items.get(position).getTitle());
            return view;
        }
    }
}
