#include "shader.h"


string shader::readshader(const char* shadername)
{
	ifstream input(shadername);
	if (!input.good())
	{
		cout << shadername << "file failed to load...." << endl;
		exit(1);
	}
	else {
		//cout << filename << "shader loaded successfully...." << endl;
	}
	return string(
		istreambuf_iterator<char>(input),
		istreambuf_iterator<char>()
	);
	
}

void shader::printShaderInfoLog(GLuint obj)
{
	GLint compilestatus;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &compilestatus);
	if (compilestatus != GL_TRUE)
	{
		GLint infolength;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infolength);
		GLchar *buffer = new GLchar[infolength];

		GLsizei buffersize;
		glGetShaderInfoLog(obj, infolength, &buffersize, buffer);
		cout << buffer << endl;
		delete[] buffer;
	}
}

void shader::printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}


shader::shader(const GLchar * vertexshader_path, const GLchar * fragmentshader_path)
{

	
	string vertex_temp = readshader(vertexshader_path);
	string fragment_temp = readshader(fragmentshader_path);

	vertexsource = vertex_temp.c_str();
	fragmentsource = fragment_temp.c_str();

	GLuint vertexshader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);

	VShader = vertexshader;
	FShader = fragmentshader;

	glShaderSource(vertexshader, 1, &vertexsource, NULL);
	glShaderSource(fragmentshader, 1, &fragmentsource, NULL);

	glCompileShader(vertexshader);
	glCompileShader(fragmentshader);

	printShaderInfoLog(vertexshader); // check loading error of vertex shader
	printShaderInfoLog(fragmentshader); // check loading error of fragment shader

	/*program of consisting two shaders*/
	shaderprogramID = glCreateProgram();

	/*attach the shader to our program*/
	glAttachShader(shaderprogramID, vertexshader);
	glAttachShader(shaderprogramID, fragmentshader);

	/*link the program*/
	glLinkProgram(shaderprogramID);

	/*check the error of linking shader program*/
	printProgramInfoLog(shaderprogramID);


}

GLuint shader::get_unifrom_location( const GLchar* uniform_name)
{
	return glGetUniformLocation(shaderprogramID, uniform_name);
}

GLuint shader::get_attrib_location( const GLchar * attrib_name)
{
	return glGetAttribLocation(shaderprogramID, attrib_name);
}

void shader::setuniform1f(const GLchar * uniform_name, float value)
{
	glUniform1f(glGetUniformLocation(shaderprogramID,uniform_name),value);
}

void shader::setuniform3f(const GLchar * uniform_name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(shaderprogramID, uniform_name), x,y,z);

}

void shader::setuniform3f(const GLchar * uniform_name, const glm::vec3 & value)
{
	glUniform3fv(glGetUniformLocation(shaderprogramID, uniform_name),1,&value[0]);

}

void shader::setmat3(const GLchar * uniform_name, const glm::mat3 & value)
{
	glUniformMatrix3fv(glGetUniformLocation(shaderprogramID, uniform_name), 1, false, &value[0][0]);

}

void shader::setmat4(const GLchar * uniform_name, const glm::mat4 & value)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderprogramID, uniform_name), 1, false, &value[0][0]);

}

void shader::setInt(const GLchar * uniform_name, const int & value)
{
	glUniform1i(glGetUniformLocation(shaderprogramID, uniform_name), value);


}



