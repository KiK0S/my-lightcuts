// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------

#define _USE_MATH_DEFINES

#include <glad/glad.h>

#include <cstdlib>
#include <cstdio>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include <algorithm>
#include <exception>
#include <random>
#include <filesystem>

namespace fs = std::filesystem;

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Resources.h"
#include "Error.h"
#include "Console.h"
#include "MeshLoader.h"
#include "Scene.h"
#include "Image.h"
#include "Rasterizer.h"
#include "RayTracer.h"
#include "Model.hpp"
#include "Material.hpp"
#include "LightSource.hpp"

using namespace std;

// Window parameters
static GLFWwindow * windowPtr = nullptr;
static std::shared_ptr<Scene> scenePtr;
static std::shared_ptr<Rasterizer> rasterizerPtr;
static std::shared_ptr<RayTracer> rayTracerPtr;

// Camera control variables
static glm::vec3 center = glm::vec3 (0.0); // To update based on the mesh position
static float meshScale = 1.0; // To update based on the mesh size, so that navigation runs at scale
static bool isRotating (false);
static bool isPanning (false);
static bool isZooming (false);
static double baseX (0.0), baseY (0.0);
static glm::vec3 baseTrans (0.0);
static glm::vec3 baseRot (0.0);

// Files
static std::string basePath;
static std::string meshFilename;

// Raytraced rendering
static bool isDisplayRaytracing (false);

void clear ();

void printHelp () {
	Console::print (std::string ("Help:\n") 
			  + "\tMouse commands:\n" 
			  + "\t* Left button: rotate camera\n" 
			  + "\t* Middle button: zoom\n" 
			  + "\t* Right button: pan camera\n" 
			  + "\tKeyboard commands:\n" 
   			  + "\t* ESC: quit the program\n"
   			  + "\t* H: print this help\n"
   			  + "\t* F12: reload GPU shaders\n"
   			  + "\t* F: decrease field of view\n"
   			  + "\t* G: increase field of view\n"
   			  + "\t* TAB: switch between rasterization and ray tracing display\n"
   			  + "\t* SPACE: execute ray tracing\n");
}

/// Adjust the ray tracer target resolution and runs it.
void raytrace () {
	int width, height;
	glfwGetWindowSize(windowPtr, &width, &height);
	rayTracerPtr->setResolution (width, height);
	rayTracerPtr->render (scenePtr);
}

/// Executed each time a key is entered.
void keyCallback (GLFWwindow * windowPtr, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_H) {
			printHelp ();
		} else if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose (windowPtr, true); // Closes the application if the escape key is pressed
		} else if (action == GLFW_PRESS && key == GLFW_KEY_F12) {
			rasterizerPtr->loadShaderProgram (basePath);
		} else if (action == GLFW_PRESS && key == GLFW_KEY_F) {
			scenePtr->camera()->setFoV (std::max (5.f, scenePtr->camera()->getFoV () - 5.f));
		} else if (action == GLFW_PRESS && key == GLFW_KEY_G) {
			scenePtr->camera()->setFoV (std::min (120.f, scenePtr->camera()->getFoV () + 5.f));
		} else if (action == GLFW_PRESS && key == GLFW_KEY_TAB) {
			isDisplayRaytracing = !isDisplayRaytracing;
		} else if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
			raytrace ();
		} else {
			printHelp ();
		}
	}
}

/// Called each time the mouse cursor moves
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	int width, height;
	glfwGetWindowSize (windowPtr, &width, &height);
	float normalizer = static_cast<float> ((width + height)/2);
	float dx = static_cast<float> ((baseX - xpos) / normalizer);
	float dy = static_cast<float> ((ypos - baseY) / normalizer);
	if (isRotating) {
		glm::vec3 dRot (-dy * M_PI, dx * M_PI, 0.0);
		scenePtr->camera()->setRotation (baseRot + dRot);
	} else if (isPanning) {
		scenePtr->camera()->setTranslation (baseTrans + meshScale * glm::vec3 (dx, dy, 0.0));
	} else if (isZooming) {
		scenePtr->camera()->setTranslation (baseTrans + meshScale * glm::vec3 (0.0, 0.0, dy));
	}
}

/// Called each time a mouse button is pressed
void mouseButtonCallback (GLFWwindow * window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    	if (!isRotating) {
    		isRotating = true;
    		glfwGetCursorPos (window, &baseX, &baseY);
    		baseRot = scenePtr->camera()->getRotation ();
        } 
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    	isRotating = false;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    	if (!isPanning) {
    		isPanning = true;
    		glfwGetCursorPos (window, &baseX, &baseY);
    		baseTrans = scenePtr->camera()->getTranslation ();
        } 
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
    	isPanning = false;
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    	if (!isZooming) {
    		isZooming = true;
    		glfwGetCursorPos (window, &baseX, &baseY);
    		baseTrans = scenePtr->camera()->getTranslation ();
        } 
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
    	isZooming = false;
    }
}

/// Executed each time the window is resized. Adjust the aspect ratio and the rendering viewport to the current window. 
void windowSizeCallback (GLFWwindow * windowPtr, int width, int height) {
	scenePtr->camera()->setAspectRatio (static_cast<float>(width) / static_cast<float>(height));
	rasterizerPtr->setResolution (width, height);
	rayTracerPtr->setResolution (width, height);
}

