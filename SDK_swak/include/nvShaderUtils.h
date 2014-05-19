// Copyright (c) 2012 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, NONINFRINGEMENT,IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA 
// OR ITS SUPPLIERS BE  LIABLE  FOR  ANY  DIRECT, SPECIAL,  INCIDENTAL,  INDIRECT,  OR  
// CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS 
// OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY 
// OTHER PECUNIARY LOSS) ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, 
// EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// Please direct any bugs or questions to SDKFeedback@nvidia.com

#ifndef NV_SHADER_UTILS_H
#define NV_SHADER_UTILS_H


#define NV_REPORT_COMPILE_ERRORS
namespace nv {

// Matrices, must align to 4 vector (16 bytes)
#define SDK_MAT4 __declspec(align(16)) nv::matrix4f

// vectors, 4-tuples and 3-tuples must align to 16 bytes
//  2-vectors must align to 8 bytes
#define SDK_VEC4 __declspec(align(16)) nv::vec4f
#define SDK_VEC3 __declspec(align(16)) nv::vec3f
#define SDK_VEC2 __declspec(align(8)) nv::vec2f

#define ivec4 __declspec(align(16)) nv::vec4i
#define ivec3 __declspec(align(16)) nv::vec3i
#define ivec2 __declspec(align(8)) nv::vec2i

#define uivec4 __declspec(align(16)) nv::vec4ui
#define uivec3 __declspec(align(16)) nv::vec3ui
#define uivec2 __declspec(align(8)) nv::vec2ui

// scalars
//  uint can be a typedef
//  bool needs special padding / alignment
#define SDK_BOOL __declspec(align(4)) nv::boolClass
#define uint unsigned int

//class to make uint look like bool to make GLSL packing rules happy
struct boolClass
{
	unsigned int _rep;

	boolClass() : _rep(false) {}
	boolClass( bool b) : _rep(b) {}
	operator bool() { return _rep == 0 ? false : true; }
	boolClass& operator=( bool b) { _rep = b; return *this; }
};

//
//
////////////////////////////////////////////////////////////
inline GLuint CompileGLSLShader( GLenum target, const char* shader)
{
    GLuint object = glCreateShaderProgramv( target, 1, &shader );

	GLint status;
	glGetProgramiv(object, GL_LINK_STATUS, &status);

	if (!status)
	{
		GLint charsWritten, infoLogLength;
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &infoLogLength);
		char * infoLog = new char[infoLogLength];
		glGetProgramInfoLog(object, infoLogLength, &charsWritten, infoLog);
		printf(infoLog);
		delete [] infoLog;

		glDeleteProgram( object);
		object = 0;
	}

	return object;
}

//
//
////////////////////////////////////////////////////////////
inline GLuint CompileGLSLShaderFromFile( GLenum target, const char* filename)
{
    nv::FilePtr* shaderFile;
    char *text;
    long size;

    //must read files as binary to prevent problems from newline translation
    shaderFile = nv::FilePtr::Create( filename, nv::FileMode::Read);

    if ( shaderFile == 0)
        return 0;

    size = shaderFile->GetSize();


    text = new char[size+1];

    shaderFile->Read( size, text);

    delete shaderFile;

    text[size] = '\0';

    GLuint object = CompileGLSLShader( target, text);

    delete []text;

    return object;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
inline bool RegisterShaderInclude( const char* filename)
{
	nv::FilePtr* includeFile;
    char *text;
    long size;
	std::string str= "/";
	str += filename;

    //must read files as binary to prevent problems from newline translation
    includeFile = nv::FilePtr::Create( filename, nv::FileMode::Read);

    if ( includeFile == 0)
        return false;

    size = includeFile->GetSize();


    text = new char[size+1];

    includeFile->Read( size, text);

    delete includeFile;

    text[size] = '\0';

    glNamedStringARB( GL_SHADER_INCLUDE_ARB, -1, str.c_str(), -1, text);

    delete []text;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
inline void ProgramUniform( GLuint prog, const char* var, bool val)
{
	int loc = glGetUniformLocation( prog, var);
	if (loc >= 0)
		glProgramUniform1i( prog, loc, int(val));
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////

static const char sdk_defs[] =
	"// Standard SDK defines\n"
	"#define SDK_BOOL  bool\n"
	"#define SDK_VEC2  vec2\n"
	"#define SDK_VEC3  vec3\n"
	"#define SDK_VEC4  vec4\n"
	"#define SDK_MAT4  mat4\n";

inline GLuint CompileGLSLShaderFromFile( GLenum target, const char* version, const char** defines, const char* filename)
{
    nv::FilePtr* shaderFile;
    std::string text;
    long size;

    //must read files as binary to prevent problems from newline translation
    shaderFile = nv::FilePtr::Create( filename, nv::FileMode::Read);

    if ( shaderFile == 0)
        return 0;

	//add version and defines to the shader
	text = "#version ";
	text += version;
	text += "\n\n";

	text += sdk_defs;

	text += "\n";

	// walk the NULL-terminated list of defines
	if (defines)
	{
		text += "// User-injected defines\n";
		while (*defines && **defines)
		{
			text += "#define ";
			text += *defines;
			text += " ";
			defines++;
			if (*defines)
			{
				text += *defines;
			}
			defines++;
			text += "\n";
		}
		text += "\n";
	}

	//reset the line number, so error messages make sense
	text += "#line 1\n";

	//grab and append the shader text
    size = shaderFile->GetSize();
	char *shaderText = new char[size+1];

    shaderFile->Read( size, shaderText);

	shaderText[size] = '\0';

	text += shaderText;

	delete []shaderText;

    delete shaderFile;

	//compile the shader
    GLuint object = CompileGLSLShader( target, text.c_str());

    return object;
}

}

#endif