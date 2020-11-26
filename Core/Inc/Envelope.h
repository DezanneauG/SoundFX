/*
 * Envelope.h
 *
 *  Created on: 24 nov. 2020
 *      Author: Gael Dezanneau
 */

#ifndef ENVELOPE_H_
#define ENVELOPE_H_



#endif /* ENVELOPE_H_ */

enum {
	ATTACK, DECAY, SUSTAIN, RELEASE, OFF
};
//Etats

typedef struct Envelope_Param{   //Structure Parametres
  float Attack;      //Attack
  float Decay;       //Decay
  float Sustain;     //Sustain
  float Release;     //Release
}Envelope_Param_t;

typedef struct Envelope_Mem{   //Structure Memoire
  float Compteur;    //Compteur
  int Phase;       //Gain
  int Note;     //Attack
}Envelope_Mem_t;

//Prototype fonction Envelope
float Envelope(int, Envelope_Param_t *, Envelope_Mem_t *);

