#pragma once
// GLM Mathematicsclear

#include <glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "Shader2.h"

using namespace std;

class FillGBuffer
{
    private:

    const unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 800;
    GLuint gBuffer;
    GLuint gViewPos, gNormal, gAlbedoSpec, gDisplacement, gTangent, gBitangent, gTexNormal, gModelMatrix, gModelNormal;
    vector<MyModelClass*> myModels;
    Shader2* gBufferShader;

    void Init()
    {

        GLenum err2;
        while ((err2 = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error 22: " << err2 << std::endl;
        }
       
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        
        
        // - position color buffer
        glGenTextures(1, &gViewPos);
        glBindTexture(GL_TEXTURE_2D, gViewPos);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
        glGenerateMipmap(GL_TEXTURE_2D);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gViewPos, 0);

        // - normal color buffer
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
        
        // - color + specular color buffer
        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

        // - displacement
        glGenTextures(1, &gDisplacement);
        glBindTexture(GL_TEXTURE_2D, gDisplacement);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gDisplacement, 0);

        // - Tangent
        glGenTextures(1, &gTangent);
        glBindTexture(GL_TEXTURE_2D, gTangent);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F , SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gTangent, 0);

        // - BiTangent
        glGenTextures(1, &gBitangent);
        glBindTexture(GL_TEXTURE_2D, gBitangent);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F , SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gBitangent, 0);

        // - normal texture override color buffer
        glGenTextures(1, &gTexNormal);
        glBindTexture(GL_TEXTURE_2D, gTexNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, gTexNormal, 0);

          // - position color buffer
        glGenTextures(1, &gModelMatrix);
        glBindTexture(GL_TEXTURE_2D, gModelMatrix);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, GL_TEXTURE_2D, gModelMatrix, 0);


        // then also add render buffer object as depth buffer and check for completeness.
        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        
        // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
        GLenum attachments[8] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7};
        glDrawBuffers(8, attachments);

        
        // - check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Framebuffer is not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void RenderModels()
    {
        gBufferShader->Use();

        for(int i = 0; i < myModels.size(); i++)
		{
            //gBufferShader->Use();     

            gBufferShader -> set1i(0, "invertedNormals");
            gBufferShader->Use();
			myModels[i]->RenderForGBuffer(*gBufferShader);
		}
    }

    public:

    FillGBuffer(vector<MyModelClass*> myModels)
    {

        GLenum err2;
        while ((err2 = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error 24: " << err2 << std::endl;
        }

        this->myModels = myModels;

        gBufferShader = new Shader2("Shaders/GVertex.vs", "Shaders/GGeom.gs", "Shaders/GFrag.frag");

        Init();
    }

    ~FillGBuffer()
    {
        glDeleteTextures(1, &gViewPos);
        glDeleteTextures(1, &gNormal);
        glDeleteTextures(1, &gAlbedoSpec);
        glDeleteTextures(1, &gDisplacement);
        glDeleteTextures(1, &gTangent);
        glDeleteTextures(1, &gBitangent);
        glDeleteTextures(1, &gTexNormal);

        glDeleteFramebuffers(1, &gBuffer);

        // Delete gBufferShader
        delete gBufferShader;
    }

	void RenderToGBuffer()
	{

        gBufferShader->Use();

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);                     // bind the G-buffer framebuffer
        
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderModels();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error in RenderModels to gBuffer: " << err << std::endl;
        }
       
        glBindFramebuffer(GL_FRAMEBUFFER, 0);                           // unbind the G-buffer framebuffer
        glBindTexture(GL_TEXTURE_2D, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}	

    void BindAllGBufferTextures(Shader2* screenQuadShader, unsigned int ssaoColorBufferBlur)
    {
        // set the shader program for rendering the screen quad
        
        screenQuadShader->Use();

        //update view matrix


        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gViewPos"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gViewPos);

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gNormal"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gAlbedoSpec"), 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gDisplacement"), 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gDisplacement);

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gTangent"), 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, gTangent);

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gBitangent"), 5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, gBitangent);

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gTexNormal"), 6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, gTexNormal);

       /* glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gModelMatrix"), 7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, gModelMatrix);*/

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "ssao"), 7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);


        GLenum err;
        // Check for errors
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error when binding textures to screenQuad: " << err << std::endl;
        }
    }

    void BindGBufferPositionNormal(Shader2* screenQuadShader)
    {
        screenQuadShader->Use();

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gViewPos"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gViewPos);

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gTexNormal"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gTexNormal);

        GLenum err;
        // Check for errors
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error when binding textures to screenQuad: " << err << std::endl;
        }
    }

    unsigned int getGBuffer()
    {
        return gBuffer;
    }
};


