/*
 * Operator.c
 *
 *  Created on: 1 déc. 2020
 *      Author: Gael Dezanneau
 */

#include "Operator.h"
#include "Envelope.h"
#include "Oscillator.h"

#include <stdio.h>

float Operator(int Note, float frequency, OperatorParam_t Param, OperatorMem_t *Mem) {
	float output_fct, output_env, output_osc;

	//		Parametres		//
	//Oscillator
	OscillatorParam_t OscParam;
	OscParam.Alpha = Param.Alpha;
	OscParam.WaveType = Param.WaveType;
	//Envelope
	Envelope_Param_t EnvParam;
	EnvParam.Attack = Param.Attack;
	EnvParam.Decay = Param.Decay;
	EnvParam.Sustain = Param.Sustain;
	EnvParam.Release = Param.Release;

	//		Lecture Mémoire		//
	//Oscillator
	OscillatorMem_t OscMem;
	OscMem.Compteur = Mem->CompteurOsc;
	OscMem.Frequency = Mem->Frequency;
	//Envelope
	Envelope_Mem_t EnvMem;
	EnvMem.Compteur = Mem->CompteurEnv;
	EnvMem.Phase = Mem->Phase;
	EnvMem.Note = Mem->Note;

	//Oscillator function
	output_osc = Oscillator(frequency, &OscParam, &OscMem);
	//Envelope Function
	output_env = Envelope(Note, &EnvParam, &EnvMem);

	//		Mise en Mémoire		//
	//Oscillator
	Mem->CompteurOsc = OscMem.Compteur;
	Mem->Frequency = OscMem.Frequency;
	//Envelope

	Mem->CompteurEnv = EnvMem.Compteur;
	Mem->Phase = EnvMem.Phase;
	Mem->Note = EnvMem.Note;

	//Calcul valeur de retour
	output_fct = output_env * output_osc;
	//Retour
	return (output_fct);
}
