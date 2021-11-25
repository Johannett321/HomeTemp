import no.hometemp.api.*;

import java.io.IOException;
import java.net.Socket;

public class APITester {

    public static HomeTempDevice homeTempDevice;

    public static void main(String[] args) {
        HomeTempManager homeTempManager = new HomeTempManager();
        homeTempDevice = homeTempManager.getDevice("172.20.10.9", 80);
        homeTempDevice.establishConnection(new ConEstablishedListener() {
            @Override
            public void connectionEstablished() {
                System.out.println("Connection established!");
                doCommands();
            }
        });
    }

    private static void doCommands() {
        askForCurrentTemp();
        askForHistory();
    }

    private static void askForHistory() {
        homeTempDevice.sendCommand("HISTORY").setAnswerReceivedListener(new AnswerReceivedListener() {
            @Override
            public void receivedAnswer(CommandReqAnswer commandReqAns) {
                System.out.println("History: " + commandReqAns.getAnswer());
            }
        });
    }

    private static void askForCurrentTemp() {
        homeTempDevice.sendCommand("CURRENT_TEMP").setAnswerReceivedListener(new AnswerReceivedListener() {
            @Override
            public void receivedAnswer(CommandReqAnswer commandReqAns) {
                System.out.println("Received: " + commandReqAns.getAnswer());
            }
        });
    }
}
