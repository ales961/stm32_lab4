#include <math.h>
#include <string.h>
#include <stdio.h>
#include "hardware/buzzer.h"
#include "tim.h"

uint8_t octave = 0;
char* octaveNames[9] = {"sub-contra octave", "contra octave", "great octave", "small octave",
		"one-line octave", "two-line octave", "three-line octave", "four-line octave", "five-line octave"};

double notes[7] = {16.352, 18.354, 20.602, 21.827, 24.5, 27.5, 30.868};
char* noteNames[7] = {"A", "B", "C", "D", "E", "F", "G"};

int duration = 1000;

double getNoteFrequency(uint8_t noteNumber) {
	return rint(notes[noteNumber] * pow(2, octave));
}

void setVolume(uint8_t volume) {
	if (0 <= volume && volume <= 10) {
		TIM1->CCR1 = volume;
	}
}

void setFrequency(uint16_t freq) {
	TIM1->PSC = HAL_RCC_GetPCLK2Freq() / (10 * freq) - 1;
}

void setPlay(uint8_t noteNumber) {
	setFrequency((uint8_t) getNoteFrequency(noteNumber));
	setVolume(10);
}

char buf[128];
char* playNote(uint8_t noteNumber) {
	setPlay(noteNumber);
	TIM6_START();
	sprintf(buf, "note %s, %s, duration %dms", noteNames[noteNumber], octaveNames[octave], duration);
	return buf;
}

char* playA() {
	return playNote(0);
}
char* playB() {
	return playNote(1);
}
char* playC() {
	return playNote(2);
}
char* playD() {
	return playNote(3);
}
char* playE() {
	return playNote(4);
}
char* playF() {
	return playNote(5);
}
char* playG() {
	return playNote(6);
}

char* playAll() {
	setPlayAllFlag();
	setPlay(0);
	TIM6_START();
	sprintf(buf, "notes A,B,C,D,E,F,G, %s, duration %dms", octaveNames[octave], duration);
	return buf;
}

char* upOctave() {
	if (octave < 8) {
		octave++;
		return strcat(octaveNames[octave], "\n");
	}
	return "Octave is maximum\n";
}

char* downOctave() {
	if (octave > 0) {
		octave--;
		return strcat(octaveNames[octave], "\n");
	}
	return "Octave is minimum\n";
}

char* upDuration() {
	if (duration < 5000) {
		TIM6->CNT += 100;
		duration += 100;
		sprintf(buf, "%dms\n", duration);
		return buf;
	}
	return "Duration is maximum\n";
}

char* downDuration() {
	if (duration > 100) {
		TIM6->CNT -= 100;
		duration -= 100;
		sprintf(buf, "%dms\n", duration);
		return buf;
	}
	return "Duration is minimum\n";
}
