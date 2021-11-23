package no.hometemp.api;

public class HomeTempManager {

    public HomeTempDevice getDevice(String ip, int port) {
        return new HomeTempDevice(ip, port);
    }
}
