package no.hometemp.api;

public interface TempDataReceivedListener {
    void onDataReceived(float temperature, float humidity);
}
