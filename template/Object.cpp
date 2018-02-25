#define _CRT_SECURE_NO_DEPRECATE
#define STB_IMAGE_IMPLEMENTATION

#include"stb_image.h"
#include "Object.h"

void Object::loadobj(const char * filename)
{
	bool hasNormals = false;
	bool hasTexture = false;

	/*create temporary data*/
	vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	vector< glm::vec3 > temp_vertices;
	vector< glm::vec2 > temp_uvs;
	vector< glm::vec3 > temp_normals;

	glm::vec3 SumVertices = glm::vec3(0.0f);
	int Num=0;

	/*open a file*/
	FILE * file = fopen(filename, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return;
	}

	while (1) {
		char lineHeader[128];

		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		//Let's deal with the vertices first :
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			SumVertices += vertex;
			Num++;
			temp_vertices.push_back(vertex * 1.0f);
		}

		//If the first word of the line is vt then store the UV coordinate
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 UV;
			fscanf(file, "%f %f\n", &UV.x, &UV.y);
			temp_uvs.push_back(UV);
			hasTexture = true;
		}

		//if the first word is vn, then store the normals coordinate
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3  normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
			hasNormals = true;
		}

		//And now the f which is more difficult
		else if (strcmp(lineHeader, "f") == 0) {
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches;

			if (hasTexture && hasNormals) {
				matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("9File can't be read by our simple parser : ( Try exporting with other options\n");
					return;
				}
			}
			else if (hasTexture && !hasNormals) {
				matches = fscanf(file, "%d/%d/ %d/%d/ %d/%d/\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
				if (matches != 6) {
					printf("61File can't be read by our simple parser : ( Try exporting with other options\n");
					return;
				}
			}
			else if (hasNormals && !hasTexture) {
				matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				if (matches != 6) {
					printf("62File can't be read by our simple parser : ( Try exporting with other options\n");
					return;
				}
			}
			else
			{
				matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
				if (matches != 3) {
					printf("3File can't be read by our simple parser : ( Try exporting with other options\n");
					return;
				}
			}

			if (hasTexture  && hasNormals)
			{
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
			else if (hasTexture && !hasNormals)
			{
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
			}
			else if (hasNormals && !hasTexture)
			{
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);

				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);

				//	cout << "has normals, no texture" << endl;

			}
			else
			{
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
			}
		}
	}

	//cout << " normals" << hasNormals << '\n ' << "texture" << hasTexture << '\n ';
	fclose(file);

	/*indexing data (saving the info of each vertex to the output vec3)*/
	for (int i = 0; i < vertexIndices.size(); i++)
	{
		if (hasTexture  && hasNormals)
		{
			obj_UVs.push_back(temp_uvs[uvIndices[i] - 1]);
			obj_normals.push_back(temp_normals[normalIndices[i] - 1]);
			obj_vertices.push_back(temp_vertices[vertexIndices[i] - 1]);

			//cout << out_UVs[i].x << ' ' << out_UVs[i].y <<  endl;
			//cout << out_UVs.size();
		}
		else if (hasTexture && !hasNormals)
		{
			obj_UVs.push_back(temp_uvs[uvIndices[i] - 1]);
			obj_vertices.push_back(temp_vertices[vertexIndices[i] - 1]);
		}
		else if (!hasTexture && hasNormals)
		{
			obj_normals.push_back(temp_normals[normalIndices[i] - 1]);
			obj_vertices.push_back(temp_vertices[vertexIndices[i] - 1]);

			//cout << out_normals[i].x << ' ' << out_normals[i].y << ' ' << out_normals[i].z << endl;

		}
		else
		{
			obj_vertices.push_back(temp_vertices[vertexIndices[i] - 1]);
		}
	}

	VerticesNumber = obj_normals.size();

	ModelMatrix = glm::mat4(1.0f);

	CenterVertices = SumVertices / float(Num);
	//cout << CenterVertices.x << " " << CenterVertices.y << " " << CenterVertices.z << endl;

	cout << "Finished loading " << filename << "\n";
}

void Object::RenderObject()
{

	GLuint Object_VBO[3];

	/* Allocate and assign a Vertex Array Object to our handle */
	glGenVertexArrays(1, &Object_VAO);
	glBindVertexArray(Object_VAO);

	glGenBuffers(3, Object_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, Object_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, obj_vertices.size() * sizeof(glm::vec3), &obj_vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, Object_VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, obj_normals.size() * sizeof(glm::vec3), &obj_normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/*bind the third VBO to store the texture data*/
	/*glBindBuffer(GL_ARRAY_BUFFER, Object_VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, obj_UVs.size() * sizeof(glm::vec2), &obj_UVs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);*/

	glBindVertexArray(0);
}

GLuint Object::AddTexture(const char * filename)
{
	GLuint texture1;

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

											// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << filename << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	
	return texture1;
}

GLuint Object::LoadCubeMapTexture(vector<const char*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
