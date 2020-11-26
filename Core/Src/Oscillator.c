#include "Oscillator.h"
#include <math.h>

#define PI 3.14159f

float Oscillator(float Frequency, OscillatorParam_t *Param,
		OscillatorMem_t *Memory) {
	//frequency Normalisée par rapport a la frequence d'exec
	//idem pour period (arrondie en entier)
	//WaveType: 1 -> Sinus
	//          2 -> SawTooth
	//          3 -> Triangle
	//          4 -> Square(PWM)
	//déclarations variables
	float Compteur, Tmp, Output;
	Compteur = Memory->Compteur;
	//RàZ compteur si note diffférente
	//section de RAZ suppr car FM = variation de la fréquence d'entrée
	/*
	 if((Memory->Frequency != Frequency)){
	 Compteur = 0;
	 }
	 */
	//compteur
	if (Compteur < 2) {
		Compteur += Frequency;  //Peut engendrer des problemes d'intégration
	} else {
		Compteur = 0;
	}
	//Normalisation output entre -1 et 1
	Output = Compteur - 1;

	//Oscillator
	switch (Param->WaveType) {
	case SAWTOOTH: //SawTooth
		Output = Sat(Output, Param->Alpha);
		break;
	case TRIANGLE: //triangle
		if (Output > 0) {
			Tmp = -Output;
		} else {
			Tmp = Output;
		}
		Output = Tmp * 2 + 1;
		Output = Sat(Output, Param->Alpha);
		break;
	case SQUARE: //Square
		if (Output > (1 - 2 * Param->Alpha)) {
			Output = 1;
		} else {
			Output = -1;
		}
		break;
	default: //Sinus
		Output = sinf(Output * 2 * PI);
		Output = Sat(Output, Param->Alpha);
		break;
	}
	//Mise en Memoire
	Memory->Frequency = Frequency;
	Memory->Compteur = Compteur;
	//output value
	return Output;
}

float Sat(float Input, int Alpha) { //Saturation avec conservation de l'amplitude max
	float Output;
	//Coeff Alpha
	Output = Input / Alpha;
	//Saturation en -1,1
	if (Output > 1) { //Positive
		Output = 1;
	} else if (Output < -1) { //Negative
		Output = -1;
	}
	return Output;
}
