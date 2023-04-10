#pragma once

#include<iostream>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

enum direction {NONE, FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN, PITCHUP, PITCHDOWN, YAWUP, YAWDOWN};

class Camera
{
private:
 	
	glm::mat4* ViewMatrix;

	float keyDownTime;
	float speed;

	GLfloat movementSpeed;
	GLfloat sensitivity;

	glm::vec3 worldUp;
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	GLfloat pitch;
	GLfloat yaw;
	GLfloat roll;

	void updateCameraVectors()
	{
		this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front.y = sin(glm::radians(this->pitch));
		this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

		this->front = glm::normalize(this->front);
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->right, this->front));

		*this->ViewMatrix = glm::lookAt(this->position, this->position + this->front, this->worldUp);

		//cout << this->position.x << " : " << this->position.y << " : " << this->position.z << endl;

	}

public:
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp, glm::mat4* ViewMatrix)
	{
		this->ViewMatrix = ViewMatrix;


		this->keyDownTime = 0.0;
		this->speed = 1.0;

		this->movementSpeed = 10.f;
		this->sensitivity = 50.f;

		this->worldUp = worldUp;
		this->position = position;
		this->right = glm::vec3(0.f);
		this->up = worldUp;

		this->pitch = -0.f;
		this->yaw = -90.f;
		this->roll = 0.f;

		this->updateCameraVectors();
	
	}

	~Camera() {}


	const glm::vec3 getPosition() const
	{
		return this->position;
	}

	//Functions
	void move(const float& dt, const int direction)
	{

		//Update position vector
		switch (direction)
		{
		case FORWARD:
			this->position += this->front * this->movementSpeed * dt;
			break;
		case BACKWARD:
			this->position -= this->front * this->movementSpeed * dt;
			break;
		case LEFT:
			this->position -= this->right * this->movementSpeed * dt;
			break;
		case RIGHT:
			this->position += this->right * this->movementSpeed * dt;
			break;
		case UP:
			this->position += this->up * this->movementSpeed * dt;
			break;
		case DOWN:
			this->position -= this->up * this->movementSpeed * dt;
			break;
		case PITCHUP:
			this->pitch += this->sensitivity * dt;
			if (this->pitch > 89.f)
				this->pitch = 89.f;
			break;
		case PITCHDOWN:
			this->pitch -= this->sensitivity * dt;
			if (this->pitch < -89.f)
				this->pitch = -89.f;
			break;
		case YAWUP:
			this->yaw += this->sensitivity * dt;
			break;
		case YAWDOWN:
			this->yaw -= this->sensitivity * dt;
			break;
				
		default:
			break;
		}

		// Update camera vectors and view matrix
		this->updateCameraVectors();
	}

	void updateMouseInput(const float& dt, const double& offsetX, const double& offsetY)
	{
		//Update pitch yaw and roll
		this->pitch += static_cast<GLfloat>(offsetY) * this->sensitivity * dt;
		this->yaw += static_cast<GLfloat>(offsetX) * this->sensitivity * dt;

		if (this->pitch > 80.f)
			this->pitch = 80.f;
		else if (this->pitch < -80.f)
			this->pitch = -80.f;

		if (this->yaw > 360.f || this->yaw < -360.f)
			this->yaw = 0.f;

		this->updateCameraVectors();
	}

	void updateInput(const float& dt, const int direction, const double& offsetX, const double& offsetY)
	{

		
		this->updateMouseInput(dt, offsetX, offsetY);
	}

};



