#ifndef NOTE_H_
#define NOTE_H_

#endif /* NOTE_H_ */

typedef struct midiNote { //Structure de note
	//int midiCode;       //Code de la note midi recu via UART
	float frequency;    //frequence de la note en Hz
	float period;       //Periode de la note en ms
	//char noteName[2];   //nom de la note de type NoteOctave (A4)
} midiNote_t;
