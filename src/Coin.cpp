#include <GLAD/glad.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include "ShaderProgram.h"
#include "Ground.hpp"
#include <Coin.hpp>
#include <stb_image.h>
#include <iostream>
Coin::Coin(float positionAttribute[], unsigned int positionIndices[], const char* vrtxShaderPath, const char* frgmtShaderPath)
	:Ground(positionAttribute, positionIndices, vrtxShaderPath, frgmtShaderPath) { // call parent version

	originalColliderUpperSide = upperSide;
	originalColliderLowerSide = lowerSide;
	originalColliderRightSide = rightSide;
	originalColliderLeftSide = leftSide;
}

void Coin::Draw(glm::vec3 translationVector) {
	glBindVertexArray(vaoId);
	shaderProgram.activate();

	upperSide = originalColliderUpperSide + translationVector.y;
	lowerSide = originalColliderLowerSide + translationVector.y;
	rightSide = originalColliderRightSide + translationVector.x;
	leftSide = originalColliderLeftSide + translationVector.x;


	glm::mat4 modelMat = glm::mat4(1.0f); // local -> world
	modelMat = glm::translate(modelMat, translationVector);

	shaderProgram.setMat4Uniform("modelMat", modelMat);

	//glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 70);
	shaderProgram.deactivate();
	glBindVertexArray(0);
}
void Coin::DeleteVAO() {
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vaoId);
}
