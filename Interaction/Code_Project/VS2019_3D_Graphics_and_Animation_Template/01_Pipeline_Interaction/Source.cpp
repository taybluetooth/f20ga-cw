// 3D Graphics and Animation - Main Template
// This uses Visual Studio 2019 and other libraries - see notes in code and linker files.
// Last Changed 05/10/2021

#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <GL/glew.h>			// Add library to extend OpenGL to newest version
#include <GLFW/glfw3.h>			// Add library to launch a window
#include <GLM/glm.hpp>			// Add helper maths library
#include <GLM/gtx/transform.hpp>

#include <stb_image.h>			// Add library to load images for textures

#include "Mesh.h"				// Simplest mesh holder and OBJ loader - can update more - from https://github.com/BennyQBD/ModernOpenGLTutorial


// MAIN FUNCTIONS
void setupRender();
void startup();
void update(GLfloat currentTime);
void render(GLfloat currentTime);
void endProgram();

// HELPER FUNCTIONS OPENGL
void hintsGLFW();
string readShader(string name);
void checkErrorShader(GLuint shader);
void errorCallbackGLFW(int error, const char* description);
void debugGL();
static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam);

// CALLBACK FUNCTIONS FOR WINDOW
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);


// VARIABLES
GLFWwindow*		window;											
int				windowWidth = 640;				
int				windowHeight = 480;
bool			running = true;								
glm::mat4		proj_matrix;								
glm::vec3		cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float           aspect = (float)windowWidth / (float)windowHeight;
float			fovy = 45.0f;
bool			keyStatus[1024];
GLfloat			deltaTime = 0.0f;
GLfloat			lastTime = 0.0f;
GLint			proj_location;

// Positioning
glm::vec3		modelPosition;
glm::vec3		modelRotation;
glm::vec3		advertPos, streetPos, sewerPos, kerbPos, buildingFoundationPos, 
seatPos, lampPostPos, treeHolderPos, treeBasePos, leavesPos, externalWallPosition;

// Add OBJ files
Mesh			street;
Mesh			sewer;
Mesh			kerbSide;
Mesh			lampPost;
Mesh			treeHolder;
Mesh			treeBase;
Mesh			leaves;
Mesh			buildingFoundation;
Mesh			externalWall;
Mesh			seating;
Mesh			stairSteps;
Mesh			stairRailings;
Mesh			barBase;
Mesh			barCounter;
Mesh			ballLightWire;
Mesh			ballLights;
Mesh			tallSection;
Mesh			smallSection;
Mesh			doorFrame;
Mesh			doorDivider;
Mesh			doors;
Mesh			doorMat;
Mesh			balconyFloor;
Mesh			largeRoof;
Mesh			windowSill;
Mesh			windowBars;
Mesh			fence;
Mesh			sideRoofTiles;
Mesh			sideShelterBeams;
Mesh			topWindow;
Mesh			vents;
Mesh			electricBox;
Mesh			electricBoxWire;
Mesh			pipe;
Mesh			largeAdvert;
Mesh			smallAdvertRa;
Mesh			smallAdvertA;
Mesh			smallAdvertMe;
Mesh			smallAdvertN;
Mesh			roofEdge;
Mesh			outPipe;

// Shaders
GLuint			program;

// Textures
GLuint white;
GLuint metallic;
GLuint wood;
GLuint advert;

// Lighting
glm::vec4 ia = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
GLfloat ka = 1.0f;
glm::vec4 id = glm::vec4(0.1f, 0.5f, 0.8f, 1.0f);
glm::vec4 is = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 lightDisp = glm::vec4(1.0f, 1.0f, 1.0f, 2.0f);

// Global scaling constants
float g_Scale = 0.2f;
float g_MouseWheelScale = 0.1f;

