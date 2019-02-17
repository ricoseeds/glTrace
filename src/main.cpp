#include "../include/main.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;

void drawPoint(double xpos, double ypos, GLubyte, GLubyte, GLubyte);
void drawBigPoint(double xpos, double ypos);
void render(GLFWwindow *, vector<vector<RGBType> >);
void sweep(vector<vector<RGBType>> );
GLfloat adjustY(double yVal);
static void cursorPositionCallback( GLFWwindow *window, double xpos, double ypos );
void shootRay(glm::vec3, glm::vec3, glm::vec3);
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void drawLightRays();
glm::vec3 vec3Convert(Vect x);
double xpos, ypos;
glm::vec3 primary_ray(1.0f, 0.0f, 0.0f);
int main (int argc, char *argv[]) {
	string filepath = argv[1];
	GLFWwindow *window;
    // Initialize the library
    if ( !glfwInit( ) )
    {
        return -1;
    }
    // Create a windowed mode window and its OpenGL context
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
    
    //Callbacks
    glfwSetCursorPosCallback( window, cursorPositionCallback );
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
    glfwSetInputMode( window, GLFW_STICKY_MOUSE_BUTTONS, 1 );
    glfwSetKeyCallback( window, KeyCallback );
    // OpenGL specifics
    glViewport( 0.0f, 0.0f, width, height ); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glMatrixMode( GL_PROJECTION ); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glLoadIdentity(); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glOrtho( 0, width, 0, height, 0, 1 ); // essentially set coordinate system
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
        glfwGetCursorPos( window, &xpos, &ypos);
        // glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
        // drawPoint(10,10, 255, 0, 0); // draws a single pixel in the screen 
        sweep(data);
        //simulation logic
        drawBigPoint(xpos, ypos);
        // drawPrimaryRay(glm::vec3(3, 1.5, -30), glm::vec3(200, 200, 0));
        shootRay(glm::vec3(cpos.getVectX() + width/2, cpos.getVectY() +height/2, 0), glm::vec3(xpos, adjustY(ypos), 0), primary_ray);
        drawLightRays();        

        // Swap front and back buffers
        glfwSwapBuffers( window );
    
        // Poll for and process events
        glfwPollEvents( );
    }
}

GLfloat adjustY(double yVal){
    return (GLfloat)(-yVal + height);
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
}
static void cursorPositionCallback( GLFWwindow *window, double xpos, double ypos )
{
    std::cout << xpos << " : " << ypos << std::endl;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        cout << "Press\n";
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            cout << "Release\n";
        }
    }
}

void drawBigPoint(double xpos, double ypos){
    glEnable( GL_POINT_SMOOTH );
    glPointSize( 8 );
    glBegin(GL_POINTS);
    glColor3ub( 255, 255, 255 );
    glVertex2f((GLfloat)xpos, adjustY(ypos));
    glEnd();
}

void shootRay(glm::vec3 from, glm::vec3 to, glm::vec3 color){
	glPointSize(1);
	glLineWidth(1); 
	glColor3f(color.x, color.y, color.z);
	glBegin(GL_LINES);
    glVertex3f(from.x, from.y, from.z);
	glVertex3f(to.x, to.y, to.z);
	glEnd();
}
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{   
    if ( GLFW_KEY_ESCAPE == key && GLFW_PRESS == action )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void drawLightRays(){
    for (int light_index = 0; light_index < light_sources.size(); light_index++) {
        glm::vec3 pos = vec3Convert(light_sources[light_index]->getLightPosition());
        shootRay( glm::vec3(pos.x, adjustY(pos.y), 0), glm::vec3(xpos, adjustY(ypos), 0), glm::vec3(1.0f, 1.0f, 0.0f));
        if (light_index == 0) {
            cout <<  "\n" << glm::to_string(pos) << "SOURCE1" << endl;
        }
        else {
            cout << "\n" << glm::to_string(pos) << "SOURCE2" << endl;

        }
        
    }
}
glm::vec3 vec3Convert(Vect x){
    return glm::vec3((float)x.getVectX(), (float)x.getVectY(), (float)x.getVectZ());
}