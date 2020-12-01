/*
 * Operator.h
 *
 *  Created on: 1 déc. 2020
 *      Author: Gael Dezanneau
 */
#ifndef OPERATOR_H_
#define OPERATOR_H_


#endif /* OPERATOR_H_ */

typedef struct OperatorParam {   //Structure Parametres Operator
	//Oscillator
	int WaveType;         //Selecteur de WaveForm
	float Alpha;          //Coefficient Aplha (Saturatttyion/rapport cyclique)
	//Enveloppe
	float Attack;      //Attack
	float Decay;       //Decay
	float Sustain;     //Sustain
	float Release;     //Release
} OperatorParam_t;

typedef struct OperatorMem {   //Structure Parametres Operator
	//Oscillator
	float Frequency;      //frequence de la note precedente
	float CompteurOsc;       //Compteur
	//Enveloppe
	float CompteurEnv;    //Compteur Précendent
	int Phase;       //Phase Précendente
	int Note;     //Note Précendente
} OperatorMem_t;

float Operator(int,float, OperatorParam_t, OperatorMem_t *);
