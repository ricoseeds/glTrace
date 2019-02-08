#include "main.h"
#include <fstream>
#include "json.hpp"

using namespace std;


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 720

Vect O (0,0,0);
Vect X (1,0,0);
Vect Y (0,1,0);
Vect Z (0,0,1);


struct RGBType {
	double r;
	double g;
	double b;
};




vector<vector<RGBType> > compute();
// void savebmp();
void drawPoint(double xpos, double ypos, GLubyte, GLubyte, GLubyte);
void render(GLFWwindow *, vector<vector<RGBType> >);
void sweep(vector<vector<RGBType>> );

GLfloat adjustY(double yVal);
int main (int argc, char *argv[]) {
	GLFWwindow *window;
    // Initialize the library
    if ( !glfwInit( ) )
    {
        return -1;
    }
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "RayTracer", NULL, NULL );
    
    if ( !window ) {
        glfwTerminate( );
        return -1;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent( window );

    // OpenGL specifics
    glViewport( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT ); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glMatrixMode( GL_PROJECTION ); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glLoadIdentity(); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glOrtho( 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1 ); // essentially set coordinate system
    glMatrixMode( GL_MODELVIEW ); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
    
	vector<vector<RGBType> > data;
	data = compute();
	render(window, data);
	// render
	return 0;
}

vector<vector<RGBType> > compute() {
	
	// int dpi = 72;
	int width = SCREEN_WIDTH;
	int height = SCREEN_HEIGHT;
	int n = width*height;
	// RGBType *pixels = new RGBType[n];
	
	int aadepth = 1;
	double aathreshold = 0.1;
	double aspectratio = (double)width/(double)height;
	double ambientlight = 0.2;
	double accuracy = 0.00000001;
	
	Vect O (0,0,0);
	Vect X (1,0,0);
	Vect Y (0,1,0);
	Vect Z (0,0,1);
	
	Vect new_sphere_location (1.75, -0.25, 0);
	Vect new_sphere_location2 (2.75, -0.80, 0);
	Vect plane_loc(0.2, 1, 0.1);
	Vect campos (3, 1.5, -20);
	
	Vect look_at (0, 6, 0);
	Vect diff_btw (campos.getVectX() - look_at.getVectX(), campos.getVectY() - look_at.getVectY(), campos.getVectZ() - look_at.getVectZ());
	
	Vect camdir = diff_btw.negative().normalize();
	Vect camright = Y.crossProduct(camdir).normalize();
	Vect camdown = camright.crossProduct(camdir);
	Camera scene_cam (campos, camdir, camright, camdown);
	
	Color white_light (1.0, 1.0, 1.0, 0.0);
	Color pretty_green (0.0, 1.0, 0.0, 0.5);
	Color maroon (1.0, 0.0, 0.0, 0.5);
	Color tile_floor (1, 1, 1, 2);
	Color gray (0.5, 0.5, 0.5, 0);
	Color black (0.0, 0.0, 0.0, 0);
	Color blue (0.0, 0.0, 1.0, 0.4);
	Color blue_solid (0.0, 0.0, 1.0, 0);
	Color vio(0.8, 0.4, 0.5, 0.0);
	Vect light_position (-7,10,-10);
	Vect light_position2 (-7,10,10);
	Light scene_light (light_position, white_light);
	Light scene_light2 (light_position2, white_light);
	vector<Light*> light_sources;
	light_sources.push_back(dynamic_cast<Light*>(&scene_light));
	light_sources.push_back(dynamic_cast<Light*>(&scene_light2));
	Quadric qq;
	
	// scene objects
	Sphere scene_sphere (Z, 1, pretty_green);
	Sphere scene_sphere2 (new_sphere_location.vectAdd(Vect(4, 5, -10)), 2, Color(0,0,0,1));
	Sphere scene_sphere3 (new_sphere_location2, 0.5, blue);
	int b = 8, a = 6;
	Triangle tri(Vect(-b, 0, 0), Vect(-b, 0, a), Vect(a-b, 0, 0), vio);
	Triangle tri2(Vect(a-b, 0, 0), Vect(-b, 0, a), Vect(a-b, 0, a), vio);
	

	Triangle tri3(Vect(b, 0, 0), Vect(b + 4, 0, 0), Vect(b + 2, 4, 0), blue_solid);
	Triangle tri4(Vect(b, 4, 0), Vect(b + 4, 4, 0), Vect(b + 2, 9, 0), blue_solid);

	// Plane scene_plane (X, -1, tile_floor);
	// Plane scene_plane2 (plane_loc, white_light);
	Plane scene_plane2 (Y.vectAdd(Vect(0, 5, 0)), white_light);
	vector<Object*> scene_objects;
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere2));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_plane2));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere3));
	scene_objects.push_back(dynamic_cast<Object*>(&qq));
	scene_objects.push_back(dynamic_cast<Object*>(&tri));
	scene_objects.push_back(dynamic_cast<Object*>(&tri2));
	scene_objects.push_back(dynamic_cast<Object*>(&tri3));
	scene_objects.push_back(dynamic_cast<Object*>(&tri4));
	
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



void render(GLFWwindow *window, vector<vector<RGBType> > data){
    while ( !glfwWindowShouldClose( window ) )
    {
        glClear( GL_COLOR_BUFFER_BIT );
        // drawPoint(10,10, 255, 0, 0); // draws a single pixel in the screen 
        sweep(data);
        
        // Swap front and back buffers
        glfwSwapBuffers( window );
    
        // Poll for and process events
        glfwPollEvents( );
    }
}

GLfloat adjustY(double yVal){
    return (GLfloat)(-yVal + SCREEN_HEIGHT);
}

void drawPoint(double xpos, double ypos, GLubyte red, GLubyte green, GLubyte blue){
    // glEnable( GL_POINT_SMOOTH );
    glPointSize( 1 );
    glBegin(GL_POINTS);
    glColor3ub( red, green, blue );
    glVertex2i((GLfloat)xpos, adjustY(ypos));
    // glRasterPos2s((GLfloat)xpos, adjustY(ypos));
    glEnd();
}

void sweep(vector<vector<RGBType> > data){
	for(size_t x = 0; x < data.size(); x++){
		for(size_t y = 0; y < data[0].size(); y++) {
			drawPoint(x, adjustY (y), data[x][y].r * 255, data[x][y].g * 255, data[x][y].b * 255);
		}
		
	}
	// 	for(size_t x = 0; x < SCREEN_WIDTH; x++){
	// 	for(size_t y = 0; y < SCREEN_HEIGHT; y++) {
	// 		drawPoint(x, y, 255, 0, 0 );
	// 	}
		
	// }
	// std::cout << data[10][10].r;
	// std::cout << data[10][10].g;
	// std::cout << data[10][10].b;
	
}
