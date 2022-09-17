#include <GLAD/glad.h>
#include <GLM/glm.hpp>

#include "Ground.hpp"
#include "ShaderProgram.h"
#include <stb_image.h>
#include <iostream>

// Constructor
Ground::Ground(float positionAttribute[],unsigned int positionIndices[], const char* vrtxShaderPath, const char* frgmtShaderPath) {
	upperSide = glm::max(positionAttribute[1], glm::max(positionAttribute[4], positionAttribute[7]));
	lowerSide = glm::min(positionAttribute[1], glm::min(positionAttribute[4], positionAttribute[7]));
	rightSide = glm::max(positionAttribute[0], glm::max(positionAttribute[3], positionAttribute[6]));
	leftSide = glm::min(positionAttribute[0], glm::min(positionAttribute[3], positionAttribute[6]));

	// Create vertex array object
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	// Create vertex buffer object 
	unsigned int positionVBO;
	glGenBuffers(1, &positionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(float), positionAttribute, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)(6 * sizeof(float)));
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// Create element buffer object
	unsigned int positionEBO;
	glGenBuffers(1, &positionEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, positionEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), positionIndices, GL_STATIC_DRAW);






	// Assign shaders to shader program
	shaderProgram.Setup(vrtxShaderPath, frgmtShaderPath);

	// Unbind VAO
	glBindVertexArray(0);
}


// Public Functions:

void Ground::Draw() {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load("../res/textures/ground1.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(vaoId);
	shaderProgram.activate();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	shaderProgram.deactivate();
	glBindVertexArray(0);
}

void Ground::DeleteVAO() {
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vaoId);
}