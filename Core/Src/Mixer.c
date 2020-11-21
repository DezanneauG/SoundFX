#include "Mixer.h"


float Mixer(Tranche_t Tranches[],float GeneralGain,int NombreTranches) {
	//exemple pour2 tranches
	//Output = (Gain1*In1+Gain2*In2)/(Gain1+Gain2)*GeneralGain
	float SumGain = 0;
	float SumValues = 0;
	float Output;

	int i;//for Variable

	for(i=0;i<NombreTranches;i++){//Parcours de chaque tranche
		SumGain = SumGain + Tranches[i].Gain;
		SumValues = SumValues + Tranches[i].Gain * Tranches[i].Value;
	}
	if(SumGain !=0 ){// Div par 0
		Output = SumValues/SumGain*GeneralGain; //Gain  Général (indépendant tranches)
	}else{
		Output = 0;
	}
	return Output;
}

