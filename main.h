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
#include <fstream>
#include "json.hpp"


using json = nlohmann::json;

using namespace std;


struct RGBType {
	double r;
	double g;
	double b;
};
const Vect O (0,0,0);
const Vect X (1,0,0);
const Vect Y (0,1,0);
const Vect Z (0,0,1);

vector<Light *> light_sources;
vector<vector<RGBType> > compute(int, int, double, vector<Object *>&);
void get_data(string, int&, int&, double&, vector<Object *>&);
int get_closest_index(vector<double> );
void get_data(string filepath, int &width, int &height, double &ambientlight, vector<Object *> &scene_objects){
    std::ifstream ifs(filepath);
    json j = json::parse(ifs);
    width = (int) j["width"];
    height = (int) j["height"];
	ambientlight = (double) j["ambient_light"];
	for (auto& x : json::iterator_wrapper(j["scene"])) {
		for (auto& y : json::iterator_wrapper(x.value())) {
			if (y.key() == "Sphere"){
				Vect pos(y.value()["position"][0], y.value()["position"][1], y.value()["position"][2]);
				Color col(y.value()["color"][0], y.value()["color"][1], y.value()["color"][2], y.value()["color"][3]);
				Sphere *sphere = new Sphere(pos, (double)y.value()["radius"], col);
				scene_objects.push_back(dynamic_cast<Object*>(sphere));
			} else if(y.key() == "Plane"){
				Vect normal(y.value()["normal"][0], y.value()["normal"][1], y.value()["normal"][2]);
				Color col(y.value()["color"][0], y.value()["color"][1], y.value()["color"][2], y.value()["color"][3]);
				Plane *plane = new Plane(normal, col);
				scene_objects.push_back(dynamic_cast<Object*>(plane));
			} else if(y.key() == "Quadric"){
				Color col(y.value()["color"][0], y.value()["color"][1], y.value()["color"][2], y.value()["color"][3]);
				Quadric *qq = new Quadric((double)y.value()["A"], (double)y.value()["B"], (double)y.value()["C"], (double)y.value()["D"], (double)y.value()["E"], (double)y.value()["F"],  (double)y.value()["G"],  (double)y.value()["H"],  (double)y.value()["I"],  (double)y.value()["J"], col);
				scene_objects.push_back(dynamic_cast<Object*>(qq));
			} else if(y.key() == "Triangle"){
				Vect posX(y.value()["pos_x"][0], y.value()["pos_x"][1], y.value()["pos_x"][2]);
				Vect posY(y.value()["pos_y"][0], y.value()["pos_y"][1], y.value()["pos_y"][2]);
				Vect posZ(y.value()["pos_z"][0], y.value()["pos_z"][1], y.value()["pos_z"][2]);
				Color col(y.value()["color"][0], y.value()["color"][1], y.value()["color"][2], y.value()["color"][3]);
				Triangle *tri = new Triangle(posX, posY, posZ, col);
				scene_objects.push_back(dynamic_cast<Object*>(tri));
			}
		}
    }
    for (auto& x : json::iterator_wrapper(j["light"])) {
		cout << x.value()["color"] << endl;
		Vect pos((double)x.value()["position"][0], (double)x.value()["position"][1], (double)x.value()["position"][2]);
		Color col((double)x.value()["color"][0], (double)x.value()["color"][1], (double)x.value()["color"][2], (double)x.value()["color"][3]);
		Light *newLight =new Light(pos, col);
		light_sources.push_back(dynamic_cast<Light*>(newLight));
	}

}




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
Color getColorAt(Vect intersection_position, Vect intersecting_ray_direction, vector<Object*> scene_objects, int index_of_winning_object, vector<Light*> light_sources, double accuracy, double ambientlight) {
	
	Color winning_object_color = scene_objects[index_of_winning_object]->getColor();
	Vect winning_object_normal = scene_objects[index_of_winning_object]->getNormalAt(intersection_position);
	Color final_color = winning_object_color.colorScalar(ambientlight);
	
	if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
		// reflection from objects with specular intensity
		double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
		Vect scalar1 = winning_object_normal.vectMult(dot1);
		Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
		Vect scalar2 = add1.vectMult(2);
		Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
		Vect reflection_direction = add2.normalize();
		
		Ray reflection_ray (intersection_position, reflection_direction);
		
		// determine what the ray intersects with first
		vector<double> reflection_intersections;
		
		for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++) {
			reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
		}
		
		// int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);
		int index_of_winning_object_with_reflection = get_closest_index(reflection_intersections);
		
		if (index_of_winning_object_with_reflection != -1) {
			// reflection ray missed everthing else
			if (reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy) {
				// determine the position and direction at the point of intersection with the reflection ray
				// the ray only affects the color if it reflected off something
				
				Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
				Vect reflection_intersection_ray_direction = reflection_direction;
				
				Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);
				
				final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(winning_object_color.getColorSpecial()));
			}
		}
	}
	
	for (int light_index = 0; light_index < light_sources.size(); light_index++) {
		Vect light_direction = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();
		
		float cosine_angle = winning_object_normal.dotProduct(light_direction);
		
		if (cosine_angle > 0) {
			// test for shadows
			bool shadowed = false;
			
			Vect distance_to_light = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();
			float distance_to_light_magnitude = distance_to_light.magnitude();
			
			Ray shadow_ray (intersection_position, light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize());
			
			vector<double> secondary_intersections;
			
			for (int object_index = 0; object_index < scene_objects.size() && shadowed == false; object_index++) {
				secondary_intersections.push_back(scene_objects.at(object_index)->findIntersection(shadow_ray));
			}
			
			for (int c = 0; c < secondary_intersections.size(); c++) {
				if (secondary_intersections.at(c) > accuracy) {
					if (secondary_intersections.at(c) <= distance_to_light_magnitude) {
						shadowed = true;
					}
					break;
				}
				
			}
			
			if (shadowed == false) {
				final_color = final_color.colorAdd(winning_object_color.colorMultiply(light_sources.at(light_index)->getLightColor()).colorScalar(cosine_angle));
				
				if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
					// special [0-1]
					double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
					Vect scalar1 = winning_object_normal.vectMult(dot1);
					Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
					Vect scalar2 = add1.vectMult(2);
					Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
					Vect reflection_direction = add2.normalize();
					
					double specular = reflection_direction.dotProduct(light_direction);
					if (specular > 0) {
						specular = pow(specular, 10);
						final_color = final_color.colorAdd(light_sources.at(light_index)->getLightColor().colorScalar(specular*winning_object_color.getColorSpecial()));
					}
				}
				
			}
			
		}
	}
	
	return final_color.clip();
}

