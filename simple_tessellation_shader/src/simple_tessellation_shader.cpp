
#include <stdio.h>
#include <algorithm>
#include <iostream>

#include "GL/glew.h"
#include "FreeImage.h"
#include "sdk_swak.h"

#include "icosahedron.hpp"
#include "cylinder.hpp"
#include "objFormatter.hpp"
#include "Shader.h"
#include "GeneralizedCylinder.h"
#include "Path.h"
#include "Contour.h"
#include "Image.h"

//shaders
unsigned sProgram;
unsigned shaderIds[5] = { 0u };
unsigned texBufferIds[1] = { 0u };

//uniform locations
unsigned MVPlocation		= 0;
unsigned MVlocation			= 0;
unsigned inTessLocation		= 0;
unsigned outTessLocation	= 0;
unsigned lodLocation		= 0;
unsigned distanceLocation	= 0;
unsigned lightLocation		= 0;
unsigned litLocation		= 0;
unsigned texLocation		= 0;

// Flag to identify when the app needs to close
bool running = true;

//widgets handle
nv::SdkUIContext ui;

//uniform buffer object
GLuint ubo = 0;

/*******************************************************************************
 *
 * Reflection of uniform buffer object
 *
 *  Special defines + including file shared with shaders to declare a struct
 * that maps the state directly
 *
 *******************************************************************************/

//define to make sure the header understands the compilation mode
#define CPP

// now just include the header all the shader use
#include "uniforms.h"

// declare a global struct that holds the uniform data
TessellationParams tessellationParameters;

/*******************************************************************************
 *
 * Control variables
 *
 *  Variables used to bind control toggles and ranges to the sample
 *  Variables auto-register themselves with a singleton which handles the
 * routing of key presses.
 *
 *******************************************************************************/
nv::CVar<bool> lit( "lit", false, nv::KeyCode::Key_L, "Toggle lighting 'l'");
nv::CVar<bool> lod( "lod", true, nv::KeyCode::Key_K, "Toggle lod mode 'k'");
nv::CRefVar<float> innerTess( tessellationParameters.innerTessFactor, "InnerTessFactor", 1.0f, 1.0f, 64.0f, nv::KeyCode::Key_I, "Change tesselation factor");
nv::CRefVar<float> outerTess( tessellationParameters.outerTessFactor, "OuterTessFactor", 1.0f, 1.0f, 64.0f, nv::KeyCode::Key_O, "Change tesselation factor");
nv::CRefVar<float> distance( tessellationParameters.distance, "distance", 1.0f, 1.0f, 64.0f, nv::KeyCode::Key_P, "Change distance");

unsigned mode = 1;

/*******************************************************************************
 *
 * Callback Functions
 *
 *******************************************************************************/

//
//  Function: key
//
//    Callback function registered with SDK_swak to note keypresses
//
////////////////////////////////////////////////////////////////////////////////
void key( nv::KeyCode::KeyCode kc)
{
	switch (kc)
	{
		case nv::KeyCode::Key_Q:
		case nv::KeyCode::Key_Escape:
			running = false;
			break;
		case nv::KeyCode::Key_1:
			mode = 1;			
			break;
		case nv::KeyCode::Key_2:
			mode = 2;			
			break;
		case nv::KeyCode::Key_3:
			mode = 3;			
			break;
	};

	int x, y;
	nv::GetMousePosition( x, y);
	ui.keyboard( kc, x, y);
	//pass key presses to the control variable registry
	nv::CVarRegistry::Instance().ProcessKey(kc, nv::GetModifierKeyState());
}

void mouse( nv::MouseButton::MouseButton button, bool down)
{
	int x, y;
	nv::GetMousePosition( x, y);
	ui.mouse( button, int(down), x, y);
}

