#pragma once


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "ShaderProgram.h"
class Sprite
{
public:
    void Setup(const char* vrtxShaderPath, const char* frgmtShaderPath);
    void DrawSprite();
    float upperSide, lowerSide, rightSide, leftSide;
    void DeleteVAO();
private:
    // Render state
    ShaderProgram shaderProgram;
    unsigned int quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();

};
