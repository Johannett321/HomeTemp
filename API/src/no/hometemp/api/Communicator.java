package no.hometemp.api;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.ArrayList;

class Communicator {
    private ArrayList<CommandReq> commandList = new ArrayList<>();
    private BufferedReader in;
    private PrintWriter out;

    private boolean shouldListen = true;

    protected Communicator(Socket socket) throws IOException {
        out = new PrintWriter(socket.getOutputStream(), true);
        in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                startListening();
            }
        });
        thread.start();
    }

    private void startListening() {
        System.out.println("Communicator is now listening!");
        while (shouldListen) {
            try {
                String readLine = in.readLine();
                if (readLine != null && !readLine.isEmpty()) {
                    decodeCommand(readLine);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void decodeCommand(String rawCommand) {
        System.out.println("Raw answer: " + rawCommand);
        String[] commandSplit = rawCommand.split(";");
        String reqID = commandSplit[0];

        for (int i = 0; i < commandList.size(); i++) {
            if (commandList.get(i).getReqID().equals(reqID)) {
                CommandReqAnswer commandReqAnswer = new CommandReqAnswer(commandSplit[1]);

                if (commandSplit.length > 2) {
                    commandReqAnswer.setAnswerParams(commandSplit[2]);
                }

                System.out.println("Formatted answer: " + commandReqAnswer.getAnswer());
                commandList.get(i).answerReceived(commandReqAnswer);
                commandList.remove(commandList.get(i));
                break;
            }
        }
    }

    protected CommandReq createCommandRequest(String command, String parameters) {
        CommandReq commandReq = new CommandReq(command,parameters);
        executeCommand(commandReq);
        return commandReq;
    }

    private void executeCommand(CommandReq commandReq) {
        String lineToPrint = commandReq.getReqID();
        lineToPrint += ";";
        lineToPrint += commandReq.getCommand();

        if (commandReq.getParameters() != null && !commandReq.getParameters().isEmpty()) {
            lineToPrint += ";" + commandReq.getParameters();
        }

        System.out.println("Trying to send command: " + lineToPrint);
        commandList.add(commandReq);
        out.println(lineToPrint);
    }
}
