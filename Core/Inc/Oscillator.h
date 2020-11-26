#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_

#endif /* OSCILLATOR_H_ */

enum {
	SINE = 1, SAWTOOTH, TRIANGLE, SQUARE
};

typedef struct OscillatorMem{   //Structure Memoire Oscillator
  float Frequency;      //frequence de la note precedente
  float Compteur;       //Compteur
}OscillatorMem_t;

typedef struct OscillatorParam{ //Structure Parametre Oscillator
  int WaveType;         //Selecteur de WaveForm
  float Alpha;          //Coefficient Aplha (Saturatttyion/rapport cyclique)
}OscillatorParam_t;

//Prototype fonction Oscillator
float Oscillator(float, OscillatorParam_t *, OscillatorMem_t *);

//Prototype fonction Saturation
float Sat(float, int);
