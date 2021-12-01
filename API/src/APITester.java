import no.hometemp.api.*;

public class APITester {

    public static HomeTempDevice homeTempDevice;
    public static HomeTempManager homeTempManager;

    public static void main(String[] args) {
        homeTempManager = new HomeTempManager();

        boolean config = false;

        if (config) {
            configureDevice();
        }else {
            doCommands();
        }
    }

    private static void doCommands() {
        homeTempDevice = homeTempManager.getDevice("192.168.11.172", 80);
        homeTempDevice.establishConnection(new ConEstablishedListener() {
            @Override
            public void connectionEstablished() {
                //deleteConfig();
                //askForCurrentTemp();
                askForHistory();
            }
        });
    }

    private static void deleteConfig() {
        homeTempDevice.sendCommand("deleteConfig").setAnswerReceivedListener(new AnswerReceivedListener() {
            @Override
            public void receivedAnswer(CommandReqAnswer commandReqAns) {
                System.out.println(commandReqAns.getAnswer());
            }
        });
    }

    private static void configureDevice() {
        /*
        String wifiSSID = "JohanH02";
        String wifiPass = "8438936427";
        */

        String wifiSSID = "Johans iPhone";
        String wifiPass = "12345678";

        homeTempManager.configureCurrentDevice(wifiSSID, wifiPass, "23897983", "StuaMi", new AnswerReceivedListener() {
            @Override
            public void receivedAnswer(CommandReqAnswer commandReqAns) {
                System.out.println(commandReqAns.getAnswerParams());
            }
        });
    }

    private static void askForHistory() {
        long now = System.currentTimeMillis()/1000;
        long start = now-(60*60*24*365);
        homeTempDevice.sendCommand("HISTORY;" + start + "," + now).setAnswerReceivedListener(new AnswerReceivedListener() {
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
