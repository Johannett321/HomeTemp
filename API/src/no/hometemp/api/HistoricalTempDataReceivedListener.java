package no.hometemp.api;

import java.util.ArrayList;

public interface HistoricalTempDataReceivedListener {
    void onReceive(ArrayList<TempData> historicalTempData);
}
