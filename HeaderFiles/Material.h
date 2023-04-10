#pragma once

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/vec2.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Shader2.h"

class Material
{
private:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLint diffuseTex;
	GLint shadowMap;
	GLint normalTex;
	GLint displacementTex;
	GLint glossMap;
	GLint roughnessMap;
	float alpha;
	glm::vec2 textureScale;
	float shininess;

public:
	Material(
	glm::vec3 ambient,
	glm::vec3 diffuse,
	glm::vec3 specular,
	GLint diffuseTex,
	GLint shadowMap, 
	float alpha,
	glm::vec2 textureScale,
	float shininess
	)
	{
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->diffuseTex = diffuseTex;
		this->shadowMap = shadowMap;
		this->textureScale = textureScale;
		this->alpha = alpha;
		this->shininess = shininess;
	}

	Material(
	glm::vec3 ambient,
	glm::vec3 diffuse,
	glm::vec3 specular,
	GLint diffuseTex,
	GLint shadowMap,
	GLint displacementTex,
	GLint normalTex,
	float alpha,
	glm::vec2 textureScale,
	float shininess
	)
	{
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->diffuseTex = diffuseTex;
		this->shadowMap = shadowMap;
		this->displacementTex = displacementTex;
		this->normalTex = normalTex;
		this->textureScale = textureScale;
		this->alpha = alpha;
		this->shininess = shininess;
	}

	Material(
	glm::vec3 ambient,
	glm::vec3 diffuse,
	glm::vec3 specular,
	GLint diffuseTex,
	GLint shadowMap, 
	GLint normalMap,
	GLint displacementTex,
	GLint glossMap,
	GLint roughnessMap,
	float alpha,
	glm::vec2 textureScale
	)
	{
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->diffuseTex = diffuseTex;
		this->shadowMap = shadowMap;
		this->textureScale = textureScale;
		this->normalTex = normalTex;
		this->displacementTex = displacementTex;
		this->roughnessMap = roughnessMap;
		this->glossMap = glossMap;

		this->alpha = alpha;
	}

	~Material(){}


	GLint getDiffuseID()
	{
		return diffuseTex;
	}

	GLint getDisplacementID()
	{
		return displacementTex;
	}

	GLint getShadowMapID()
	{
		return shadowMap;
	}

	GLint getNormalID()
	{
		return normalTex;
	}

	GLint getGlossMapID()
	{
		return glossMap;
	}

	GLint getRoughnessMapID()
	{
		return roughnessMap;
	}
	//Function
	void sendToShader(Shader2 &ourShader)
	{
		ourShader.setVec3f(this->ambient, "material.ambient");
		ourShader.setVec3f(this->diffuse, "material.diffuse");
		ourShader.setVec3f(this->specular, "material.specular");
		ourShader.set1i(this->shadowMap, "material.shadowMap");
		ourShader.set1i(this->diffuseTex, "material.diffuseTex");
		ourShader.set1f(this->alpha, "material.alpha");
		//ourShader.set1f(this->shininess, "material.shininess");
		ourShader.setVec2f(this->textureScale, "material.textureScale");
	}

	void sendToGBuffer(Shader2 &ourShader)
	{
		ourShader.set1i(this->shadowMap, "material.shadowMap");
		ourShader.set1i(this->diffuseTex, "material.diffuseTex");
		ourShader.set1i(this->displacementTex, "material.displacementTex");
		ourShader.set1i(this->normalTex, "material.normalTex");
		ourShader.setVec2f(this->textureScale, "material.textureScale");
	}

};