int main()
{
	if (!glfwInit()) {							// Checking for GLFW
		cout << "Could not initialise GLFW...";
		return 0;
	}

	glfwSetErrorCallback(errorCallbackGLFW);	// Setup a function to catch and display all GLFW errors.

	hintsGLFW();								// Setup glfw with various hints.		

												// Start a window using GLFW
	string title = "3D Diorama Interaction";

	// Fullscreen
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	// windowWidth = mode->width; windowHeight = mode->height; //fullscreen
	// window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL); // fullscreen

	// Window
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
	if (!window) {								// Window or OpenGL context creation failed
		cout << "Could not initialise GLFW...";
		endProgram();
		return 0;
	}

	glfwMakeContextCurrent(window);				// making the OpenGL context current

												// Start GLEW (note: always initialise GLEW after creating your window context.)
	glewExperimental = GL_TRUE;					// hack: catching them all - forcing newest debug callback (glDebugMessageCallback)
	GLenum errGLEW = glewInit();
	if (GLEW_OK != errGLEW) {					// Problems starting GLEW?
		cout << "Could not initialise GLEW...";
		endProgram();
		return 0;
	}

	debugGL();									// Setup callback to catch openGL errors.	

												// Setup all the message loop callbacks.
	glfwSetWindowSizeCallback(window, onResizeCallback);		// Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);					// Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback);	// Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);		// Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);		// Set callback for mouse wheel.
																//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor. Fullscreen
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS fullscreen.

	setupRender();								// setup some render variables.
	startup();									// Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	do {										// run until the window is closed
		GLfloat currentTime = (GLfloat)glfwGetTime();		// retrieve timelapse
		deltaTime = currentTime - lastTime;		// Calculate delta time
		lastTime = currentTime;					// Save for next frame calculations.
		glfwPollEvents();						// poll callbacks
		update(currentTime);					// update (physics, animation, structures, etc)
		render(currentTime);					// call render function.

		glfwSwapBuffers(window);				// swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);	// exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram();			// Close and clean everything up...

	cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}

void errorCallbackGLFW(int error, const char* description) {
	cout << "Error GLFW: " << description << "\n";
}

void hintsGLFW() {
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);			// Create context in debug mode - for debug message callback
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void endProgram() {
	glfwMakeContextCurrent(window);		// destroys window handler
	glfwTerminate();	// destroys all windows and releases resources.
}

void setupRender() {
	glfwSwapInterval(1);	// Ony render when synced (V SYNC)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 2);
	glfwWindowHint(GLFW_STEREO, GL_FALSE);
}

void loadModels() {
	street.LoadModel("streetfloor_uv.obj");
	sewer.LoadModel("sewer_uv.obj");
	buildingFoundation.LoadModel("buildingfoundation_uv.obj");
	externalWall.LoadModel("externalwall_uv.obj");
	seating.LoadModel("seating_uv.obj");
	kerbSide.LoadModel("kerbside_uv.obj");
	lampPost.LoadModel("lamppost_uv.obj");
	treeHolder.LoadModel("treeholder_uv.obj");
	treeBase.LoadModel("treebase_uv.obj");
	leaves.LoadModel("leaves_uv.obj");
	stairSteps.LoadModel("stairsteps_uv.obj");
	stairRailings.LoadModel("stairrailings_uv.obj");
	barBase.LoadModel("barbase_uv.obj");
	barCounter.LoadModel("barcounter_uv.obj");
	ballLightWire.LoadModel("balllightwire_uv.obj");
	ballLights.LoadModel("balllights_uv.obj");
	tallSection.LoadModel("tallsection_uv.obj");
	smallSection.LoadModel("smallsection_uv.obj");
	doorFrame.LoadModel("doorframe_uv.obj");
	doorDivider.LoadModel("doordivider_uv.obj");
	doors.LoadModel("doors_uv.obj");
	doorMat.LoadModel("doormat_uv.obj");
	balconyFloor.LoadModel("balconyfloor_uv.obj");
	largeRoof.LoadModel("largeroof_uv.obj");
	windowSill.LoadModel("windowsill_uv.obj");
	windowBars.LoadModel("windowbars_uv.obj");
	fence.LoadModel("fence_uv.obj");
	sideRoofTiles.LoadModel("siderooftiles_uv.obj");
	sideShelterBeams.LoadModel("sideshelterbeams_uv.obj");
	topWindow.LoadModel("topwindow_uv.obj");
	vents.LoadModel("vents_uv.obj");
	electricBox.LoadModel("electricbox_uv.obj");
	electricBoxWire.LoadModel("electricboxwire_uv.obj");
	pipe.LoadModel("pipe_uv.obj");
	smallAdvertRa.LoadModel("smalladvertra_uv.obj");
	smallAdvertA.LoadModel("smalladverta_uv.obj");
	smallAdvertMe.LoadModel("smalladvertme_uv.obj");
	smallAdvertN.LoadModel("smalladvertn_uv.obj");
	largeAdvert.LoadModel("largeadvert_uv.obj");
	outPipe.LoadModel("outpipe_uv.obj");
	roofEdge.LoadModel("roofedge_uv.obj");
}

