/**********************************************************************/
/* build up camera object to store the position, euler angle of camera*/
/**********************************************************************/

#pragma once
#ifndef _camera_
#define _camera_

#include<iostream>
#include<glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define MOUSE_SENSITIVITY 0.2
#define SCROLL_SENSITIVITY 0.05
#define PI 3.14159265

using namespace std;

class camera
{
private:
	void Update_Camera();
	float length();
public:
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 worldup;
	glm::vec3 target;

	//eular angles
	float yaw;
	float pitch;
	float zoom;

	camera() {};
	camera(glm::vec3 po, glm::vec3 tar ,glm::vec3 up, float zoo);
	~camera() {};

	glm::mat4 Return_Camera_View_Matrix();
	glm::mat4 Return_Camera_Project_Matrix();

	void Mouse_Motion(float xoffset, float yoffset);
	void Mouse_Sroll(int direction);
};




#endif // !_camera_
