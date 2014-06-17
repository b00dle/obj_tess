
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

#include "BarkModule.h"
#include "Crust.h"
#include "Fracture.h"
#include "BarkStrip.h"


//shaders
unsigned sProgram;
unsigned shaderIds[5] = { 0u };
unsigned texBufferIds[3] = { 0u };

//uniform locations
unsigned MVPlocation		= 0;
unsigned MVlocation			= 0;
unsigned inTessLocation		= 0;
unsigned outTessLocation	= 0;
unsigned lodLocation		= 0;
unsigned distanceLocation	= 0;
unsigned lightLocation		= 0;
unsigned litLocation		= 0;
unsigned dMappingLocation	= 0;
unsigned nMappingLocation	= 0;
unsigned texturedLocation	= 0;
unsigned trianglesLocation	= 0;
unsigned dfLocation			= 0;
unsigned texDiffuseLocation	= 0;
unsigned texHeightLocation	= 0;
unsigned texNormalLocation	= 0;

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
nv::CVar<bool> lit( "lit", true, nv::KeyCode::Key_L, "Toggle lighting 'l'");
nv::CVar<bool> lod( "lod", true, nv::KeyCode::Key_K, "Toggle lod mode 'k'");
nv::CVar<bool> dMapping( "dMapping", true, nv::KeyCode::Key_D, "Toggle displacement mapping 'd'");
nv::CVar<bool> nMapping( "nMapping", true, nv::KeyCode::Key_N, "Toggle normal mapping 'n'");
nv::CVar<bool> textured( "textured", false, nv::KeyCode::Key_T, "Toggle texture mode 't'");
nv::CVar<bool> triangles( "triangles", false, nv::KeyCode::Key_P, "Toggle triangle visualization mode 'v'");
nv::CRefVar<float> innerTess( tessellationParameters.innerTessFactor, "InnerTessFactor", 1.0f, 1.0f, 64.0f, nv::KeyCode::Key_I, "Change tesselation factor");
nv::CRefVar<float> outerTess( tessellationParameters.outerTessFactor, "OuterTessFactor", 1.0f, 1.0f, 64.0f, nv::KeyCode::Key_O, "Change tesselation factor");
nv::CRefVar<float> distance( tessellationParameters.distance, "distance", 1.0f, 1.0f, 64.0f, nv::KeyCode::Key_P, "Change distance");
nv::CRefVar<float> df( tessellationParameters.df, "displacementFactor", 0.2f, 0.0f, 1.0f, nv::KeyCode::Key_P, "Change displacement Factor");

unsigned mode = 2;

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