void loadTexture(int tID, const char* filepath) {
	FreeImage_Initialise();
	FIBITMAP* _bitmap;

	_bitmap = FreeImage_Load(FreeImage_GetFileType(filepath, 0),
							 filepath);

	if(!_bitmap){
		std::cout << "texture: \'" << filepath << "\' didn't load\n";
		return;
	}

	unsigned char* pixelData = FreeImage_GetBits(_bitmap);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texBufferIds[tID]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
				FreeImage_GetWidth(_bitmap),
				FreeImage_GetHeight(_bitmap),
				0, GL_BGR, GL_UNSIGNED_BYTE,
				pixelData
	);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void init()
{
	// setup some defines for the shaders
	static const char* defines[] = 
	{
		"ORANGE_PURPLE", "1",
		NULL, NULL
	};
	
	//register an include file that the shaders use to define a common uniform block
	nv::RegisterShaderInclude( "uniforms.h");

	//setup shader program
	sProgram = glCreateProgram();
	{
		//create stages
		shaderIds[0] = Shader::loadShader("vertex.glsl", GL_VERTEX_SHADER);
		shaderIds[1] = Shader::loadShader("fragment.glsl", GL_FRAGMENT_SHADER);
		/*shaderIds[2] = Shader::loadShader("tess_control.glsl", GL_TESS_CONTROL_SHADER);
		shaderIds[3] = Shader::loadShader("tess_eval.glsl", GL_TESS_EVALUATION_SHADER);
		shaderIds[4] = Shader::loadShader("geometry.glsl", GL_GEOMETRY_SHADER);*/

		//attach stages to program
		glAttachShader(sProgram, shaderIds[0]);
		glAttachShader(sProgram, shaderIds[1]);
		/*glAttachShader(sProgram, shaderIds[2]);
		glAttachShader(sProgram, shaderIds[3]);
		glAttachShader(sProgram, shaderIds[4]);*/
	}
	glLinkProgram(sProgram);

	//generate textures
	glGenTextures(1, texBufferIds);
	loadTexture(0, "../data/textures/stone.jpg");
	
	/*
	Image img("../data/textures/stone.jpg");

	unsigned int width = img.getWidth();
	unsigned int height = img.getHeight();
	nv::vec3f add(0.8,0.2,0.1);
	for(unsigned int x = 0; x < 649; ++x){
		for(unsigned int y = 0; y < 649; ++y){
			img.setPixel(x, y, 0.5*img.getPixel(x,y) + 0.5*add);
		}
	}

	if(img.saveToFile(FIF_JPEG, "../data/textures/stone2.jpg"))
		std::cout << "successfully saved image\n" << std::endl; 
	*/
	
	glBindTexture(GL_TEXTURE_2D, 0); //safety unbind
	
	//uniform locations
	MVPlocation			= glGetUniformLocation(sProgram, "ModelViewProjection");
	MVlocation			= glGetUniformLocation(sProgram, "ModelView");
	inTessLocation		= glGetUniformLocation(sProgram, "innerTessLevel");
	outTessLocation		= glGetUniformLocation(sProgram, "outerTessLevel");
	lodLocation			= glGetUniformLocation(sProgram, "lod");
	distanceLocation	= glGetUniformLocation(sProgram, "distance");
	lightLocation		= glGetUniformLocation(sProgram, "light");
	litLocation			= glGetUniformLocation(sProgram, "lit");
	texLocation			= glGetUniformLocation(sProgram, "tex");

	GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
		exit(-2);
    }
}