void startup() {
	
	// load obj files
	loadModels();

	program = glCreateProgram();

	string vs_text = readShader("./shaders/vs_model.glsl"); const char* vs_source = vs_text.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	checkErrorShader(vs);
	glAttachShader(program, vs);

	string fs_text = readShader("./shaders/fs_model.glsl"); const char* fs_source = fs_text.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	checkErrorShader(fs);
	glAttachShader(program, fs);

	// flat lighting texture -------------
	glGenTextures(1, &white);
	glBindTexture(GL_TEXTURE_2D, white);
	// set wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// image info
	int imWidth, imHeight, imChannels;
	unsigned char* imData = stbi_load("textures/white.png", &imWidth, &imHeight, &imChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imWidth, imHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imData);
	glLinkProgram(program);

	// metallic lighting texture -------------
	glGenTextures(1, &metallic);
	glBindTexture(GL_TEXTURE_2D, metallic);
	// set wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// image info
	imData = stbi_load("textures/metallic.png", &imWidth, &imHeight, &imChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imWidth, imHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imData);
	glLinkProgram(program);

	// wood texture -------------
	glGenTextures(1, &wood);
	glBindTexture(GL_TEXTURE_2D, wood);
	// set wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// image info
	imData = stbi_load("textures/wood.png", &imWidth, &imHeight, &imChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imWidth, imHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imData);

	glLinkProgram(program);
	glUseProgram(program);

	// Start from the centre
	modelPosition, streetPos, sewerPos, kerbPos, buildingFoundationPos,
	seatPos, lampPostPos, treeHolderPos, treeBasePos, leavesPos, externalWallPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	advertPos = glm::vec3(0.05, -0.05, 0.3);
	
	// A few optimizations.
	glFrontFace(GL_CCW);
	// no face culling to improve sense of depth.
	// uncomment for improved performance.
	//glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

// color texture using rgb parameters
void colorObject(float r, float g, float b) {
	GLint color_location = glGetUniformLocation(program, "id");
	float color[4] = { r, g, b, 1.0 };
	glUniform4fv(color_location, 1, color);
}

// update control of diorama camera
void update(GLfloat currentTime) {
	// model translations
	if (keyStatus[GLFW_KEY_LEFT])			modelRotation.y += 0.05f;
	if (keyStatus[GLFW_KEY_RIGHT])			modelRotation.y -= 0.05f;
	if (keyStatus[GLFW_KEY_UP])				modelRotation.x += 0.05f;
	if (keyStatus[GLFW_KEY_DOWN])			modelRotation.x -= 0.05f;
	// explode external aspects of diorama
	if (keyStatus[GLFW_KEY_E]) {
		streetPos.y -= 0.03;
		advertPos.y += 0.03;
		externalWallPosition.x -= 0.01;
		buildingFoundationPos.y -= 0.02;
		kerbPos.y -= 0.1;
		sewerPos.y -= 0.05;
		lampPostPos.x -= 0.02;
		treeHolderPos.y -= 0.01;
		treeBasePos.x += 0.01;
		leavesPos.y += 0.01;
		seatPos.x -= 0.01;
	}
	// retract external aspects of diorama
	if (keyStatus[GLFW_KEY_R]) {
		streetPos.y += 0.03;
		advertPos.y -= 0.03;
		externalWallPosition.x += 0.01;
		buildingFoundationPos.y += 0.02;
		kerbPos.y += 0.1;
		sewerPos.y += 0.05;
		lampPostPos.x += 0.02;
		treeHolderPos.y += 0.01;
		treeBasePos.x -= 0.01;
		leavesPos.y -= 0.01;
		seatPos.x += 0.01;
	}

	// camera translations
	if (keyStatus[GLFW_KEY_W])				cameraPosition.z -= 0.05f;
	if (keyStatus[GLFW_KEY_A])				cameraPosition.x -= 0.05f;
	if (keyStatus[GLFW_KEY_S])				cameraPosition.z += 0.05f;
	if (keyStatus[GLFW_KEY_D])				cameraPosition.x += 0.05f;
	if (keyStatus[GLFW_KEY_SPACE])			cameraPosition.y += 0.05f;
	if (keyStatus[GLFW_KEY_TAB])			cameraPosition.y -= 0.05f;
}

void render(GLfloat currentTime) {
	glViewport(0, 0, windowWidth, windowHeight);

	// Clear colour buffer
	glm::vec4 backgroundColor = glm::vec4(0.294, 0.000, 0.510, 1.0f); glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

	// Clear deep buffer
	static const GLfloat one = 1.0f; glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader programs
	glUseProgram(program);

	// Setup camera
	glm::mat4 viewMatrix = glm::lookAt(cameraPosition,					// eye
		cameraPosition + cameraFront,									// centre
		cameraUp);														// up

	// Set uniforms
	glUniform4f(glGetUniformLocation(program, "viewPosition"),
		cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(program, "lightPosition"),
		lightDisp.x, lightDisp.y, lightDisp.z, 1.0);
	glUniform3f(glGetUniformLocation(program, "lightColor"), 1.0f, 1.0f, 1.0f);
	glUniform4f(glGetUniformLocation(program, "ia"), ia.r, ia.g, ia.b,
		1.0);
	glUniform1f(glGetUniformLocation(program, "ka"), ka);
	glUniform4f(glGetUniformLocation(program, "id"), id.r, id.g, id.b,
		1.0);
	glUniform1f(glGetUniformLocation(program, "kd"), 1.0f);
	glUniform4f(glGetUniformLocation(program, "is"), is.r, is.g, is.b,
		1.0);
	glUniform1f(glGetUniformLocation(program, "ks"), 1.0f);
	glUniform1f(glGetUniformLocation(program, "shininess"), 16.0f);

	// Set active texture to flat white texture
	GLint whiteLocation = glGetUniformLocation(program, "white");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, white);
	glUniform1i(whiteLocation, 0);

	// Set position of street
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), streetPos);
	modelMatrix = glm::rotate(modelMatrix, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(g_Scale, g_Scale, g_Scale));

	glm::mat4 mv_matrix = viewMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);
	
	colorObject(0.000, 0.000, 0.000);
	// increase shininess to give reflective street
	glUniform1f(glGetUniformLocation(program, "shininess"), 2.0f);
	glUniform4f(glGetUniformLocation(program, "ia"), 0.1f, 0.1f, 0.1f, 1.0f);
	street.Draw();
	
	glUniform4f(glGetUniformLocation(program, "ia"), ia.r, ia.g, ia.b,
		1.0);

	// Set position of sewer
	glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), sewerPos);
	modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(g_Scale, g_Scale, g_Scale));

	glm::mat4 mv_matrix2 = viewMatrix * modelMatrix2;

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix2[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);

	// decrease shininess to give flat lighting
	glUniform1f(glGetUniformLocation(program, "shininess"), 16.0f);

	colorObject(0.663, 0.663, 0.663);
	sewer.Draw();

	// Set position of building foundation
	glm::mat4 modelMatrix3 = glm::translate(glm::mat4(1.0f), buildingFoundationPos);
	modelMatrix3 = glm::rotate(modelMatrix3, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix3 = glm::rotate(modelMatrix3, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix3 = glm::scale(modelMatrix3, glm::vec3(g_Scale, g_Scale, g_Scale));

	glm::mat4 mv_matrix3 = viewMatrix * modelMatrix3;

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix3[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);

	colorObject(1.000, 0.937, 0.835);
	buildingFoundation.Draw();
	
	// Set position of external wall
	glm::mat4 modelMatrix4 = glm::translate(glm::mat4(1.0f), externalWallPosition);
	modelMatrix4 = glm::rotate(modelMatrix4, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix4 = glm::rotate(modelMatrix4, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix4 = glm::scale(modelMatrix4, glm::vec3(g_Scale, g_Scale, g_Scale));

	glm::mat4 mv_matrix4 = viewMatrix * modelMatrix4;

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix4[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);

	colorObject(0.502, 0.502, 0.502);
	externalWall.Draw();


	// Set position of kerbside
	glm::mat4 modelMatrix5 = glm::translate(glm::mat4(1.0f), kerbPos);
	modelMatrix5 = glm::rotate(modelMatrix5, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix5 = glm::rotate(modelMatrix5, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix5 = glm::scale(modelMatrix5, glm::vec3(g_Scale, g_Scale, g_Scale));

	glm::mat4 mv_matrix5 = viewMatrix * modelMatrix5;

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix5[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);

	colorObject(0.502, 0.502, 0.502);
	kerbSide.Draw();


	// Set position of tree's holder
	glm::mat4 modelMatrix6 = glm::translate(glm::mat4(1.0f), treeHolderPos);
	modelMatrix6 = glm::rotate(modelMatrix6, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix6 = glm::rotate(modelMatrix6, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix6 = glm::scale(modelMatrix6, glm::vec3(g_Scale, g_Scale, g_Scale));

	glm::mat4 mv_matrix6 = viewMatrix * modelMatrix6;

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix6[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);

	colorObject(0.961, 0.961, 0.961);
	treeHolder.Draw();

	// Set position of tree base trunk
	glm::mat4 modelMatrix7 = glm::translate(glm::mat4(1.0f), treeBasePos);
	modelMatrix7 = glm::rotate(modelMatrix7, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix7 = glm::rotate(modelMatrix7, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix7 = glm::scale(modelMatrix7, glm::vec3(g_Scale, g_Scale, g_Scale));

	glm::mat4 mv_matrix7 = viewMatrix * modelMatrix7;

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix7[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);

	colorObject(0.545, 0.271, 0.075);
	treeBase.Draw();

	// Set position of leaves
	glm::mat4 modelMatrix8 = glm::translate(glm::mat4(1.0f), leavesPos);
	modelMatrix8 = glm::rotate(modelMatrix8, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix8 = glm::rotate(modelMatrix8, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix8 = glm::scale(modelMatrix8, glm::vec3(g_Scale, g_Scale, g_Scale));

	glm::mat4 mv_matrix8 = viewMatrix * modelMatrix8;

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix8[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);

	colorObject(0.933, 0.510, 0.933);
	leaves.Draw();

	// Set position of rest of diorama not being manipulated
	glm::mat4 modelMatrix9 = glm::translate(glm::mat4(1.0f), modelPosition);
	modelMatrix9 = glm::rotate(modelMatrix9, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix9 = glm::rotate(modelMatrix9, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix9 = glm::scale(modelMatrix9, glm::vec3(g_Scale, g_Scale, g_Scale));

	glm::mat4 mv_matrix9 = viewMatrix * modelMatrix9;

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix9[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);

	colorObject(0.502, 0.502, 0.502);
	roofEdge.Draw();
	colorObject(0.627, 0.322, 0.176);
	stairSteps.Draw();
	colorObject(0.000, 0.000, 0.000);
	stairRailings.Draw();
	colorObject(0.000, 0.000, 0.000);
	ballLightWire.Draw();
	electricBoxWire.Draw();
	colorObject(0.859, 0.439, 0.576);
	ballLights.Draw();
	// increase shininess to give impression of shadows
	// improves visibility of building geometry
	glUniform1f(glGetUniformLocation(program, "shininess"), 1.0f);
	colorObject(1.000, 1.000, 1.000);
	tallSection.Draw();
	smallSection.Draw();
	// decrease shininess to give flat lighting
	glUniform1f(glGetUniformLocation(program, "shininess"), 16.0f);
	balconyFloor.Draw();
	colorObject(0.980, 0.502, 0.447);
	largeRoof.Draw();
	sideRoofTiles.Draw();
	colorObject(0.545, 0.271, 0.075);
	smallAdvertRa.Draw();
	smallAdvertA.Draw();
	smallAdvertMe.Draw();
	smallAdvertN.Draw();
	colorObject(0.871, 0.722, 0.529);
	windowSill.Draw();
	doorMat.Draw();

	// Set active texture to be wood texture
	GLint woodLocation = glGetUniformLocation(program, "wood");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wood);
	glUniform1i(woodLocation, 0);

	colorObject(0.722, 0.525, 0.043);
	doorFrame.Draw();
	fence.Draw();

	colorObject(0.871, 0.722, 0.529);
	doorDivider.Draw();

	colorObject(0.957, 0.643, 0.376);
	doors.Draw();
	windowBars.Draw();
	topWindow.Draw();
	sideShelterBeams.Draw();

	colorObject(0.627, 0.322, 0.176);
	barBase.Draw();

	colorObject(1.000, 0.871, 0.678);
	barCounter.Draw();

	// Set active texture to be metal texture
	GLint metallicLocation = glGetUniformLocation(program, "metallic");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, metallic);
	glUniform1i(metallicLocation, 0);
	// increase shininess to give shiny metallic effect
	glUniform1f(glGetUniformLocation(program, "shininess"), 0.5f);
	colorObject(0.412, 0.412, 0.412);
	pipe.Draw();
	outPipe.Draw();
	electricBox.Draw();
	colorObject(0.914, 0.588, 0.478);
	vents.Draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, white);
	glUniform1i(whiteLocation, 0);

	// Set position of lamp post
	glm::mat4 modelMatrix10 = glm::translate(glm::mat4(1.0), lampPostPos);
	modelMatrix10 = glm::rotate(modelMatrix10, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix10 = glm::rotate(modelMatrix10, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix10 = glm::scale(modelMatrix10, glm::vec3(g_Scale, g_Scale, g_Scale));
	glm::mat4 mv_matrix10 = viewMatrix * modelMatrix10;

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix10[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);

	colorObject(0.000, 0.000, 0.000);
	lampPost.Draw();

	// Set position of advert
	glm::mat4 modelMatrix11 = glm::translate(glm::mat4(1.0), advertPos);
	modelMatrix11 = glm::rotate(modelMatrix11, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix11 = glm::rotate(modelMatrix11, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix11 = glm::scale(modelMatrix11, glm::vec3(g_Scale, g_Scale, g_Scale));
	glm::mat4 mv_matrix11 = viewMatrix * modelMatrix11;

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix11[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);


	colorObject(1.000, 1.000, 1.000);
	glUniform4f(glGetUniformLocation(program, "ia"), 1.0f, 1.0f, 1.0f, 1.0f);
	largeAdvert.Draw();

	// Set position of seating
	glm::mat4 modelMatrix12 = glm::translate(glm::mat4(1.0), seatPos);
	modelMatrix12 = glm::rotate(modelMatrix12, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix12 = glm::rotate(modelMatrix12, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix12 = glm::scale(modelMatrix12, glm::vec3(g_Scale, g_Scale, g_Scale));
	glm::mat4 mv_matrix12 = viewMatrix * modelMatrix12;

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix12[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);

	// Set active texture to be metallic
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, metallic);
	glUniform1i(metallicLocation, 0);
	glUniform1f(glGetUniformLocation(program, "shininess"), 0.5f);

	colorObject(0.827, 0.827, 0.827);
	seating.Draw();

}

void onResizeCallback(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;

	if (windowWidth > 0 && windowHeight > 0) { // Avoid issues when minimising window - it gives size of 0 which fails division.
		aspect = (float)w / (float)h;
		proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
	}
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) keyStatus[key] = true;
	else if (action == GLFW_RELEASE) keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);
}

static void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset) {
	g_Scale += g_MouseWheelScale * (float)yoffset;
}

void debugGL() {
	//Output some debugging information
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;

	// Enable Opengl Debug
	//glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam) {

	cout << "---------------------opengl-callback------------" << endl;
	cout << "Message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << " --- ";

	cout << "id: " << id << " --- ";
	cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		cout << "NOTIFICATION";
	}
	cout << endl;
	cout << "-----------------------------------------" << endl;
}

string readShader(string name) {
	string vs_text;
	std::ifstream vs_file(name);

	string vs_line;
	if (vs_file.is_open()) {

		while (getline(vs_file, vs_line)) {
			vs_text += vs_line;
			vs_text += '\n';
		}
		vs_file.close();
	}
	return vs_text;
}

void  checkErrorShader(GLuint shader) {
	// Get log lenght
	GLint maxLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	// Init a string for it
	std::vector<GLchar> errorLog(maxLength);

	if (maxLength > 1) {
		// Get the log file
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		cout << "--------------Shader compilation error-------------\n";
		cout << errorLog.data();
	}
}
