
#pragma once
#ifndef _shader_
#define _shader_

#include<GL/glew.h>
#include<gl/freeglut.h>
#include<string>
#include<iostream>
#include<fstream>
#include<glm/mat4x4.hpp>  //glm::mat4
#include <glm/gtc/matrix_transform.hpp>


using namespace std;

class shader
{
private:
	string readshader(const char*shadername);	//load the shader
	void printShaderInfoLog(GLuint obj);   //check the shader error
	void printProgramInfoLog(GLuint obj);  //check the program error
	GLuint VShader;	  //vertex shader
	GLuint FShader;	  //fragment shader
	const GLchar* vertexsource, *fragmentsource;

public:
	GLuint shaderprogramID;

	/*constructor to create the shader and program*/
	shader() {};
	~shader() {};
	shader(const GLchar*vertexshader,const GLchar*fragmentshader);

	/*use or detach the shader*/
	void use() { glUseProgram(shaderprogramID); }
	void detach() 
	{     
		glUseProgram(0); 
	    glDetachShader(shaderprogramID, VShader); 
		glDetachShader(shaderprogramID,FShader); 
	}
	void delete_shader()
	{
		glDeleteProgram(shaderprogramID);
		glDeleteShader(VShader);
		glDeleteShader(FShader);
		//free(vertexsource);
	}

	/*return the location of uniform and in variable from shader*/
	GLuint get_unifrom_location(const GLchar* uniform_name);
	GLuint get_attrib_location(const GLchar* attrib_name);

	/* vector variables in vs and fs*/
	void setuniform1f(const GLchar* uniform_name,float value);
	void setuniform3f(const GLchar* uniform_name,float x,float y,float z);
	void setuniform3f(const GLchar* uniform_name, const glm::vec3 &value);
	
	/* matrix variable in vs and fs*/
	void setmat3(const GLchar* uniform_name, const glm::mat3 &value);
	void setmat4(const GLchar* uniform_name, const glm::mat4 &value);

	/* int variable in vs and fs*/
	void setInt(const GLchar* uniform_name, const int &value);

};

#endif // !_shader_


