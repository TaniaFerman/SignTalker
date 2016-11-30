package com.danycabrera.signcoach;

/**
 * Created by Tristan on 2016-11-06.
 */

import android.util.Log;

import java.util.Random;
import java.util.LinkedList;

public class TestManager {
    private static int[] progressTable = new int[26];    //Stores which signs have been tested
    private static int[] testTable;     //Stores which tests have been correctly answered
    private static String[] questionSets = {"ABCD", "UTOK","BSVP","WEXF","ACLI","MRQG","NDYH"};
    private static int currentSetIndex;
    private LearnMessage current_message;
    private LinkedList<LearnMessage> message_queue, question_queue, lesson_queue;

    TestManager(){
        currentSetIndex = 0;
        initQueue();
        lesson_queue = new LinkedList<>();
        question_queue = new LinkedList<>();
    }
    //Clears and fills queue with values from current set
    private void initQueue(){
        int len = questionSets[currentSetIndex].length();
        Log.d("InitQueue::", "initializing q on " + questionSets[currentSetIndex]);
        message_queue.clear();
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
        if(currentSetIndex >= questionSets.length){
            currentSetIndex = 0;
            Log.d("nextset:::", "New set is " + questionSets[currentSetIndex]);
            return false;
        }
        initQueue();
        return true;
    }
    public LearnMessage getCurrentMessage(){
        return current_message;
    }
    public LearnMessage getNextMessage(){
        if(message_queue.size() == 0){
            if(lesson_queue.size() != 0){
                //Could probably do both loops in one
                //but that seems kinda sketch
                while(lesson_queue.size() != 0){
                    message_queue.addLast(lesson_queue.pop());
                }
                while(question_queue.size() != 0){
                    message_queue.addLast(question_queue.pop());
                }
            }
            else return null;
        }
        current_message = message_queue.pop();
        return current_message;
    }
    //Gets result for current message
    //Note that nothing is popped from queue because current message has already been popped
    public void sendResult(boolean correct){
        if(current_message.isLesson()) return;

        if(!correct) {
            lesson_queue.addLast(new LearnMessage(true, current_message.getChar()));
            question_queue.addLast(new LearnMessage(false, current_message.getChar()));
        }
    }
    public void setSet(int setIndex){
        currentSetIndex = setIndex;
        initQueue();
    }
    public boolean currentSetCompleted(){
        return message_queue.size() == 0 && question_queue.size() == 0 && lesson_queue.size() == 0;
    }
    public int getCurrentSetIndex(){
        return currentSetIndex;
    }
    public int getNumberOfSets(){
        return questionSets.length;
    }
}
