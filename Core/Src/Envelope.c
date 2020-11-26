/*
 * Envelope.c
 *
 *  Created on: 24 nov. 2020
 *      Author: Gael Dezanneau
 */
#include "Envelope.h"
#include <stdio.h>

float Envelope(int Note, Envelope_Param_t *Param, Envelope_Mem_t *Mem) {
	float Output;
	//Sécurité Parametres
	if (Param->Attack <= 0) {
		Param->Attack = 1;
	}
	if (Param->Decay <= 0) {
		Param->Decay = 1;
	}
	if (Param->Release <= 0) {
		Param->Release = 1;
	}
	if ((Param->Sustain < 0) || ((Param->Sustain > 1))) {
		Param->Sustain = 0.5;
	}

	//Determination état
//	if ((Note == 1) && (Mem->Note == 0)) { //Trig ON
//		Mem->Phase = ATTACK;
//		Mem->Compteur = 0;
//	} else if ((Note == 0) && (Mem->Note == 1)) { //Trig OFF
//		Mem->Phase = RELEASE;
//	} else if ((Note == 0) && (Mem->Note == 0)) { //Note OFF
//		if (Mem->Compteur > 0) { //Release
//			Mem->Phase = RELEASE;
//		} else { //Off
//			Mem->Phase = OFF;
//		}
//	} else if ((Note == 1) && (Mem->Note == 1)) { //Note Enclenchée
//		if (Mem->Compteur >= 1) { //Passage de attack à decay
//			Mem->Phase = DECAY;
//		} else if ((Mem->Compteur <= Param->Sustain) && (Mem->Phase = DECAY)) { //Passage de decay à sustain
//			Mem->Phase = SUSTAIN;
//		} else {
//			Mem->Phase = ATTACK;
//		}
//	}
	//Process selon les états
	switch (Mem->Phase) {
	case ATTACK:
		if (Mem->Compteur <= 1) {
			Mem->Phase = ATTACK;
		} else {
			Mem->Phase = DECAY;
		}
		if (Note == 0) {
			Mem->Phase = RELEASE;
		}
		break;
	case DECAY:
		if (Mem->Compteur > Param->Sustain) {
			Mem->Phase = DECAY;
		} else {
			Mem->Phase = SUSTAIN;
		}
		if (Note == 0) {
			Mem->Phase = RELEASE;
		}
		if ((Note == 1) && (Mem->Note == 0)) { //Trig ON
			Mem->Phase = ATTACK;
			Mem->Compteur = 0;
		}
		break;
	case SUSTAIN:
		if (Note == 1) {
			Mem->Phase = SUSTAIN;
		} else {
			Mem->Phase = RELEASE;
		}
		if ((Note == 1) && (Mem->Note == 0)) { //Trig ON
			Mem->Phase = ATTACK;
			Mem->Compteur = 0;
		}
		break;
	case RELEASE:
		if (Mem->Compteur > 0) {
			Mem->Phase = RELEASE;
		} else {
			Mem->Phase = OFF;
		}
		if ((Note == 1) && (Mem->Note == 0)) { //Trig ON
			Mem->Phase = ATTACK;
			Mem->Compteur = 0;
		}
		break;
	default: //OFF
		if ((Note == 1) && (Mem->Note == 0)) { //Trig ON
			Mem->Phase = ATTACK;
			Mem->Compteur = 0;
		}
		break;
	}
	//Process selon les états
	switch (Mem->Phase) {
	case ATTACK:
		Mem->Compteur += 1 / Param->Attack;
		break;
	case DECAY:
		Mem->Compteur -= 1 / Param->Decay;
		break;
	case SUSTAIN:
		Mem->Compteur = Param->Sustain;
		break;
	case RELEASE:
		Mem->Compteur -= 1 / Param->Release;
		break;
	default: //OFF
		Mem->Compteur = 0;
		break;
	}
	//Mise en mémoire
	Mem->Note = Note;
	//Retour Output
	Output = Mem->Compteur;
	return (Output);
}
