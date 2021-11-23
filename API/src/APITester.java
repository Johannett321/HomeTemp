import no.hometemp.api.*;

import java.io.IOException;
import java.net.Socket;

public class APITester {

    public static HomeTempDevice homeTempDevice;

    public static void main(String[] args) {
        HomeTempManager homeTempManager = new HomeTempManager();
        homeTempDevice = homeTempManager.getDevice("192.168.11.172", 80);
        homeTempDevice.establishConnection(new ConEstablishedListener() {
            @Override
            public void connectionEstablished() {
                System.out.println("Connection established!");
                askForCurrentTemp();
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
