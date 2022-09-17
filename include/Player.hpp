#pragma once

#include "ShaderProgram.h"
#include "Platform.hpp"
#include "Coin.hpp"
#include <Sprite.hpp>
#include <Sprite.hpp>


enum Player_Movement { UP, RIGHT, LEFT };


class Player {
public:
	// Functions
	void Setup(int vertices, float positionAttribute[], const char* vrtxShaderPath, const char* frgmtShaderPath, float radius);
	void Draw(float deltaTime, int numOfPlatforms, std::vector<Platform>& platforms, std::vector<Coin>& coins,Sprite sprite);
	void Move(Player_Movement key, float deltaTime);
	void GetHyper();
	void BeNormal();
	void DeleteVAO();
	bool DetectHandCollision(Sprite sprite);
	bool collected = false;
	int coinscollected;
private:
	unsigned int vaoId;
	ShaderProgram shaderProgram;

	int numOfVertices;
	float circleRadius;
	glm::vec3 playerPosition;

	// Movement variables
	float timer = 0.0f;
	bool tired = false;
	float speedup = 0.0f;
	float hyperTime = 2.0f;
	float cooldownTime = 1.0f;

	float velocityY = 0.0f;
	const float velocityX = 0.5f;

	const float gravity = 0.0079f;
	const float kickoff = 0.00230f; // initial velocity when jumping

	float lowestPoint, highestPoint, rightmostPosition, leftmostPosition;

	// Ground variables
	bool onGround = false;
	const float groundUpperline = -0.8f;

	// collision varaibles
	float closestX, closestY, distanceX, distanceY;


	// Functions
	bool DetectCollision(Platform platform);
	bool DetectCoinCollision(Coin coin);

	void Collide(Platform platform);
	void Collect(Coin coin);
	void CollectHand(Sprite sprite);
};