void initGLFW () {
	// Initialize GLFW, the library responsible for window management
	if (!glfwInit ()) {
		Console::print ("ERROR: Failed to init GLFW");
		std::exit (EXIT_FAILURE);
	}

	// Before creating the window, set some option flags
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint (GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // added to fix "Failed to open window" crash on MacOS

	// Create the window
	windowPtr = glfwCreateWindow (1024, 768, BASE_WINDOW_TITLE.c_str (), nullptr, nullptr);
	if (!windowPtr) {
		Console::print ("ERROR: Failed to open window");
		glfwTerminate ();
		std::exit (EXIT_FAILURE);
	}

	// Load the OpenGL context in the GLFW window using GLAD OpenGL wrangler
	glfwMakeContextCurrent (windowPtr);

	// Connect the callbacks for interactive control 
	glfwSetWindowSizeCallback (windowPtr, windowSizeCallback);
	glfwSetKeyCallback (windowPtr, keyCallback);
	glfwSetCursorPosCallback (windowPtr, cursorPosCallback);
	glfwSetMouseButtonCallback (windowPtr, mouseButtonCallback);
}

void initScene () {
	scenePtr = std::make_shared<Scene> ();
	scenePtr->setBackgroundColor (glm::vec3 (0.1f, 0.5f, 0.95f));

	// Mesh
	auto meshPtr = std::make_shared<Mesh> ();
	try {
		if (meshFilename[meshFilename.size() - 1] == 'j')
			MeshLoader::loadOBJ (meshFilename, meshPtr);
		if (meshFilename[meshFilename.size() - 1] == 'f')
			MeshLoader::loadOFF (meshFilename, meshPtr);
	} catch (std::exception & e) {
		exitOnCriticalError (std::string ("[Error loading mesh]") + e.what ());
	}
	meshPtr->computeBoundingSphere (center, meshScale);
	auto modelPtr = std::make_shared<Model>(meshPtr, std::make_shared<Material>(glm::vec4(0.6, 0.9, 0.4, 1.0), 16, 0.2, 0.4, 0.4));
	scenePtr->add (modelPtr); 
	// scenePtr->add (std::make_shared<DirectionalLight>(glm::vec3(0.0, 0.3, 1.0), glm::vec3(1.0, 1.0, 1.0), 0.7f));
	// scenePtr->add (std::make_shared<DirectionalLight>(glm::vec3(0.0, 1.0, 0.1), glm::vec3(1.0, 0.0, 0.0), 0.2f));
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<float> r_dist(0, 1);
	std::uniform_real_distribution<float> x_dist(-meshScale, meshScale);
	std::uniform_real_distribution<float> y_dist(-meshScale, meshScale);
	std::uniform_real_distribution<float> z_dist(-meshScale, meshScale);
	
	for (int i = 0; i < 10; i++) {
		scenePtr->add (std::make_shared<PointLight>(glm::vec3(x_dist(gen), y_dist(gen), z_dist(gen)), glm::vec3(r_dist(gen), r_dist(gen), r_dist(gen)), 20.f));
	}

	// Camera
	int width, height;
	glfwGetWindowSize (windowPtr, &width, &height);
	auto cameraPtr = std::make_shared<Camera> ();
	cameraPtr->setAspectRatio (static_cast<float>(width) / static_cast<float>(height));
	cameraPtr->setTranslation (center + glm::vec3 (0.0, 0.0, 3.0 * meshScale));
	cameraPtr->setNear (0.1f);
	cameraPtr->setFar (100.f * meshScale);
	scenePtr->set (cameraPtr);
}

void init () {
	initGLFW (); // Windowing system
	if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress)) // Load extensions for modern OpenGL
		exitOnCriticalError ("[Failed to initialize OpenGL context]");
	initScene (); // Actual scene to render
	rasterizerPtr = make_shared<Rasterizer> ();
	rasterizerPtr->init (basePath, scenePtr); // Mut be called before creating the scene, to generate an OpenGL context and allow mesh VBOs
	rayTracerPtr = make_shared<RayTracer> ();
	rayTracerPtr->init (scenePtr);
}

void clear () {
	glfwDestroyWindow (windowPtr);
	glfwTerminate ();
}


// The main rendering call
void render () {
	if (isDisplayRaytracing)
		rasterizerPtr->display (rayTracerPtr->image ());
	else
		rasterizerPtr->render (scenePtr);
}

// Update any accessible variable based on the current time
void update (float currentTime) {
	// Animate any entity of the program here
	static const float initialTime = currentTime;
	static float lastTime = 0.f;
	static unsigned int frameCount = 0;
	static float fpsTime = currentTime;
	static unsigned int FPS = 0;
	float elapsedTime = currentTime - initialTime;
	float dt = currentTime - lastTime;
	if (frameCount == 99) {
		float delai = (currentTime - fpsTime)/100;
		FPS = static_cast<unsigned int> (1.f/delai);
		frameCount = 0;
		fpsTime = currentTime;
	}
	std::string titleWithFPS = BASE_WINDOW_TITLE + " - " + std::to_string (FPS) + "FPS";
	glfwSetWindowTitle (windowPtr, titleWithFPS.c_str ());
	lastTime = currentTime;
	frameCount++;
}

void usage (const char * command) {
	Console::print ("Usage : " + std::string(command) + " [<meshfile.off>]");
	std::exit (EXIT_FAILURE);
}

void parseCommandLine (int argc, char ** argv) {
	if (argc > 3)
		usage (argv[0]);
	basePath = "./";
	meshFilename = (argc >= 2 ? argv[1] : DEFAULT_MESH_FILENAME);
}

int main (int argc, char ** argv) {
	parseCommandLine (argc, argv);
	init (); 
	while (!glfwWindowShouldClose (windowPtr)) {
		update (static_cast<float> (glfwGetTime ()));
		render ();
		glfwSwapBuffers (windowPtr);
		glfwPollEvents ();
	}
	clear ();
	Console::print ("Quit");
	return EXIT_SUCCESS;
}

