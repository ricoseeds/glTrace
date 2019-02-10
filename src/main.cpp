#include "../headers/main.h"
// #include "json.hpp"

using namespace std;


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 720





// vector<vector<RGBType> > compute();
// void savebmp();
void drawPoint(double xpos, double ypos, GLubyte, GLubyte, GLubyte);
void render(GLFWwindow *, vector<vector<RGBType> >);
void sweep(vector<vector<RGBType>> );

GLfloat adjustY(double yVal);
int main (int argc, char *argv[]) {
	string filepath = argv[1];
	GLFWwindow *window;
    // Initialize the library
    if ( !glfwInit( ) )
    {
        return -1;
    }
    // Create a windowed mode window and its OpenGL context
	int width, height;
	double ambientlight;
    // window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "RayTracer", NULL, NULL );
	vector<Object *> scene_objects;
	get_data(filepath, width, height, ambientlight, scene_objects);
    window = glfwCreateWindow( width, height, "RayTracer", NULL, NULL );
    
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

	data = compute(width, height, ambientlight, scene_objects);
	render(window, data);
	// render
	return 0;
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