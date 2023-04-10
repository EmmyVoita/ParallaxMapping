#pragma once

// GLM Mathematics
#include <glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "Shader2.h"
#include "Camera.h"

using namespace std;

class ScreenQuad
{
    private:

    GLuint quadVAO, quadVBO, quadEBO;
    Shader2* screenQuadShader;
    vector<MyModelClass*> myModels;

    void Init()
    {
       
        float quadVertices[] = {
            // positions     // texture coordinates
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f,
        };
        unsigned int quadIndices[] = {
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
        };

        // generate vertex array object
        glGenVertexArrays(1, &quadVAO);
        glBindVertexArray(quadVAO);

        // generate vertex buffer object
        glGenBuffers(1, &quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // specify vertex attribute pointers         
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // generate index buffer object
        glGenBuffers(1, &quadEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

        // unbind buffers and VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    }

    public:

    ScreenQuad(vector<MyModelClass*> myModels)
    {
        this->myModels = myModels;
        screenQuadShader = new Shader2("Shaders/renderScreenToQuad.vs", "Shaders/renderScreenToQuad.frag");

        Init();
    }
    ~ScreenQuad()
    {
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
        glDeleteBuffers(1, &quadEBO);
        delete screenQuadShader;
    }

    void renderScreenQuad() 
    {
        screenQuadShader->Use();

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void PassShaderUniforms(Camera* camera, vector<PointLight*> &pointLights, glm::mat4 ViewMatrix)
    {
        screenQuadShader->setVec3f(camera->getPosition(), "cameraPos");
        //cout << camera->getPosition().x << " : " << camera->getPosition().y << " : " << camera->getPosition().z << endl;
        screenQuadShader -> setMat4fv(glm::inverse(ViewMatrix), "invView", false);
        //screenQuadShader -> setMat4fv(ViewMatrix, "invView", false);

       
        for (unsigned int i = 0; i < pointLights.size(); i++)
        {
            string index = "[" + to_string(i) + "]";
            screenQuadShader->setVec3f(pointLights[i]->getPosition(), ("lights" + index + ".Position").c_str());
            screenQuadShader->set1f(pointLights[i]->getIntensity(), ("lights" + index + ".Intensity").c_str());
            screenQuadShader->setVec3f(pointLights[i]->getColor(), ("lights" + index + ".Color").c_str());
            screenQuadShader->set1f(pointLights[i]->getConstant(), ("lights" + index + ".Constant").c_str());
            screenQuadShader->set1f(pointLights[i]->getLinear(), ("lights" + index + ".Linear").c_str());
            screenQuadShader->set1f(pointLights[i]->getQuadratic(), ("lights" + index + ".Quadratic").c_str());
        }
    }

    Shader2* getScreenQuadShader()
    {
        return screenQuadShader;
    }

};


