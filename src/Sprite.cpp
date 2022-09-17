#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Sprite.hpp"
#include "ShaderProgram.h"
#include <stb_image.h>
#include <iostream>

 void Sprite::Setup(const char* vrtxShaderPath, const char* frgmtShaderPath) {
    shaderProgram.Setup(vrtxShaderPath, frgmtShaderPath);
    initRenderData();
    upperSide = 0.88;
    lowerSide = 0.56;
    rightSide = 0.65;
    leftSide = 0.1;

}

void Sprite::DrawSprite()
{
    // prepare transformations
    shaderProgram.activate();

 
    glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 color = glm::vec3(1.0, 0.5, 0.5);
    shaderProgram.setMat4Uniform("model", model);

    // render textured quad
    shaderProgram.setVec3Uniform("spriteColor", color);

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
	unsigned char* data = stbi_load("../res/textures/square.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
void Sprite::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = { 
        // pos      // tex
        0.05f, 0.88f, 0.0f, 1.0f,
        0.25f, 0.58f, 1.0f, 0.0f,
        0.05f, 0.58f, 0.0f, 0.0f, 

        0.05f, 0.88f, 0.0f, 1.0f,
        0.25f, 0.88f, 1.0f, 1.0f,
        0.25f, 0.58f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void Sprite::DeleteVAO() {
    glDeleteVertexArrays(1, &this->quadVAO);
    glDeleteBuffers(1, &this->quadVAO);
}