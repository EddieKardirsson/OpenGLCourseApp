#include <cstdio>
#include <cstring>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


/**		Note to self!
 *	Identity matrices cannot be initialized: glm::mat4 model;
 *	Instead use either of these two initializers:
 *
 *	glm::mat4 model(1.f);
 *	glm::mat4 model = glm::mat4(1.f);
 *
 *	For re-initializations, use the following:
 *
 *	model = glm::mat(1.f);
 */


// Window dimensions
const GLint WIDTH = 1000, HEIGHT = 750;
const float TO_RADIANS = 3.14159265f / 180.f;

GLuint VAO, VBO, IBO, shader, uniformModel;

bool direction = true;
float triOffset = 0.f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

float curAngle = 0.f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Vertex Shader
static const char* vShader = "												\n\
# version 330																\n\
																			\n\
layout (location = 0) in vec3 pos;											\n\
																			\n\
out vec4 vCol;																\n\
																			\n\
uniform mat4 model;															\n\
																			\n\
void main()																	\n\
{																			\n\
	gl_Position = model * vec4(pos, 1.0); 									\n\
	vCol = vec4(clamp(pos, 0.f, 1.0f), 1.0f);								\n\
}";

// Fragment Shader
static const char* fShader = "												\n\
# version 330																\n\
																			\n\
in vec4 vCol;																\n\
																			\n\
out	vec4 colour; 															\n\
																			\n\
void main()																	\n\
{																			\n\
	colour = vCol; 															\n\
}";


void CreateTriangle()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {	// index: 
		-1.f, -1.f, 0.f,	// 0
		0.f, -1.f, 1.f,		// 1
		1.f, -1.f, 0.f,		// 2
		0.f, 1.f, 0.f		// 3
	};

	
	glGenVertexArrays(1, &VAO);	// Params: Amount of arrays; Where to store the values and pass it by reference.
	glBindVertexArray(VAO);			// Param: Bind the vertex array (VAO).

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glGenBuffers(1, &VBO);		// Params: Amount of arrays; values stored and pass by reference
		glBindBuffer(GL_ARRAY_BUFFER, VBO);		// Params: Which buffer, choose enum and in this case array buffer; The buffer to bind (VBO)

			// glBufferData params: target buffer; size of the data; actual array; static or dynamic draw
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// glVertextAttribPointer params: index; amount of vertices; type of the values; normalize or not; stride value or not; offset where the data starts
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);		// param: enable the array by index


		glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbind the VBO binding

	glBindVertexArray(0);			// Unbind the VAO binding
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

/** Params: shader; shader code; which shader? */
void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
	// Initialize the shader program
	shader = glCreateProgram();

	if(!shader)
	{
		printf("\nError creating shader!\n");
		return;
	}

	// Add vertex and fragment shaders to the program
	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	// Link the program to the GPU
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if(!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	// Validate the program
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	uniformModel = glGetUniformLocation(shader, "model");
}

int main()
{
	// Initialize GLFW
	if(!glfwInit())
	{
		printf("\nGLFW Initialization failed\n");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No Backwards Compability
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compability
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if(!mainWindow)
	{
		printf("\nGLFW window creation failed\n");
		glfwTerminate();
		return 2;
	}

	// Get Buffer size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if(glewInit() != GLEW_OK)
	{
		printf("\nGLEW initialization failed!\n");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 3;
	}

	// Depth buffer
	glEnable(GL_DEPTH_TEST);

	// Setup Viewport Size
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();

	// Loop until window closed
	while(!glfwWindowShouldClose(mainWindow))
	{
		// Get + Handle user input events
		glfwPollEvents();

		if (direction) 
			triOffset += triIncrement;
		else
			triOffset -= triIncrement;

		if (abs(triOffset) >= triMaxOffset)
			direction = !direction;

		curAngle += 0.1f;
		if (curAngle >= 360) curAngle -= 360;


		if (sizeDirection)
			curSize += 0.001f;
		else
			curSize -= 0.001f;

		if (curSize >= maxSize || curSize <= minSize)
			sizeDirection = !sizeDirection;

		// Clear window
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);

		glm::mat4 model = glm::mat4(1.f);
		//model = glm::translate(model, glm::vec3(triOffset, 0.f, 0.f));
		model = glm::rotate(model, curAngle * TO_RADIANS, glm::vec3(0.f, 1.f, 0.f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.f));
				
		// Params: model location; amount; transpose; pointer to the value;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

			glBindVertexArray(VAO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

					glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}