package no.hometemp.api;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class HomeTempDevice {
    private String ip;
    private int port;
    private String deviceName;
    private String deviceID;

    private Socket socket;

    private boolean deviceReady = false;
    private Communicator communicator;

    protected HomeTempDevice(String ip, int port) {
        this.ip = ip;
        this.port = port;
    }

    public String getIp() {
        return ip;
    }

    public int getPort() {
        return port;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public String getDeviceID() {
        return deviceID;
    }

    public boolean isConnected() {
        return socket.isConnected() && deviceReady;
    }

    public void establishConnection(ConEstablishedListener conEstablishedListener) {
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                System.out.println("Attempting to connect to device...");
                try {
                    socket = new Socket(ip, port);
                    System.out.println("Socket connection established... Creating communicator...");
                    communicator = new Communicator(socket);
                    retrieveDeviceID();

                    conEstablishedListener.connectionEstablished();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });
        thread.start();
    }

    public CommandReq sendCommand(String command) {
        return sendCommand(command, null);
    }

    public CommandReq sendCommand(String command, String parameters) {
        return communicator.createCommandRequest(command, parameters);
    }

    private void retrieveDeviceID() {
        //out.println("DEV_ID");
    }
}
