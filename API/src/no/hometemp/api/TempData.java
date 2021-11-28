package no.hometemp.api;

public class TempData {

    private long unixTime;
    private float temperature;
    private float humidity;

    public TempData(long unixTime, float temperature, float humidity) {
        this.unixTime = unixTime;
        this.temperature = temperature;
        this.humidity = humidity;
    }

    public long getUnixTime() {
        return unixTime;
    }

    public float getTemperature() {
        return temperature;
    }

    public float getHumidity() {
        return humidity;
    }
}
