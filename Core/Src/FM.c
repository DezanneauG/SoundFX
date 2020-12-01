/*
 * FM.c
 *
 *  Created on: 1 déc. 2020
 *      Author: Gael Dezanneau
 */

#include "FM.h"
#include <stdio.h>

float FM(float Modulant, float Module, float Facteur) {
	float output;
	output = (Modulant*Facteur + 1)*Module;
	return (output);
}
