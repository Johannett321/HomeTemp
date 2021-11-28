package no.hometemp.api;

public class HomeTempManager {

    public static HomeTempDevice getDevice(String ip, int port) {
        return new HomeTempDevice(ip, port);
    }

    public HomeTempDevice[] getNetworkHomeTempDevices() {
        return new HomeTempDevice[1];
    }

    public static void configureCurrentDevice(String ssid, String pass, String deviceID, String deviceName, AnswerReceivedListener answerReceivedListener) {
        HomeTempDevice homeTempDevice = getDevice("192.168.4.1", 80);
        homeTempDevice.establishConnection(new ConEstablishedListener() {
            @Override
            public void connectionEstablished() {
                homeTempDevice.sendConfigInfo(ssid, pass, deviceID, deviceName, answerReceivedListener);
            }
        });
    }
}
