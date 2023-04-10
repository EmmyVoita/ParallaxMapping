
#pragma once


#include<iostream>
#include<fstream>
#include<string>
#include<vector>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include<glm/vec2.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>


#include<SOIL/SOIL.h>

#include"Shader2.h"

class Light
{
protected:
	float intensity;
	glm::vec3 color;

public:
	Light(float intensity, glm::vec3 color)
	{
		this->intensity = intensity;
		this->color = color;
	}
	
	~Light()
	{

	}

	//Functions
	virtual void sendToShader(Shader2* ourShader) = 0;
};

class PointLight : public Light
{
protected:
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;

public:
	PointLight(glm::vec3 position, float intensity, glm::vec3 color,
		float constant = 1.f, float linear = 0.045f, float quadratic = 0.0025f)
		: Light(intensity, color)
	{
		this->position = position;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
	}

	~PointLight()
	{

	}

	void setPosition(const glm::vec3 position)
	{
		this->position = position;
	}
	
	glm::vec3 getPosition()
	{
		return this->position;
	}

	float getLinear()
	{
		return this->linear;
	}

	float getQuadratic()
	{
		return this->quadratic;
	}

	float getConstant()
	{
		return this->constant;
	}

	float getIntensity()
	{
		return this->intensity;
	}
	
	glm::vec3 getColor()
	{
		return this->color;
	}

	glm::mat4 getModelMatrix()
	{
		glm::mat4 modelMatrix = glm::mat4(1.f);
		modelMatrix = glm::translate(modelMatrix, this->position);
		return modelMatrix;
	}

	void sendToShader(Shader2*  screenQuadShader)
	{
		//string index = "[" + i + "]";
       /*screenQuadShader->setVec3f(this->position, "lights" + index + ".Position");
        screenQuadShader->set1f(this->intensity, "lights" + index + ".Intensity");
		screenQuadShader->setVec3f(this->color, "lights" + index + ".Color");
		screenQuadShader->set1f(this->constant, "lights" + index + ".Constant");
		screenQuadShader->set1f(this->linear, "lights" + index + ".Linear");
		screenQuadShader->set1f(this->quadratic, "lights" + index + ".Quadratic");*/
	}
};
