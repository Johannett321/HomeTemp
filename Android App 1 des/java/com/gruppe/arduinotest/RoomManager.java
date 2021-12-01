package com.gruppe.arduinotest;

import android.util.Log;
import android.view.View;

import com.gruppe.arduinotest.pages.MainScreen;
import com.stealthcopter.networktools.SubnetDevices;
import com.stealthcopter.networktools.subnet.Device;

import java.util.ArrayList;

import no.hometemp.api.AnswerReceivedListener;
import no.hometemp.api.ConEstablishedListener;
import no.hometemp.api.HomeTempDevice;
import no.hometemp.api.HomeTempManager;
import no.hometemp.api.TempDataReceivedListener;

public class RoomManager {
    private static ArrayList<HomeTempDevice> listOfDevices;

    public static void searchForDevicesOnNetwork(final ConEstablishedListener conEstablishedListener) {
        listOfDevices = new ArrayList<>();

        SubnetDevices.fromLocalAddress().setTimeOutMillis(10000).findDevices(new SubnetDevices.OnSubnetDeviceFound() {
            @Override
            public void onDeviceFound(Device device) {
                Log.e("DEVICE", "Found device with IP: " + device.ip);
                final HomeTempDevice homeTempDevice = HomeTempManager.getDevice(device.ip, 80);
                homeTempDevice.establishConnection(new ConEstablishedListener() {
                    @Override
                    public void connectionEstablished() {
                        listOfDevices.add(homeTempDevice);
                        conEstablishedListener.connectionEstablished();
                    }
                });
            }

            @Override
            public void onFinished(ArrayList<Device> devicesFound) {
                Log.e("DONE", "Done scanning network...");
                MainScreen.activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        MainScreen.progressBar.setVisibility(View.GONE);
                    }
                });


                /*
                final HomeTempDevice homeTempDevice = HomeTempManager.getDevice("172.20.10.9", 80);
                homeTempDevice.establishConnection(new ConEstablishedListener() {
                    @Override
                    public void connectionEstablished() {
                        listOfDevices.add(homeTempDevice);
                        conEstablishedListener.connectionEstablished();
                    }
                });
                 */
            }
        });
    }

    public static ArrayList<HomeTempDevice> getListOfRooms() {
        return listOfDevices;
    }

    public static HomeTempDevice getRoom(String roomID) {
        for (int i = 0; i < listOfDevices.size(); i++) {
            if (listOfDevices.get(i).getDeviceID().equals(roomID)) {
                return listOfDevices.get(i);
            }
        }
        return null;
    }
}
