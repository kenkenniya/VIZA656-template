/**********************************************************************************************/
/**********************************************************************************************/
/* VIZA656/CSE647 at Texas A&M University
   Real-time GPU rendering template 
   created by Logan Zhou, 02/2018 
   Copyright (c) 2018 Logan Zhou. All rights reserved. 
  
   Instruction: 
   this program can realize diffuse and specular reflection, shadow casting, load obj file 
   and load environment mapping. A standford bunny is loaded for testing 
  
   Operation: 
   to move the camera, use your left mouse and Alt button at the same time
   use mouse scroll wheel to zoom in and out */
/**********************************************************************************************/
/**********************************************************************************************/

#define _CRT_SECURE_NO_DEPRECATE

/* The libraries used in this program*/
#include<GL/glew.h>
#include<gl/freeglut.h>
#include<iostream>
#include<string>
#include<cstring>
#include<fstream>
#include<vector>
#include<glm/glm.hpp>
#include<glm/mat4x4.hpp>  //glm::mat4
#include <glm/gtc/matrix_transform.hpp>

#include"Object.h"
#include"shader.h"
#include"camera.h"

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 900
#define NONE -1;

using namespace std;

int Main_Window; //glut window

/*These variables will store the UV, vertex and normal datas from obj file*/
vector<glm::vec3> obj_vertices;
vector<glm::vec2> obj_UVs;
vector<glm::vec3> obj_normals;

/*global variables to track mouse and key*/
static int MouseX;
static int MouseY;
static int Button = NONE;

/*camera object*/
camera camera1;

/*variables to store the framebuffers, depth texture and environmental mapping texture*/
GLuint FBO; //framebuffer object
GLuint depthTexture;//depth texture for shadow mapping
GLuint cubemaptexture0;	 //environment mapping texture

/*VAO and VBO to store data of plane and environment*/
GLuint PlaneVAO;
GLuint skyboxVAO, skyboxVBO;

/*the position of light*/
glm::vec3 light_position;

/*declare matrices*/
glm::mat4 View_matrix;	  //view matrix
glm::mat4 Project_matrix;	//project matrix
glm::mat4 light_Projection_Matrix; //for shadowmapping
glm::mat4 light_View_Matrix;  //for shadowmapping

/*variables to store the shader*/
shader  plane_shader;
shader shadow_mapping_shader;
shader obj_shader;
shader skybox_shader;

/*load the object*/
Object Bunny("bunny.obj");

/*the size of plane*/
const float BackLength = 20;
const float BackWidth = 10;

/*variables to store the six faces of a cube map (environment mapping)*/
vector< const char* > Cubemapfaces =
{
	"texture/nice/posx.jpg",
	"texture/nice/negx.jpg",
	"texture/nice/posy.jpg",
	"texture/nice/negy.jpg",
	"texture/nice/posz.jpg",
	"texture/nice/negz.jpg",
};


/*function to set up the shadow mapping*/
void setupshadow()
{
	glGenFramebuffers(1, &FBO);

	/*create depth texture buffer*/
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	/*attach the depth buffer to Framebuffer(FBO)*/
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	/*no color is drawn to*/
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

/*function to render plane*/
void renderplane()
{
	GLuint PlaneVBO;

	float planeVertices[] = {
		// positions                    //normals		             //texcoord
		BackLength, -0.4f,  BackWidth,   0.0f,1.0f,0.0f,	 1.0f, 1.0f,
		-BackLength, -0.4f,  BackWidth,  0.0f,1.0f,0.0f,	 0.0f, 1.0f,
		-BackLength, -0.4f, -BackWidth,  0.0f,1.0f,0.0f,	 0.0f, 0.0f,

		BackLength, -0.4f,  BackWidth,   0.0f,1.0f,0.0f,	 1.0f, 1.0f,
		-BackLength, -0.4f, -BackWidth,  0.0f,1.0f,0.0f,	 0.0f, 0.0f,
		BackLength, -0.4f, -BackWidth,   0.0f,1.0f,0.0f,	 1.0f, 0.0f
	};

	glGenVertexArrays(1, &PlaneVAO);
	glGenBuffers(1, &PlaneVBO);
	glBindVertexArray(PlaneVAO);
	glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	glDeleteBuffers(1, &PlaneVBO);

}

/*function to render cubemap (environment mapping)*/
void rendercubemap()
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


}

/*function to initialize shaders*/
void InitiShader()
{
	/*build up shader program*/
	plane_shader = shader("vs_plane.glsl", "fs_plane.glsl");
	shadow_mapping_shader = shader("vs_shadow_mapping.glsl", "fs_shadow_mapping.glsl");
	obj_shader = shader("vs_obj.glsl", "fs_obj.glsl");
	skybox_shader = shader("vs_skybox.glsl", "fs_skybox.glsl");
}

