package com.danycabrera.signcoach;

// Abstracts the frame counting operations
public class Counter {
	public int counter = 0;
	public int trueCount = 0;
	public int falseCount = 0;

	// Update counter (can also reset it)
	public void updateCount(boolean val) {
		if (val) counter++;
		else {
			counter = 0;
		}
	}

	// Resets counters
	public void reset() {
		counter = 0;
		trueCount = 0;
		falseCount = 0;
	}

	// Returns a string that describes the current counters
	public String toString() {
		return "[ counter: " + counter + "trueCount: " + trueCount + ", falseCount: " + falseCount + "]";
	}
}
