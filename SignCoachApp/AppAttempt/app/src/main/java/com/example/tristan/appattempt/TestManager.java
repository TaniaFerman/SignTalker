package com.example.tristan.appattempt;

/**
 * Created by Tristan on 2016-11-06.
 */

import java.util.Random;
import java.util.LinkedList;
import com.example.tristan.appattempt.LearnMessage;

public class TestManager {
    private static int[] progressTable = new int[26];    //Stores which signs have been tested
    private static int[] testTable;     //Stores which tests have been correctly answered
    private static String[] questionSets = {"UTOK","BSVP","WEXF","ACLI","MRQG","NDYH"};
    private static int currentSetIndex;
    private LearnMessage current_message;
    private LinkedList<LearnMessage> message_queue;

    TestManager(){
        currentSetIndex = 0;
        initQueue();
       // makeQuestionSet();
    }
    //Clears and fills queue with values from current set
    private void initQueue(){
        int len = questionSets[currentSetIndex].length();
        if(message_queue == null) message_queue = new LinkedList<LearnMessage>();
        else    message_queue.clear();
        for(int i = 0; i < len; i++){
            message_queue.addLast(new LearnMessage(true, questionSets[currentSetIndex].charAt(i)));
        }
        for(int i = 0; i < len; i++){
            message_queue.addLast(new LearnMessage(false, questionSets[currentSetIndex].charAt(i)));
        }

    }
    //Returns true on success, false when no sets left
    public boolean moveToNextSet(){
        currentSetIndex++;
        if(currentSetIndex == questionSets.length){
            currentSetIndex = questionSets.length - 1;
            return false;
        }
        initQueue();
        return true;
    }
    public LearnMessage getNextMessage(){
        if(currentSetCompleted()){
            return null;
        }
        current_message = message_queue.pop();
        return current_message;
    }
    //Gets result for current message
    //Note that nothing is popped from queue because current message has already been popped
    public void sendResult(boolean correct){
        if(current_message.isLesson()) return;

        if(!correct) {
            message_queue.addLast(new LearnMessage(true, current_message.getChar()));
            message_queue.addLast(new LearnMessage(false, current_message.getChar()));
        }
    }
    public boolean currentSetCompleted(){
        return message_queue.size() == 0;
    }
}