void Initialization() 
{
	Bunny.RenderObject();

	rendercubemap();
	renderplane();

	/*initialize cubemap for texture mapping*/
	cubemaptexture0 = Bunny.LoadCubeMapTexture(Cubemapfaces);

	light_position = glm::vec3(6.5, 10.5, 3.5);

	light_View_Matrix = glm::lookAt(light_position,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	light_Projection_Matrix = glm::ortho<float>(-5, 5, -5, 5, 1, 90);

	InitiShader();

}

/*draw function for display function*/
void draw()
{

	/*enable depth test to store the depth component of each fragments*/
	glEnable(GL_DEPTH_TEST);

	/************************************* render the shadow map ***********************************/
	setupshadow();
	shadow_mapping_shader.use();
	shadow_mapping_shader.setmat4("depthModelMatrix", glm::mat4(1.0));
	shadow_mapping_shader.setmat4("depthViewMatrix", light_View_Matrix);
	shadow_mapping_shader.setmat4("depthProjectMatrix", light_Projection_Matrix);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0.9, 0.9, 0.9, 1);

	glBindVertexArray(Bunny.Object_VAO);
	glDrawArrays(GL_TRIANGLES, 0, Bunny.VerticesNumber);
	glBindVertexArray(PlaneVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/**************************** render the plane ****************************************************/
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.9, 0.9, 0.9, 1);

	plane_shader.use();
	plane_shader.setmat4("ModelMatrix", glm::mat4(1.0));
	plane_shader.setmat4("ViewMatrix", camera1.Return_Camera_View_Matrix());
	plane_shader.setmat4("ProjectMatrix", camera1.Return_Camera_Project_Matrix());
	plane_shader.setmat4("depthModelMatrix", glm::mat4(1.0));
	plane_shader.setmat4("depthViewMatrix", light_View_Matrix);
	plane_shader.setmat4("depthProjectMatrix", light_Projection_Matrix);
	plane_shader.setuniform3f("light_position", light_position);
	plane_shader.setuniform3f("camera_position", camera1.position);
	plane_shader.setInt("ShadowMap", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glBindVertexArray(PlaneVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/*************************** render the object ****************************************/
	obj_shader.use();
	obj_shader.setmat4("ModelMatrix", glm::mat4(1.0));
	obj_shader.setmat4("ViewMatrix", camera1.Return_Camera_View_Matrix());
	obj_shader.setmat4("ProjectMatrix", camera1.Return_Camera_Project_Matrix());
	obj_shader.setmat4("depthModelMatrix", glm::mat4(1.0));
	obj_shader.setmat4("depthViewMatrix", light_View_Matrix);
	obj_shader.setmat4("depthProjectMatrix", light_Projection_Matrix);
	obj_shader.setuniform3f("light_position", light_position);
	obj_shader.setuniform3f("Center", Bunny.CenterVertices);
	obj_shader.setuniform3f("camera_position", camera1.position);

	obj_shader.setInt("ShadowMap", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	glBindVertexArray(Bunny.Object_VAO);
	glDrawArrays(GL_TRIANGLES, 0, Bunny.VerticesNumber);

	/******************************** render the cubemap **********************************/
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    skybox_shader.use();
	skybox_shader.setInt("skybox", 0);
	View_matrix = glm::mat4(glm::mat3(camera1.Return_Camera_View_Matrix())); // remove translation from the view matrix
	skybox_shader.setmat4("view", View_matrix);
	skybox_shader.setmat4("projection", camera1.Return_Camera_Project_Matrix());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaptexture0);
	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);	
	glDepthFunc(GL_LESS); // set depth function back to default

	glutSwapBuffers();

	/*******************************clean up everything*************************************/
	///*free the shader*/
	/*shadow_mapping_shader.detach();
	plane_shader.detach();
	obj_shader.detach();

	shadow_mapping_shader.delete_shader();
	plane_shader.delete_shader();
	obj_shader.delete_shader();*/

	///*free the framebuffer and texture*/
	//glDeleteVertexArrays(1, &Bunny.Object_VAO);
	//glDeleteVertexArrays(1, &PlaneVAO);
	//glDeleteVertexArrays(1, &skyboxVAO);

	//glDeleteBuffers(1, &cubeVBO);
	//glDeleteBuffers(1, &skyboxVAO);
	/***************************************************************************************/

}

/*(mouse button call back) mousebutton function for mouse */
void handleButtons(int button, int state, int x, int y)
{
	if (state == GLUT_UP)
	{
		button = NONE;
	}
	else
	{
		MouseX = x;
		MouseY = -y;

		Button = button;
	}

}

/*control mouse scroll wheel*/
void handlewheel(int button, int direction, int x, int y)
{
	camera1.Mouse_Sroll(direction);
	glutPostRedisplay();
}

/*(mouse motion call back) when mouse moves with a button down, update approriate camera parameters*/
void handleMotion(int x, int y)
{
	y = -y;

	int dy = y - MouseY;
	int dx = x - MouseX;

	switch (Button) {
	case GLUT_LEFT_BUTTON:
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
		{
			camera1.Mouse_Motion(dx, dy);
			glutPostRedisplay();
			break;
		}
	case GLUT_MIDDLE_BUTTON:
		break;
	}

	MouseX = x;
	MouseY = y;
}

int main(int argc, char* argv[])
{
	camera1 = camera(glm::vec3(0, 0, 10), glm::vec3(0, 0.4, 0), glm::vec3(0, 1, 0), 45);

	glutInit(&argc, argv);      // intialize glut package
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); // create single buffer RGB window
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	Main_Window = glutCreateWindow("glsl");

	glutDisplayFunc(draw);
	glutMouseFunc(handleButtons);
	glutMotionFunc(handleMotion);
	glutMouseWheelFunc(handlewheel);

	glewInit();
	if (glewIsSupported("GL_VERSION_4_3"))
	{
		printf("Ready for OpenGL 4.3\n");
	}
	else {
		printf("OpenGL 4.3 not supported\n");
		exit(1);
	}

	Initialization();
	glutMainLoop();  // enter main loop waiting for events

	return 0;
}