void generateUI(int width, int height) {
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
	ui.doCVar( dMapping);
	ui.doCVar( nMapping);
	ui.doCVar( textured);
	ui.doCVar( triangles);
	ui.doCVar( innerTess);
	ui.doCVar( outerTess);
	ui.doCVar( distance);
	ui.doCVar( df);
		
	ui.endGroup();
		
	ui.end();
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

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void generateTextures() {
	glGenTextures(3, texBufferIds);

	glEnable(GL_TEXTURE_2D);
	loadTexture(0,"../data/textures/bark_COLOR.jpg");

	glEnable(GL_TEXTURE_2D);
	loadTexture(1,"../data/textures/bark_DISP.jpg");

	glEnable(GL_TEXTURE_2D);
	loadTexture(2,"../data/textures/bark_NRM.jpg");

	glBindTexture(GL_TEXTURE_2D, 0);
}

void setUniformLocations() {
	//uniform locations
	MVPlocation			= glGetUniformLocation(sProgram, "ModelViewProjection");
	MVlocation			= glGetUniformLocation(sProgram, "ModelView");
	inTessLocation		= glGetUniformLocation(sProgram, "innerTessLevel");
	outTessLocation		= glGetUniformLocation(sProgram, "outerTessLevel");
	lodLocation			= glGetUniformLocation(sProgram, "lod");
	distanceLocation	= glGetUniformLocation(sProgram, "distance");
	lightLocation		= glGetUniformLocation(sProgram, "light");
	litLocation			= glGetUniformLocation(sProgram, "lit");
	dMappingLocation	= glGetUniformLocation(sProgram, "dMapping");
	nMappingLocation	= glGetUniformLocation(sProgram, "nMapping");
	texturedLocation	= glGetUniformLocation(sProgram, "textured");
	trianglesLocation	= glGetUniformLocation(sProgram, "triangles");
	dfLocation			= glGetUniformLocation(sProgram, "df");
	texDiffuseLocation  = glGetUniformLocation(sProgram, "texColor");
	texHeightLocation	= glGetUniformLocation(sProgram, "texHeight");
	texNormalLocation	= glGetUniformLocation(sProgram, "texNormal");
}

void setUniformValues(int width, int height, float angle) {
	//compute projection matrix
	nv::matrix4f projectionMatrix;
	nv::perspective( projectionMatrix, 45.0f * 2.0f*3.14159f / 360.0f, (float)width/(float)height, 0.1f, 1000.0f);

	//compute modelview matrix
	nv::matrix4f translation, rotation;
	nv::translation( translation, 0.0f, 0.0f, -distance);
	nv::rotationY( rotation, angle*2.0f*3.14159f / 360.0f );
	nv::matrix4f modelviewMatrix = translation * rotation;
		
	// update struct representing UBO
	tessellationParameters.ModelView			= modelviewMatrix;
	tessellationParameters.ModelViewProjection	= projectionMatrix * modelviewMatrix;
	tessellationParameters.light				= lit;
	tessellationParameters.dMapping				= dMapping;
	tessellationParameters.nMapping				= nMapping;
	tessellationParameters.textured				= textured;
	tessellationParameters.triangles			= triangles;
	tessellationParameters.df					= df;

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
	
	//bind shader program
	glUseProgram(sProgram);
	
	//tranfer updated uniforms to gpu
	glUniformMatrix4fv(MVPlocation, 1, GL_FALSE, tessellationParameters.ModelViewProjection.get_value());
	glUniformMatrix4fv(MVlocation, 1, GL_FALSE, tessellationParameters.ModelView.get_value());
	glUniform1f(inTessLocation, tessellationParameters.innerTessFactor);
	glUniform1f(outTessLocation, tessellationParameters.outerTessFactor);
	glUniform1f(distanceLocation, tessellationParameters.distance);
	glUniform3fv(lightLocation, 1, tessellationParameters.lightPosition.get_value());
	glUniform1f(dfLocation, tessellationParameters.df);

	if(tessellationParameters.light) 
		glUniform1f(litLocation, 1.0f);
	else
		glUniform1f(litLocation, 0.0f);

	if(tessellationParameters.dMapping) 
		glUniform1f(dMappingLocation, 1.0f);
	else
		glUniform1f(dMappingLocation, 0.0f);

	if(tessellationParameters.nMapping) 
		glUniform1f(nMappingLocation, 1.0f);
	else
		glUniform1f(nMappingLocation, 0.0f);

	if(tessellationParameters.textured) 
		glUniform1f(texturedLocation, 1.0f);
	else
		glUniform1f(texturedLocation, 0.0f);

	if(tessellationParameters.triangles) 
		glUniform1f(trianglesLocation, 1.0f);
	else
		glUniform1f(trianglesLocation, 0.0f);

	if(tessellationParameters.lod)
		glUniform1f(lodLocation, 1.0f);
	else
		glUniform1f(lodLocation, 0.0f);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texBufferIds[0]);
	//glBindTexture(GL_TEXTURE_2D, img.getTexBufferID());
	//glUniform1i(texHeightLocation, 0);

	//nv framework binds at location 1, so textures are bound starting from location 2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[0]);
	glUniform1i(texDiffuseLocation, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[1]);
	glUniform1i(texHeightLocation, 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[2]);
	glUniform1i(texNormalLocation, 4);
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
		shaderIds[2] = Shader::loadShader("tess_control.glsl", GL_TESS_CONTROL_SHADER);
		shaderIds[3] = Shader::loadShader("tess_eval.glsl", GL_TESS_EVALUATION_SHADER);
		shaderIds[4] = Shader::loadShader("geometry.glsl", GL_GEOMETRY_SHADER);

		//attach stages to program
		glAttachShader(sProgram, shaderIds[0]);
		glAttachShader(sProgram, shaderIds[1]);
		glAttachShader(sProgram, shaderIds[2]);
		glAttachShader(sProgram, shaderIds[3]);
		glAttachShader(sProgram, shaderIds[4]);
	}
	glLinkProgram(sProgram);

	//loads all textures from files (as specified in generateTextures() function)
	generateTextures();

	//sets uniform locations for shader program (as specified in setUniformLocations() function)
	setUniformLocations();
	
	GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
		exit(-2);
    }
}

