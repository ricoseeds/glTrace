#ifndef _RAY_H
#define _RAY_H

#include "Vect.h"

class Ray {
	Vect origin, direction;
	
	public:
	Ray (Vect o, Vect d): origin(o), direction(d) {}
	Vect getRayOrigin () { return origin; }
	Vect getRayDirection () { return direction; }
	
};

#endif
