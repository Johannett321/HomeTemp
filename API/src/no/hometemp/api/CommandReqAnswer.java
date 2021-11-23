package no.hometemp.api;

public class CommandReqAnswer {
    private String answer;
    private String answerParams;

    public CommandReqAnswer(String answer) {
        this.answer = answer;
    }

    public String getAnswer() {
        return answer;
    }

    public String getAnswerParams() {
        return answerParams;
    }

    protected void setAnswerParams(String answerParams) {
        this.answerParams = answerParams;
    }
}
