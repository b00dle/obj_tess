
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
#include "Object3D.h"
#include "Scene3D.h"

#include <list>
#include "BarkModule.h"
#include "Bark.h"
#include "Crust.h"
#include "Fracture.h"
#include "BarkStrip.h"
#include "treeloader.h"

//shaders
unsigned sProgram;
unsigned shaderIds[5] = { 0u };
unsigned texBufferIds[12] = { 0u };

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
unsigned texDiffuseLocationY= 0; //young bark
unsigned texHeightLocationY	= 0;
unsigned texNormalLocationY	= 0;
unsigned texDiffuseLocationM= 0; //mid old bark
unsigned texHeightLocationM	= 0;
unsigned texNormalLocationM	= 0;
unsigned texDiffuseLocationO= 0; //old bark
unsigned texHeightLocationO	= 0;
unsigned texNormalLocationO	= 0;
unsigned texArrayLocation	= 0;

// Flag to identify when the app needs to close
bool running = true;
bool tree = false;

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
	case nv::KeyCode::Key_0:
		tree = !tree;			
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

void loadTexArray(int tID, std::vector<const char*> filepaths) {
	if(filepaths.size() < 2){
		std::cout << "Error!!! A texture array needs to contain at least 2 textures.\n";
		return;
	}

	FreeImage_Initialise();
	FIBITMAP* _bitmap;

	_bitmap = FreeImage_Load(FreeImage_GetFileType(filepaths[0], 0),
		filepaths[0]);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texBufferIds[tID]);

	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_2D_ARRAY,
		0, GL_RGB,
		FreeImage_GetWidth(_bitmap),
		FreeImage_GetHeight(_bitmap),
		filepaths.size(), //depth
		0, GL_RGB,
		GL_UNSIGNED_BYTE,
		NULL
		);

	for(int i = 0; i < filepaths.size(); ++i) {
		_bitmap = FreeImage_Load(FreeImage_GetFileType(filepaths[i], 0), filepaths[i]);
		unsigned char* pixelData = FreeImage_GetBits(_bitmap);

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0, 0,
			0, i,
			FreeImage_GetWidth(_bitmap),
			FreeImage_GetHeight(_bitmap),
			1, GL_RGB, GL_UNSIGNED_BYTE,
			pixelData
			);
	}
}

void generateTextures() {
	glGenTextures(9, texBufferIds);

	//young bark
	glEnable(GL_TEXTURE_2D);
	loadTexture(0,"../data/textures/bark_young/bark_young_COLOR.jpg");

	glEnable(GL_TEXTURE_2D);
	loadTexture(1,"../data/textures/bark_young/bark_young_DISP.jpg");

	glEnable(GL_TEXTURE_2D);
	loadTexture(2,"../data/textures/bark_young/bark_young_NRM.jpg");

	//middle old bark
	glEnable(GL_TEXTURE_2D);
	loadTexture(3,"../data/textures/bark_mid/bark_mid_COLOR.jpg");

	glEnable(GL_TEXTURE_2D);
	loadTexture(4,"../data/textures/bark_mid/bark_mid_DISP.jpg");

	glEnable(GL_TEXTURE_2D);
	loadTexture(5,"../data/textures/bark_mid/bark_mid_NRM.jpg");

	//old bark
	glEnable(GL_TEXTURE_2D);
	loadTexture(6,"../data/textures/bark_old/bark_old_COLOR.jpg");

	glEnable(GL_TEXTURE_2D);
	loadTexture(7,"../data/textures/bark_old/bark_old_DISP.jpg");

	glEnable(GL_TEXTURE_2D);
	loadTexture(8,"../data/textures/bark_old/bark_old_NRM.jpg");

	std::vector<const char*> filepaths;
	filepaths.push_back("../data/textures/bark_old/bark_old_COLOR.jpg");
	filepaths.push_back("../data/textures/bark_old/bark_old_DISP.jpg");
	filepaths.push_back("../data/textures/bark_old/bark_old_NRM.jpg");
	loadTexArray(9, filepaths);

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
	texDiffuseLocationY = glGetUniformLocation(sProgram, "texColorYoung");
	texHeightLocationY	= glGetUniformLocation(sProgram, "texHeightYoung");
	texNormalLocationY	= glGetUniformLocation(sProgram, "texNormalYoung");
	texDiffuseLocationM = glGetUniformLocation(sProgram, "texColorMid");
	texHeightLocationM	= glGetUniformLocation(sProgram, "texHeightMid");
	texNormalLocationM	= glGetUniformLocation(sProgram, "texNormalMid");
	texDiffuseLocationO = glGetUniformLocation(sProgram, "texColorOld");
	texHeightLocationO	= glGetUniformLocation(sProgram, "texHeightOld");
	texNormalLocationO	= glGetUniformLocation(sProgram, "texNormalOld");
	texArrayLocation	= glGetUniformLocation(sProgram, "texArray");
}

