#include "camera.h"

void camera::Update_Camera()
{
	// Calculate the new Front vector
	glm::vec3 po;
	po.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	po.y = sin(glm::radians(pitch));
	po.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	position = glm::normalize(po)*length();

	//cout << position.x << " " << position.y << " " << position.z << endl;
}

float camera::length()
{
	return sqrt(position.x*position.x + position.y*position.y + position.z*position.z);
}

camera::camera(glm::vec3 po, glm::vec3 tar,glm::vec3 upp, float zoo)
{
	position = po;
	target = tar;
	up = upp;
	zoom = zoo;

	glm::vec3 NormalizedPos = glm::normalize(position);
	pitch = asin(NormalizedPos.y)* 180 / PI; // degree
	yaw = acos(NormalizedPos.x / cos(glm::radians(pitch))) * 180 / PI; // degree

}

glm::mat4 camera::Return_Camera_View_Matrix()
{
	return glm::lookAt(position,target,up);
}

glm::mat4 camera::Return_Camera_Project_Matrix()
{
	//return glm::ortho(-30.0f,30.0f,-30.0f,30.0f,0.1f,20.0f);
	return glm::perspective(zoom, 3.0f / 3.0f, 0.1f, 30.0f);
}

void camera::Mouse_Motion(float xoffset, float yoffset)
{
	xoffset *= MOUSE_SENSITIVITY;
	yoffset *= MOUSE_SENSITIVITY;

	yaw += xoffset;
	pitch -= yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Update Front, Right and Up Vectors using the updated Eular angles
	Update_Camera();
}

void camera::Mouse_Sroll(int direction)
{
	if (direction > 0)
		zoom -= SCROLL_SENSITIVITY;
	else if (direction < 0)
		zoom += SCROLL_SENSITIVITY;
}


