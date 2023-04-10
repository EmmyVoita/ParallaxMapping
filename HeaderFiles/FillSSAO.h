
// GLM Mathematics
#include <glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include <GL/gl.h>

#include <iostream>
#include <random>

#include "Shader2.h"
#include "FillGBuffer.h"
#include "ScreenQuad.h"

using namespace std;

class FillSSAO
{
    private:

    const unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 800;
    unsigned int ssaoFBO, ssaoBlurFBO;
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
    Shader2* SSAOShader;
    Shader2* BlurSSAOShader;
    glm::mat4 ProjectionMatrix;
    glm::mat4 *ViewMatrix;


    std::uniform_real_distribution<GLfloat> randomFloats{0.0f, 1.0f}; // generates random floats between 0.0 and 1.0
    std::mt19937 generator{std::random_device{}()};
    std::vector<glm::vec3> ssaoKernel; 
    

    unsigned int noiseTexture; 

    unsigned int quadVAO = 0;
    unsigned int quadVBO;

    void renderQuad()
    {
        if (quadVAO == 0)
        {
            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void renderQuad(glm::mat4 modelViewProjection)
    {
        if (quadVAO == 0)
        {
            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // transform verticies to world space
            for (int i = 0; i < 4; i++) {
                glm::vec4 pos = glm::vec4(quadVertices[i * 5], quadVertices[i * 5 + 1], quadVertices[i * 5 + 2], 1.0f);
                pos = modelViewProjection * pos;
                quadVertices[i * 5] = pos.x;
                quadVertices[i * 5 + 1] = pos.y;
                quadVertices[i * 5 + 2] = pos.z;
            }
            // gen vao,vbo
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void Init()
    {
        glGenFramebuffers(1, &ssaoFBO);  
        glGenFramebuffers(1, &ssaoBlurFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

        // SSAO color buffer
        glGenTextures(1, &ssaoColorBuffer);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;

        // and blur stage
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glGenTextures(1, &ssaoColorBufferBlur);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // generate sample kernel
        // ----------------------
        for (unsigned int i = 0; i < 128; ++i)
        {
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = float(i) / 128.0f;

            // scale samples s.t. they're more aligned to center of kernel
            scale = ourLerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaoKernel.push_back(sample);
        }

        // generate noise texture
        // ----------------------
       std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++)
        {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
            //cout << noise.x << ":" << noise.y << ":" << noise.z << endl;
            ssaoNoise.push_back(noise);
        }

       
        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    }

    float ourLerp(float a, float b, float f)
    {
        return a + f * (b - a);
    }

    public:

    FillSSAO(glm::mat4 ProjectionMatrix,glm::mat4* ViewMatrix)
    {

        this->ProjectionMatrix = ProjectionMatrix;
        this->ViewMatrix = ViewMatrix;

        SSAOShader = new Shader2("Shaders/SSAO/ssao.vs", "Shaders/SSAO/ssao.frag");
        BlurSSAOShader = new Shader2("Shaders/SSAO/ssao.vs", "Shaders/SSAO/ssao_blur.frag");

        Init();
    }

    ~FillSSAO()
    {
        delete SSAOShader;
        delete BlurSSAOShader;

        glDeleteTextures(1, &ssaoColorBuffer);
        glDeleteTextures(1, &ssaoColorBufferBlur);
        glDeleteTextures(1, &noiseTexture);
        glDeleteFramebuffers(1, &ssaoFBO);
        glDeleteFramebuffers(1, &ssaoBlurFBO);
        glDeleteBuffers(1, &quadVBO);
        glDeleteVertexArrays(1, &quadVAO);
    }

	void GenerateSSAO(FillGBuffer* fillGBuffer)
	{
        SSAOShader->Use();

        // 2. generate SSAO texture
        // ------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Send kernel + rotation 
            for (unsigned int i = 0; i < 128; ++i)
            {
                SSAOShader->setVec3f(ssaoKernel[i], ("samples[" + std::to_string(i) + "]").c_str());
            }
                

            SSAOShader -> setMat4fv(this->ProjectionMatrix, "projection", false);
            SSAOShader -> setMat4fv(*this->ViewMatrix, "View", false);
            SSAOShader->Use();

            //Bind Textures
            fillGBuffer-> BindGBufferPositionNormal(SSAOShader);

            SSAOShader->Use();
            glUniform1i(glGetUniformLocation(SSAOShader->Program, "texNoise"), 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}	

    void BlurSSAO()
    {
        BlurSSAOShader->Use();

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUniform1i(glGetUniformLocation(BlurSSAOShader->Program, "ssaoInput"), 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int getSSAOTexture()
    {
        return ssaoColorBufferBlur;
    }

};