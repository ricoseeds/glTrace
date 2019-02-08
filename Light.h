#ifndef _Light_H
#define _Light_H

#include "Vect.h"
#include "Color.h"

class Light {
	Vect position;
	Color color;
	
	public:
	Light (Vect position, Color color) {
		this->color = color;
		this->position = position;
	}
	Vect getLightPosition () { return position; }
	Color getLightColor () { return color; }
	
};
#endif
