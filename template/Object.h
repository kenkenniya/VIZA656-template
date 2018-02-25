#pragma once
#ifndef _OBJECT_
#define _OBJECT_

#include<GL/glew.h>
#include<GL/freeglut.h>
#include<glm\glm.hpp>
#include<vector>

#include"shader.h"


using namespace std;

class Object
{
private:
	
	/*store the data from obj file*/
	vector<glm::vec3> obj_vertices;
	vector<glm::vec2> obj_UVs;
	vector<glm::vec3> obj_normals;

	/*load obj file*/
	void loadobj(const char* filename);

public:

	/*object VAO*/
	GLuint Object_VAO;

	glm::vec3 CenterVertices;

	glm::mat4 ModelMatrix;
	unsigned int VerticesNumber;

	/*set up VBO and VAO of the object*/
	void RenderObject();

	Object() {};
	Object(const char*filename) { loadobj(filename); };
	~Object() {};

	/*add texture and environment mapping to the object*/
	GLuint AddTexture(const char* filename);
	GLuint LoadCubeMapTexture(vector<const char*> faces);

};

#endif // !_OBJECT_