int main( int argc, char **argv)
{
	BarkModule* crust = new Crust(0.0f,0.0f,0.0f,12.0f);
	BarkModule* fracture = new Fracture(0.0f,0.0f,0.0f);

	Crust* castCrust = (Crust*) crust;

	std::cout << "crust type: " << crust->type() << std::endl;
	std::cout << "crust threshold: " << castCrust->getThreshold() << std::endl;
	std::cout << "fracture type: " << fracture->type() << std::endl;
	
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
	start.set_translate(nv::vec3f(0.0f,-1.0f,0.0f));
	end.set_translate(nv::vec3f(0.0f,1.0f,0.0f));
	
	Path path(start);
	path.addSegment(end);
	path.calculate();
	
	//contour
	std::vector<nv::vec4f> temp;
	temp.push_back(nv::vec4f(0,		0,		1,		1));
	temp.push_back(nv::vec4f(-1,	0,		0.5,	1));
	temp.push_back(nv::vec4f(-1,	0,		-0.5,	1));
	temp.push_back(nv::vec4f(0,		0,		-1,		1));
	temp.push_back(nv::vec4f(1,		0,		-0.5,	1));
	temp.push_back(nv::vec4f(1,		0,		0.5,	1));

	//Contour contour(temp);
	Contour contour;

	//thickness
	std::vector<float> thickness;
	thickness.push_back(1.0f);
	thickness.push_back(1.0f);
	
	//water
	std::vector<float> water;
	water.push_back(1.0f);
	water.push_back(1.0f);

	///////////////////////////////

	Icosahedron ico;
	Cylinder cyl;
	GeneralizedCylinder genCyl(path, contour, thickness, water);
	
	//Image img("../data/textures/bark_DISP.jpg");
	
	/*unsigned int width = img.getWidth();
	unsigned int height = img.getHeight();
	nv::vec3f add(0.8,0.2,0.1);
	for(unsigned int x = 0; x < 649; ++x){
		for(unsigned int y = 0; y < 649; ++y){
			img.setPixel(x, y, 0.5*img.getPixel(x,y) + 0.5*add);
		}
	}*/
	
	//img.setTexBuffer(0);
	//glBindTexture(GL_TEXTURE_2D, 0); //safety unbind
	
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

		setUniformValues(width, height, angle);

		angle += 0.5f;
		if (angle >= 360.0f)
			angle -= 360.0f;

		glEnable(GL_PROGRAM_POINT_SIZE_EXT);
		glPointSize(4);

		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glBindVertexArray(genCyl.getVAO());
		
		glDrawElements(GL_PATCHES, genCyl.getIndexCount(), GL_UNSIGNED_INT, 0);

		//unbind everything
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		generateUI(width, height);

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << err << std::endl;
			exit(-2);
		}

		nv::SwapBuffers();
	}

	return 0; 
}
