package no.hometemp.api;

import java.util.Random;

public class CommandReq {
    private String reqID;
    private String command;
    private String parameters;

    private AnswerReceivedListener answerReceivedListener;

    protected CommandReq(String command, String parameters) {
        Random random = new Random();
        reqID = System.currentTimeMillis() + String.valueOf(random.nextInt(100000));

        this.command = command;
        this.parameters = parameters;
    }

    public String getReqID() {
        return reqID;
    }

    public String getCommand() {
        return command;
    }

    public String getParameters() {
        return parameters;
    }

    protected void answerReceived(CommandReqAnswer commandReqAnswer) {
        if (answerReceivedListener != null) {
            answerReceivedListener.receivedAnswer(commandReqAnswer);
        }
    }

    public void setAnswerReceivedListener(AnswerReceivedListener answerReceivedListener) {
        this.answerReceivedListener = answerReceivedListener;
    }
}
