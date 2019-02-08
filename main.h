#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Vect.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Light.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
#include "Quadric.h"
#include "Triangle.h"

using namespace std;
int get_closest_index(vector<double> );

int get_closest_index(vector<double> object_intersections) {
	if (object_intersections.size() == 0) {
		return -1;
	}
	else if (object_intersections.size() == 1) {
        return object_intersections.at(0) > 0 ? 0 : 1 ;
	}
	else {
        double min_positive, index=0;
        bool all_neg = true;
        for(int i=0; i<object_intersections.size();i++){
            if(object_intersections[i] > 0){
                min_positive = object_intersections[i];
                all_neg = false;
                break;
            }
        }
        if (all_neg) {
            return -1;
        }
        for (int i=0; i<object_intersections.size(); i++){
            if(object_intersections[i]<= min_positive && object_intersections[i]>0){
                index = i;
                min_positive = object_intersections[i];
            }
        }
        return index;
    }
}