void setUniformValues(int width, int height, float angle) {
	//compute projection matrix
	nv::matrix4f projectionMatrix;
	nv::perspective( projectionMatrix, 45.0f * 2.0f*3.14159f / 360.0f, (float)width/(float)height, 0.1f, 1000.0f);

	//compute modelview matrix
	nv::matrix4f translation, rotation;

	if(!tree)
		nv::translation( translation, 0.0f, 0.0f, -distance);
	else
		nv::translation( translation, 0.0f, -200.0f, -100*distance);

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

	//young bark
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[0]);
	glUniform1i(texDiffuseLocationY, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[1]);
	glUniform1i(texHeightLocationY, 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[2]);
	glUniform1i(texNormalLocationY, 4);

	//mid old bark
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[3]);
	glUniform1i(texDiffuseLocationM, 5);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[4]);
	glUniform1i(texHeightLocationM, 6);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[5]);
	glUniform1i(texNormalLocationM, 7);

	//old bark
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[6]);
	glUniform1i(texDiffuseLocationO, 8);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[7]);
	glUniform1i(texHeightLocationO, 9);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[8]);
	glUniform1i(texNormalLocationO, 10);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, texBufferIds[9]);
	glUniform1i(texArrayLocation, 11);
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

	//system("PAUSE");

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
	nv::matrix4f p0fat, p1fat;
	p0fat.make_identity();
	p1fat.make_identity();
	p0fat.set_translate(nv::vec3f(0.0f,-1.0f,0.0f));
	p1fat.set_translate(nv::vec3f(0.0f,1.0f,0.0f));

	Path pathfat(p0fat);
	pathfat.addSegment(p1fat);
	pathfat.calculate();

	//contour
	std::vector<nv::vec4f> tempfat;
	tempfat.push_back(nv::vec4f(0,		0,		1,		1));
	tempfat.push_back(nv::vec4f(-1,	0,		0.5,	1));
	tempfat.push_back(nv::vec4f(-1,	0,		-0.5,	1));
	tempfat.push_back(nv::vec4f(0,		0,		-1,		1));
	tempfat.push_back(nv::vec4f(1,		0,		-0.5,	1));
	tempfat.push_back(nv::vec4f(1,		0,		0.5,	1));

	//Contour contour(temp);
	Contour contourfat;

	//thickness
	std::vector<float> thicknessfat;
	thicknessfat.push_back(0.6f);
	thicknessfat.push_back(0.3f);

	//water
	std::vector<float> waterfat;
	waterfat.push_back(1.0f);
	waterfat.push_back(1.0f);

	//water
	std::vector<float> agesfat;
	agesfat.push_back(1.0f);
	agesfat.push_back(0.0f);

	///////////////////////////////

	Icosahedron ico;
	Cylinder cyl;
	GeneralizedCylinder genCylfat(pathfat, contourfat, thicknessfat, waterfat, agesfat);
	TreeLoader tree;
	tree.readTreeDescr();
	GeneralizedCylinder genCyl = tree.getTreeDescr();
	//Image img("../data/textures/bark_DISP.jpg");

	/*unsigned int width = img.getWidth();
	unsigned int height = img.getHeight();
	nv::vec3f add(0.8,0.2,0.1);
	for(unsigned int x = 0; x < 649; ++x){
	for(unsigned int y = 0; y < 649; ++y){
	img.setPixel(x, y, 0.5*img.getPixel(x,y) + 0.5*add);
	}
	}
	*/

	//img.setTexBuffer(0);
	//glBindTexture(GL_TEXTURE_2D, 0); //safety unbind

	//////GENERALIZED CYLINDERS//////
	//water
	std::vector<float> water;
	water.push_back(0.0f);
	water.push_back(0.0f);
	water.push_back(0.0f);
	water.push_back(0.0f);
	water.push_back(0.0f);
	water.push_back(0.0f);
	water.push_back(0.0f);
	water.push_back(0.0f);
	water.push_back(0.0f);
	water.push_back(0.0f);
	water.push_back(0.0f);
	water.push_back(0.0f);
	water.push_back(0.0f);

	std::vector<float> ages;
	ages.push_back(1.0f);
	ages.push_back(1.0f);
	ages.push_back(1.0f);
	ages.push_back(1.0f);
	ages.push_back(1.0f);
	ages.push_back(1.0f);
	ages.push_back(1.0f);
	ages.push_back(1.0f);
	ages.push_back(1.0f);
	ages.push_back(1.0f);
	ages.push_back(1.0f);
	ages.push_back(1.0f);

	//Contour
	Contour contour;

	////Cyl1////

	//path
	nv::matrix4f p0(1.0,0.0,0.0,0.0,
		0.0,1.0,0.0,0.0,
		0.0,0.0,1.0,0.0,
		0.0,0.0,0.0,1.0);

	nv::matrix4f p1(1.0,0.0,0.0,0.0,
		0.0,1.0,0.0,0.0,
		0.0,0.0,1.0,0.0,
		0.0,40.0,0.0,1.0);

	nv::matrix4f p2(0.994525, 0, 0.104497, 0.0,
		-0.000863566, 0.999966, 0.00822915, 0.0,
		-0.104493, -0.0082743, 0.994491, 0.0,
		-0.0345426, 79.9986, 0.329166, 1.0 );

	nv::matrix4f p3(0.906395, -0.00268308, 0.422423, 0.0,
		0.000382466, 0.999985, 0.00553374, 0.0,
		-0.422431, -0.00485406, 0.906382, 0.0,
		-0.019244, 119.998, 0.550515, 1.0);

	nv::matrix4f p4(0.848118, -0.00325082, 0.529797, 0.0,
		-0.00247136, 0.999946, 0.0100939, 0.0,
		-0.529801, -0.00986999, 0.848064, 0.0,
		-0.118099, 159.996, 0.954269, 1.0 );

	nv::matrix4f p5(0.940036, -0.00113117, 0.341073, 0.0,
		-0.00435189, 0.999873, 0.0153166, 0.0,
		-0.341047, -0.0158825, 0.939912, 0.0,
		-0.292174, 199.991, 1.56693 ,1.0 );

	nv::matrix4f p6(0.913964, -0.0022373, 0.40579, 0.0,
		-0.00479406, 0.999855, 0.0163154, 0.0,
		-0.405768, -0.0168571, 0.913821, 0.0,
		-0.483937, 239.985, 2.21955, 1.0);

	nv::matrix4f p7(0.545421, -0.0110606, 0.838089, 0.0,
		-0.00811369, 0.999796, 0.0184784, 0.0,
		-0.838123, -0.0168783, 0.545221, 0.0,
		-0.808484, 279.977, 2.95868, 1.0 );

	nv::matrix4f p8(0.829132, -0.00358517, 0.559042, 0.0,
		-0.00225985, 0.99995, 0.00976967, 0,
		-0.559049, -0.00936349, 0.829082, 0.0,
		-0.898878, 319.975, 3.34947, 1.0 );

	nv::matrix4f p9(0.848101, -0.00325601, 0.529825, 0.0,
		-0.0066194, 0.999838, 0.0167456, 0.0,
		-0.529794, -0.0177089, 0.847942, 0.0,
		-1.16365, 359.968, 4.0193 ,1.0 );

	nv::matrix4f p10(0.720049, -0.00687099, 0.693889, 0.0,
		-0.00256533, 0.999918, 0.012566, 0.0,
		-0.693918, -0.0108281, 0.719972, 0.0,
		-1.26627, 399.965, 4.52194, 1.0);

	nv::matrix4f p11(0.424112, -0.0102914, 0.905552, 0.0,
		-0.0107291, 0.999808, 0.0163875, 0.0,
		-0.905546, -0.0166661, 0.42392, 0.0,
		-1.69543, 439.957, 5.17744, 1.0 );

	Path path(p0);
	path.addSegment(p1);
	path.addSegment(p2);
	path.addSegment(p3);
	path.addSegment(p4);
	path.addSegment(p5);
	path.addSegment(p6);
	path.addSegment(p7);
	path.addSegment(p8);
	path.addSegment(p9);
	path.addSegment(p10);
	path.addSegment(p11);
	path.calculate();

	//thickness
	std::vector<float> thickness;
	thickness.push_back(10.817);
	thickness.push_back(7.657);
	thickness.push_back(7.27);
	thickness.push_back(6.941);
	thickness.push_back(6.515);
	thickness.push_back(6.063);
	thickness.push_back(5.577);
	thickness.push_back(5.03);
	thickness.push_back(4.38);
	thickness.push_back(3.673); 
	thickness.push_back(2.74);
	thickness.push_back(0);

	////Cyl2//////////////////////
	//PATH//
	nv::matrix4f p12(0.848101, -0.00325601, 0.529825, 0.0,
		-0.0066194, 0.999838, 0.0167456, 0.0,
		-0.529794, -0.0177089, 0.847942, 0.0,
		-1.16365, 359.968, 4.0193, 1.0 );

	nv::matrix4f p13(0.720049, -0.00687099, 0.693889, 0.0,
		-0.00256533, 0.999918, 0.012566, 0.0,
		-0.693918, -0.0108281, 0.719972, 0.0,
		-1.26627, 399.965, 4.52194, 1.0);

	nv::matrix4f p14(0.970044, 0.00554262, -0.242866, 0.0,
		0.184436, 0.633874, 0.751124, 0.0,
		0.15811, -0.773416, 0.613864, 0.0,
		6.11118, 425.32, 34.5669, 1.0 );

	Path path1(p12);
	path1.addSegment(p13);
	path1.addSegment(p14);
	path1.calculate();

	//thickness///
	std::vector<float> thickness1;
	thickness1.push_back(2.74);
	thickness1.push_back(2.74);
	thickness1.push_back(0);

	////Cyl3////
	//PATH///
	nv::matrix4f p15(0.970044, 0.00554262, -0.242866, 0.0,
		0.184436, 0.633874, 0.751124, 0.0,
		0.15811, -0.773416, 0.613864, 0.0,
		6.11118, 425.32, 34.5669, 1.0 );    
	nv::matrix4f p16(0.970044, 0.00554262, -0.242866, 0.0,
		0.184436, 0.633874, 0.751124, 0.0,
		0.15811, -0.773416, 0.613864, 0.0,
		6.11118, 425.32, 34.5669, 1.0 );    
	nv::matrix4f p17(0.515915, -0.00943932, 0.856588, 0.0,
		0.654573, 0.649384, -0.387085, 0.0,
		-0.552601, 0.760402, 0.341206, 0.0,
		24.9166, 425.94 ,-10.9615, 1.0 );

	Path path2(p15);
	path2.addSegment(p16);
	path2.addSegment(p17);
	path2.calculate();

	//thickness///
	std::vector<float> thickness2;
	thickness2.push_back(2.74);
	thickness2.push_back(2.74);
	thickness2.push_back(0);

	///////////////////////////
	GeneralizedCylinder genCyl(path, contour, thickness, water, ages);
	GeneralizedCylinder genCyl1(path1, contour, thickness1, water, ages);
	GeneralizedCylinder genCyl2(path2, contour, thickness2, water, ages);


	//Scene3D scene("../data/models/cylinder_round.obj");

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

		if(!tree) {
			glBindVertexArray(genCylfat.getVAO());
			glDrawElements(GL_PATCHES, genCylfat.getIndexCount(), GL_UNSIGNED_INT, 0);
		}
		else {
			glBindVertexArray(genCyl.getVAO());
			glDrawElements(GL_PATCHES, genCyl.getIndexCount(), GL_UNSIGNED_INT, 0);

			glBindVertexArray(genCyl1.getVAO());
			glDrawElements(GL_PATCHES, genCyl1.getIndexCount(), GL_UNSIGNED_INT, 0);

			glBindVertexArray(genCyl2.getVAO());
			glDrawElements(GL_PATCHES, genCyl2.getIndexCount(), GL_UNSIGNED_INT, 0);
		}
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
