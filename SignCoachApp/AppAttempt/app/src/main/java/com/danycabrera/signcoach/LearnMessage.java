package com.danycabrera.signcoach;

/**
 * Created by Tristan on 2016-11-07.
 */

public class LearnMessage {
    private boolean message_type = false;    //false for question, true for lesson
    private char character;
    LearnMessage(boolean type, char character){
        this.character = character;
        this.message_type = type;
    }
    public boolean isLesson(){
        return message_type;
    }
    public String getString(){
        return Character.toString(character);
    }
    public char getChar(){
        return character;
    }
}
