#include <GLAD/glad.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <vector>

#include <Player.hpp>
#include "ShaderProgram.h"
#include <iostream>
#include <stb_image.h>


// Public Functions:
void Player::Setup(int vertices, float positionAttribute[], const char* vrtxShaderPath, const char* frgmtShaderPath, float radius) {
	circleRadius = radius;
	numOfVertices = vertices;
	playerPosition.y = -0.4f;
	highestPoint = 1.0f - circleRadius;
	lowestPoint = groundUpperline + circleRadius;
	

	// Create vertex array object
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	// Create vertex buffer object
	unsigned int positionVBO;
	glGenBuffers(1, &positionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, numOfVertices * 3 * sizeof(float), positionAttribute, GL_DYNAMIC_DRAW); // put data in buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float))); // specify data layout
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)(6 * sizeof(float)));
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// Assign shaders to shader program
	shaderProgram.Setup(vrtxShaderPath, frgmtShaderPath);

	// Unbind VAO & VBO

	glBindVertexArray(0);
}

void Player::Draw(float deltaTime, int numOfPlatforms, std::vector<Platform>& platforms, std::vector<Coin>& coins, Sprite sprite) {
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
	unsigned char* data = stbi_load("../res/textures/HootyFace.jpg", &width, &height, &nrChannels, 0);
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
	
	
	
	glBindVertexArray(vaoId);
	shaderProgram.activate();

	// Make sure player doesn't get out of screen
	playerPosition.x = glm::max(-1.0f + circleRadius, glm::min(1.0f - circleRadius, playerPosition.x));
	playerPosition.y = glm::min(highestPoint, glm::max(playerPosition.y + velocityY, lowestPoint));

	// Check if player is on ground
	onGround = false;
	if (playerPosition.y == lowestPoint) onGround = true;

	// limit hyper mode time
	if (speedup > 0.1f) {
		timer += deltaTime;
		shaderProgram.setBoolUniform("hyper", true);
		if (timer > hyperTime) tired = true, speedup = 0.0f, timer = 0.0f;
	}
	else {
		shaderProgram.setBoolUniform("hyper", false);
	}

	// prevent entering hyper mode until cooldown
	if (tired) {
		timer += deltaTime;
		if (timer > cooldownTime) tired = false, timer = 0.0f;
	}

	// Create model matrix (local space -> world space)
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, playerPosition);
	shaderProgram.setMat4Uniform("modelMat", modelMat);

	// Check if collisions occur
	for (int i = 0; i < numOfPlatforms; i++)
		if (DetectCollision(platforms[i])) Collide(platforms[i]);

	for (int i = 0; i < 10; i++)
		if (DetectCoinCollision(coins[i])) Collect(coins[i]);
	if (DetectHandCollision(sprite)) CollectHand(sprite);
	// Draw player
	glDrawArrays(GL_TRIANGLE_FAN, 0, numOfVertices+2);


	// Calculate new "velocityY" value
	if (onGround)                              velocityY = 0.0f;
	else if (playerPosition.y == highestPoint) velocityY = glm::min(0.0f, velocityY - gravity * deltaTime);
	else                                       velocityY -= gravity * deltaTime;


	shaderProgram.deactivate();
	glBindVertexArray(0);
}


void Player::Move(Player_Movement key, float deltaTime) {
	if (key == UP && onGround) velocityY= 10*kickoff + (speedup/20) ; // jump

	if (key == RIGHT) playerPosition.x += (velocityX + speedup) * deltaTime; // move right
	if (key == LEFT)  playerPosition.x -= (velocityX + speedup) * deltaTime; // move left
}


void Player::GetHyper() { if (!tired) speedup = 0.5f; }
void Player::BeNormal() { speedup = 0.0f; timer = 0.0f; }


void Player::DeleteVAO() {
	glDeleteVertexArrays(1, &vaoId);
	glDeleteBuffers(1, &vaoId);
}




// Private Functions:

bool Player::DetectCollision(Platform platform) {
	// calculate the closest point on platform to circle
	closestX = glm::max(platform.leftSide, glm::min(playerPosition.x, platform.rightSide));
	closestY = glm::max(platform.lowerSide, glm::min(playerPosition.y, platform.upperSide));

	// calculate square distance between point and circle center
	distanceX = playerPosition.x - closestX;
	distanceY = playerPosition.y - closestY;
	float distanceSquare = (distanceX * distanceX) + (distanceY * distanceY);


	// if circle collides with platform
	return (distanceSquare < (circleRadius* circleRadius));
}


void Player::Collide(Platform platform) {
	// circle equation: (x - centerX) ^ 2	+ (y - centerY) ^ 2 = (radius) ^ 2
	// application    :    firstTerm        +     secondTerm    = radiusSquare
	float radiusSquare = circleRadius * circleRadius;

	if (closestY == platform.upperSide && glm::abs(distanceX) < (0.5f * circleRadius)) { // circle is above platform

		if (closestX == platform.leftSide) { // upper left corner
			float firstTerm = (playerPosition.x - platform.leftSide) * (playerPosition.x - platform.leftSide);
			playerPosition.y = glm::sqrt(radiusSquare - firstTerm) + platform.upperSide;

		}
		else if (closestX == platform.rightSide) { // upper right corner
			float firstTerm = (playerPosition.x - platform.rightSide) * (playerPosition.x - platform.rightSide);
			playerPosition.y = glm::sqrt(radiusSquare - firstTerm) + platform.upperSide;

		}
		else {
			playerPosition.y = glm::max(playerPosition.y, platform.upperSide + circleRadius);
		}

		onGround = true;


	}
	else if (closestY == platform.lowerSide) { // circle is below platform
		velocityY = glm::min(velocityY, 0.0f);

		if (closestX == platform.leftSide) { // lower left corner
			float firstTerm = (playerPosition.x - platform.leftSide) * (playerPosition.x - platform.leftSide);
			playerPosition.y = -glm::sqrt(radiusSquare - firstTerm) + platform.lowerSide;

		}
		else if (closestX == platform.rightSide) { // lower right corner
			float firstTerm = (playerPosition.x - platform.rightSide) * (playerPosition.x - platform.rightSide);
			playerPosition.y = -glm::sqrt(radiusSquare - firstTerm) + platform.lowerSide;

		}
		else {
			playerPosition.y = glm::min(playerPosition.y, platform.lowerSide - circleRadius);
		}


	}
	else if (closestX == platform.leftSide) { // circle is left of platform
		playerPosition.x = platform.leftSide - circleRadius;

	}
	else if (closestX == platform.rightSide) { // circle is right of platform
		playerPosition.x = platform.rightSide + circleRadius;

	}
}
bool Player::DetectCoinCollision(Coin coin) {
	// calculate the closest point on platform to circle
	closestX = glm::max(coin.leftSide, glm::min(playerPosition.x, coin.rightSide));
	closestY = glm::max(coin.lowerSide, glm::min(playerPosition.y, coin.upperSide));

	// calculate square distance between point and circle center
	distanceX = playerPosition.x - closestX;
	distanceY = playerPosition.y - closestY;
	float distanceSquare = (distanceX * distanceX) + (distanceY * distanceY);


	return (distanceSquare < (circleRadius* circleRadius));
}


void Player::Collect(Coin coin) {
	coin.DeleteVAO();
	coinscollected++;
}

bool Player::DetectHandCollision(Sprite sprite) {
	// calculate the closest point on platform to circle
	closestX = glm::max(sprite.leftSide, glm::min(playerPosition.x, sprite.rightSide));
	closestY = glm::max(sprite.lowerSide, glm::min(playerPosition.y, sprite.upperSide));

	// calculate square distance between point and circle center
	distanceX = playerPosition.x - (closestX);
	distanceY = playerPosition.y - closestY;
	float distanceSquare = (distanceX * distanceX) + (distanceY * distanceY);


	// if circle collides with platform
	return (distanceSquare < (circleRadius* circleRadius));
}

void Player::CollectHand(Sprite sprite) {
	sprite.DeleteVAO();
	collected = true;
}