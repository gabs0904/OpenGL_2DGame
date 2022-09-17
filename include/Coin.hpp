#pragma once

#include <GLM/glm.hpp>
#include "Ground.hpp"
#include "ShaderProgram.h"
#include "Platform.hpp"

class Coin : public Ground {
public:
	Coin(float positionAttribute[], unsigned int positionIndices[], const char* vrtxShaderPath, const char* frgmtShaderPath);
	void Draw(glm::vec3 translationVector);
	void DeleteVAO();

private:
	int numOfVertices;
	int numOfCoins;
	float circleRadius=0.06f;
	float originalColliderUpperSide, originalColliderLowerSide, originalColliderRightSide, originalColliderLeftSide;
};