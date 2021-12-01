package no.hometemp.api;

import java.io.IOException;
import java.net.Socket;
import java.util.ArrayList;

public class HomeTempDevice {
    private String ip;
    private int port;
    private String deviceName;
    private String deviceID;

    private Socket socket;

    private boolean deviceReady = false;
    private Communicator communicator;

    private float roomTemp;
    private float roomHumidity;
    private long lastUpdate;

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

                    sendCommand("deviceInfo").setAnswerReceivedListener(new AnswerReceivedListener() {
                        @Override
                        public void receivedAnswer(CommandReqAnswer commandReqAns) {
                            if (!commandReqAns.getAnswer().equals("null")) {
                                deviceID = commandReqAns.getAnswer().split(",")[0];
                                deviceName = commandReqAns.getAnswer().split(",")[1];

                                conEstablishedListener.connectionEstablished();
                            }
                        }
                    });
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

    protected void sendConfigInfo(String ssid, String pass, String deviceID, String deviceName, AnswerReceivedListener answerReceivedListener) {
        sendCommand("configure", ssid + "," + pass + "," + deviceName + "," + deviceID + "," + String.valueOf(System.currentTimeMillis()/1000)).setAnswerReceivedListener(answerReceivedListener);
    }

    /*-----------------------------COMMANDS-----------------------------*/

    public void getTempData(TempDataReceivedListener tempDataReceived) {
        if (System.currentTimeMillis() - lastUpdate > 1000*60) {
            sendCommand("CURRENT_TEMP").setAnswerReceivedListener(new AnswerReceivedListener() {
                @Override
                public void receivedAnswer(CommandReqAnswer commandReqAns) {
                    lastUpdate = System.currentTimeMillis();

                    String[] answerSplit = commandReqAns.getAnswer().split(",");
                    roomTemp = Float.parseFloat(answerSplit[0]);
                    roomHumidity = Float.parseFloat(answerSplit[1]);

                    tempDataReceived.onDataReceived(roomTemp, roomHumidity);
                }
            });
        }else {
            tempDataReceived.onDataReceived(roomTemp, roomHumidity);
        }
    }

    public void getHistoricalData(long startMillis, long endMillis, HistoricalTempDataReceivedListener historicalTempDataReceivedListener) {
        sendCommand("HISTORY", startMillis + "," + endMillis).setAnswerReceivedListener(new AnswerReceivedListener() {
            @Override
            public void receivedAnswer(CommandReqAnswer commandReqAns) {
                ArrayList<TempData> historicalTempDataList = new ArrayList<>();

                String[] answerSplit = commandReqAns.getAnswer().split(",");
                for (String temperatureReading:answerSplit) {
                    String[] temperatureReadingSplit = temperatureReading.split(":");

                    TempData tempData = new TempData(Long.parseLong(temperatureReadingSplit[0]),
                            Float.parseFloat(temperatureReadingSplit[1]),
                            Float.parseFloat(temperatureReadingSplit[2]));

                    historicalTempDataList.add(tempData);
                }
                historicalTempDataReceivedListener.onReceive(historicalTempDataList);
            }
        });
    }

    public void setRefreshFreq(int minutes, AnswerReceivedListener answerReceivedListener) {
        sendCommand("setRefreshFreq", String.valueOf(minutes*60*1000)).setAnswerReceivedListener(answerReceivedListener);
    }

    public String getDeviceID() {
        return deviceID;
    }
}
