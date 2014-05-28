#ifndef __SHADER_H
#define __SHADER_H
//#define GLEW_STATIC

class Shader {
	public:
		static unsigned loadShader(const char *fileName, unsigned shaderType);
	private:
		Shader();
};

#endif