vector<vector<RGBType> > compute(int width, int height, double ambientlight, vector<Object *>&scene_objects ) {
	
	// int dpi = 72;
	// int width = SCREEN_WIDTH;
	// int height = SCREEN_HEIGHT;

	int n = width*height;
	// RGBType *pixels = new RGBType[n];
	
	int aadepth = 1;
	double aathreshold = 0.1;
	double aspectratio = (double)width/(double)height;
	// double ambientlight = 0.2;
	double accuracy = 0.00000001;
	
	Vect O (0,0,0);
	Vect X (1,0,0);
	Vect Y (0,1,0);
	Vect Z (0,0,1);
	
	// Vect new_sphere_location (1.75, -0.25, 0);
	// Vect new_sphere_location2 (2.75, -0.80, 0);
	// Vect plane_loc(0.2, 1, 0.1);
	Vect campos (3, 1.5, -20);
	
	Vect look_at (0, 6, 0);
	Vect diff_btw (campos.getVectX() - look_at.getVectX(), campos.getVectY() - look_at.getVectY(), campos.getVectZ() - look_at.getVectZ());
	
	Vect camdir = diff_btw.negative().normalize();
	Vect camright = Y.crossProduct(camdir).normalize();
	Vect camdown = camright.crossProduct(camdir);
	Camera scene_cam (campos, camdir, camright, camdown);
	
	// Color white_light (1.0, 1.0, 1.0, 0.0);
	// Vect light_position (-7,10,-10);
	// Vect light_position2 (-7,10,10);
	// Light scene_light (light_position, white_light);
	// Light scene_light2 (light_position2, white_light);
	// vector<Light*> light_sources;
	// light_sources.push_back(dynamic_cast<Light*>(&scene_light));
	// light_sources.push_back(dynamic_cast<Light*>(&scene_light2));

	int thisone, aa_index;
	double xamnt, yamnt;
	double tempRed, tempGreen, tempBlue;
	vector<vector<RGBType> > sweepxy;
	RGBType pixel;
	for (int x = 0; x < width; x++) {
		vector<RGBType> pixelx;
		for (int y = 0; y < height; y++) {
			thisone = y*width + x;
			
			// start with a blank pixel
			double tempRed[aadepth*aadepth];
			double tempGreen[aadepth*aadepth];
			double tempBlue[aadepth*aadepth];
			
			
			for (int aax = 0; aax < aadepth; aax++) {
				for (int aay = 0; aay < aadepth; aay++) {
			
					aa_index = aay*aadepth + aax;
					
					
					// create the ray from the camera to this pixel
					if (aadepth == 1) {
					
						// start with no anti-aliasing
						if (width > height) {
							// the image is wider than it is tall
							xamnt = ((x+0.5)/width)*aspectratio - (((width-height)/(double)height)/2);
							yamnt = ((height - y) + 0.5)/height;
						}
						else if (height > width) {
							// the imager is taller than it is wide
							xamnt = (x + 0.5)/ width;
							yamnt = (((height - y) + 0.5)/height)/aspectratio - (((height - width)/(double)width)/2);
						}
						else {
							// the image is square
							xamnt = (x + 0.5)/width;
							yamnt = ((height - y) + 0.5)/height;
						}
					}
					else {
						// anti-aliasing
						if (width > height) {
							// the image is wider than it is tall
							xamnt = ((x + (double)aax/((double)aadepth - 1))/width)*aspectratio - (((width-height)/(double)height)/2);
							yamnt = ((height - y) + (double)aax/((double)aadepth - 1))/height;
						}
						else if (height > width) {
							// the imager is taller than it is wide
							xamnt = (x + (double)aax/((double)aadepth - 1))/ width;
							yamnt = (((height - y) + (double)aax/((double)aadepth - 1))/height)/aspectratio - (((height - width)/(double)width)/2);
						}
						else {
							// the image is square
							xamnt = (x + (double)aax/((double)aadepth - 1))/width;
							yamnt = ((height - y) + (double)aax/((double)aadepth - 1))/height;
						}
					}
					
					Vect cam_ray_origin = scene_cam.getCameraPosition();
					Vect cam_ray_direction = camdir.vectAdd(camright.vectMult(xamnt - 0.5).vectAdd(camdown.vectMult(yamnt - 0.5))).normalize();
					
					Ray cam_ray (cam_ray_origin, cam_ray_direction);
					
					vector<double> intersections;
					
					for (int index = 0; index < scene_objects.size(); index++) {
						intersections.push_back(scene_objects.at(index)->findIntersection(cam_ray));
					}
					
					unsigned int index_of_winning_object = get_closest_index(intersections);
					
					if (index_of_winning_object == -1) {
						// set the backgroung black
						// tempRed[aa_index] = tempGreen[aa_index] =tempBlue[aa_index] = 0;
						tempRed[aa_index] =0; 
						tempGreen[aa_index] = 0;
						tempBlue[aa_index] = 0;
					}
					else if (intersections[index_of_winning_object] ) {
						// index coresponds to an object in our scene
						// if (intersections.at(index_of_winning_object) > accuracy) {
							// determine the position and direction vectors at the point of intersection
							
						Vect intersection_position = cam_ray_origin.vectAdd(cam_ray_direction.vectMult(intersections.at(index_of_winning_object)));
						Vect intersecting_ray_direction = cam_ray_direction;
	
						Color intersection_color = getColorAt(intersection_position, intersecting_ray_direction, scene_objects, index_of_winning_object, light_sources, accuracy, ambientlight);
						
						tempRed[aa_index] = intersection_color.getColorRed();
						tempGreen[aa_index] = intersection_color.getColorGreen();
						tempBlue[aa_index] = intersection_color.getColorBlue();
						// }
					}
				}
			}
			
			// average the pixel color
			double totalRed = 0;
			double totalGreen = 0;
			double totalBlue = 0;
			
			for (int iRed = 0; iRed < aadepth*aadepth; iRed++) {
				totalRed = totalRed + tempRed[iRed];
			}
			for (int iGreen = 0; iGreen < aadepth*aadepth; iGreen++) {
				totalGreen = totalGreen + tempGreen[iGreen];
			}
			for (int iBlue = 0; iBlue < aadepth*aadepth; iBlue++) {
				totalBlue = totalBlue + tempBlue[iBlue];
			}
			
			double avgRed = totalRed/(aadepth*aadepth);
			double avgGreen = totalGreen/(aadepth*aadepth);
			double avgBlue = totalBlue/(aadepth*aadepth);
			
			pixel.r = avgRed;
			pixel.g = avgGreen;
			pixel.b = avgBlue;
			pixelx.push_back(pixel);

		}
		sweepxy.push_back(pixelx);
	}
	
	// savebmp("scene_anti-aliased.bmp",width,height,dpi,pixels);
	//render
	return sweepxy;
}


