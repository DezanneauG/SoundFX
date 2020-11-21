#ifndef MIXER_H_
#define MIXER_H_

#endif /* MIXER_H_ */

typedef struct Tranche{   //Structure Tranche
  float Value;      //Valeur
  float Gain;       //Gain
}Tranche_t;


//Prototype fonction Mixer
float Mixer(Tranche_t[], float, int);