int main( int argc, char **argv)
{
	// Initialize window system portion of library (potenially refactor to have library-wide init?)
	nv::InitWindowSystem();

	// Create window offers no options, to more closely represent the constrained tablet environment
	//  Window is always double-buffered with Depth
	//  On Windows, the size defaults to 720p
	nv::CreateWindow();

	// Calback function registration
	nv::RegisterKeydownCallback( key);
	nv::RegisterMouseButtonCallback( mouse);

	glewInit();

	printf( "GL version %s\n", glGetString(GL_VERSION));

	if (!glewIsSupported("GL_VERSION_4_2"))
	{
        printf("Sample requires 4.2\n");
		return -1; 
	}

	if (glewIsSupported("GL_ARB_shading_language_include"))
		printf("Supports glsl include\n");

	init();

	printf( "\nCommands:\n");
	nv::CVarRegistry::Instance().PrintHelp();

	//
	// Explicit run loop, unlike GLUT, allows polling instead of callbacks
	//   Probably want to add callback for rendering and RunLoop function toallow passing the responsibility off
	//   May also want inlined C++ classexposing these as methods to allow a more OOP-like interface 
	//

	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	printf("Max supported patch vertices %d\n", MaxPatchVertices);

	//////GENERALIZED CYLINDER//////
	
	//path
	nv::matrix4f start, mid, end;
	start.make_identity();
	end.make_identity();
	//mid.make_identity();
	start.set_translate(nv::vec3f(0.0f,-1.0f,0.0f));
	//mid.set_translate(nv::vec3f(0.0f,0.0f,0.0f));
	end.set_translate(nv::vec3f(0.0f,1.0f,0.0f));
	
	Path path(start);
	//path.addSegment(start);
	//path.addSegment(mid);
	path.addSegment(end);
	path.calculate();
	
	//contour
	Contour contour;

	//thickness
	std::vector<float> thickness;
	thickness.push_back(1.0f);
	//thickness.push_back(1.0f);
	thickness.push_back(1.0f);
	
	//water
	std::vector<float> water;
	water.push_back(1.0f);
	//water.push_back(1.0f);
	water.push_back(1.0f);

	///////////////////////////////

	Icosahedron ico;
	Cylinder cyl;
	GeneralizedCylinder genCyl(path, contour, thickness, water, 1);
	//ObjFormatter obj("cylinder_right.obj");

	float angle = 0.0f;
	while (running)
	{
		// check the size every frame, to make sure the window has not been resized
		int width, height;
		nv::GetWindowSize( width, height);

		// clear color, dpeth, and stencil, because the UI code relies on stencil
		glClearColor( 0.7, 0.7, 0.7, 1.0);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		//
		// Compute matrices without the legacy matrix stack support
		//
		nv::matrix4f projectionMatrix;
		nv::perspective( projectionMatrix, 45.0f * 2.0f*3.14159f / 360.0f, (float)width/(float)height, 0.1f, 1000.0f);

		nv::matrix4f translation, rotation;
		nv::translation( translation, 0.0f, 0.0f, -distance);
		nv::rotationY( rotation, angle*2.0f*3.14159f / 360.0f );
		nv::matrix4f modelviewMatrix = translation * rotation;
		
		//
		// update struct representing UBO
		//
		tessellationParameters.ModelView = modelviewMatrix;
		tessellationParameters.ModelViewProjection = projectionMatrix * modelviewMatrix;
		tessellationParameters.light = lit;

		angle += 0.5f;
		if (angle >= 360.0f)
			angle -= 360.0f;

		// toggle mode dependent on keyboard input (keys: 1,2,3)
		switch(mode){
			case 1:
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
				break;
			case 2:
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
				break;
			case 3:
				glPolygonMode( GL_FRONT_AND_BACK, GL_POINT);
				break;
		};
		
		glUseProgram(sProgram);
		glEnable(GL_PROGRAM_POINT_SIZE_EXT);
		glPointSize(4);

		//tranfer updated uniforms to gpu
		glUniformMatrix4fv(MVPlocation, 1, GL_FALSE, tessellationParameters.ModelViewProjection.get_value());
		glUniformMatrix4fv(MVlocation, 1, GL_FALSE, tessellationParameters.ModelView.get_value());
		glUniform1f(inTessLocation, tessellationParameters.innerTessFactor);
		glUniform1f(outTessLocation, tessellationParameters.outerTessFactor);
		glUniform1f(distanceLocation, tessellationParameters.distance);
		glUniform3fv(lightLocation, 1, tessellationParameters.lightPosition.get_value());
		
		if(tessellationParameters.light) 
			glUniform1f(litLocation, 1.0);
		else
			glUniform1f(litLocation, 0.0);

		if(tessellationParameters.lod)
			glUniform1f(lodLocation, 1.0f);
		else
			glUniform1f(lodLocation, 0.0f);		

		//draw icosahedron
		//glPatchParameteri(GL_PATCH_VERTICES, 3);
		glBindVertexArray(genCyl.getVAO());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texBufferIds[0]);
		glUniform1i(texLocation, 0);
		
		glDrawElements(GL_TRIANGLES, genCyl.getIndexCount(), GL_UNSIGNED_INT, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
		
		glBindVertexArray(0);
		glUseProgram(0);

		//
		// update the UI on mouse location, and render a simple immediate-mode style UI
		//
		int x,y;
		nv::GetMousePosition( x, y);
		ui.mouseMotion( x, y);

		ui.reshape( width, height);

		ui.begin();
		ui.beginGroup();

		// Show the app title
		ui.doLabel( nv::Rect(), "Simple Tessellation Shader", 1);
		
		// expose the variables
		ui.doCVar( lit);
		ui.doCVar( lod);
		ui.doCVar( innerTess);
		ui.doCVar( outerTess);
		ui.doCVar( distance);
		
		ui.endGroup();
		
		ui.end();

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << err << std::endl;
			exit(-2);
		}

		nv::SwapBuffers();
	}

	return 0; 
}
