
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<sstream>
#include<algorithm>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL/SOIL.h>

// GLM Mathematics
#include<glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

// Other includes
#include "HeaderFiles/Shader2.h"
#include "HeaderFiles/OBJloader.h"
#include "HeaderFiles/Vertex.h"
#include "HeaderFiles/Texture.h"
#include "HeaderFiles/MyModelClass.h"
#include "HeaderFiles/Camera.h"
#include "HeaderFiles/Material.h"
#include "HeaderFiles/Light.h"
#include "HeaderFiles/ShadowPassClass.h"
#include "HeaderFiles/FillGBuffer.h"
#include "HeaderFiles/ScreenQuad.h"
#include "HeaderFiles/FillSSAO.h"

using namespace std;


// Window 
const GLuint WIDTH = 800, HEIGHT = 800;
GLFWwindow* window;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && window != nullptr)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


vector<Shader2*> shaders;
vector<PointLight*> pointLights;
vector<Material*> materials; 
vector<Texture*> textures; 
vector<MyModelClass*> models;


float dt = 0.0f;
float curTime = 0.0f;
float lastTime = 0.0f;

double lastMouseX = 0.0;
double lastMouseY = 0.0;
double mouseX = 0.0;
double mouseY = 0.0;
double mouseOffsetX = 0.0;
double mouseOffsetY = 0.0;

bool firstMouse = true;

glm::mat4 ViewMatrix = glm::mat4(1.f);
glm::mat4 ProjectionMatrix = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 1000.0f);
//glm::mat4 ProjectionMatrix = glm::ortho(-1.0f * (float)WIDTH / (float)HEIGHT, 1.0f * (float)WIDTH / (float)HEIGHT, -1.0f, 1.0f, 0.1f, 1000.0f);



Camera* camera;
FillGBuffer* fillGBuffer;
ScreenQuad* screenQuad;
FillSSAO* fillSSAO;


void initWindow()
{
    // Init GLFW
    if (glfwInit() == GLFW_FALSE)
	{
		std::cout << "ERROR::GLFW_INIT_FAILED" << "\n";
		glfwTerminate();
	}

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    window = glfwCreateWindow(WIDTH, HEIGHT, "Deferred Rendering", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "ERROR::GLFW_WINDOW_INIT_FAILED" << "\n";
		glfwTerminate();
	}

    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR::MAIN.CPP::GLEW_INIT_FAILED" << "\n";
		glfwTerminate();
	}

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void updateKeyboardInput()
{
	//Camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->move(dt, FORWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->move(dt, BACKWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->move(dt, LEFT);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->move(dt, RIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		camera->move(dt,DOWN);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera->move(dt,UP);
	}
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		camera->move(dt,PITCHUP);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera->move(dt,PITCHDOWN);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera->move(dt,YAWDOWN);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera->move(dt,YAWUP);
	}
}

void updateDt()
{
	curTime = static_cast<float>(glfwGetTime());
	dt = curTime - lastTime;
	lastTime = curTime;
}

void updateMouseInput()
{
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (firstMouse)
	{
		lastMouseX = mouseX;
		lastMouseY = mouseY;
		firstMouse = false;
	}

	//Calc offset
	mouseOffsetX = mouseX - lastMouseX;
	mouseOffsetY = lastMouseY - mouseY;

	//Set last X and Y
	lastMouseX = mouseX;
	lastMouseY = mouseY;
}

void updateInput()
{
	if(window != nullptr)
	{
		glfwPollEvents();
	}

	updateKeyboardInput();
	//updateMouseInput();
	//camera->updateInput(dt, -1, mouseOffsetX, mouseOffsetY);
}

void cleanUp()
{
	glfwSetKeyCallback(window, NULL);
	//glfwSetCursorPosCallback(window, NULL);
	if(window != nullptr)
	{
		glfwDestroyWindow(window);
	}

    glfwTerminate();

	delete camera;
	
    // Delete all the allocated memory
    
    delete fillGBuffer;
    delete screenQuad;
    delete fillSSAO;


	// Delete all the Shader2 objects
    for (Shader2* shader : shaders) 
	{
        delete shader;
    }
    shaders.clear();

    // Delete all the PointLight objects
    for (PointLight* pointLight : pointLights) 
	{
        delete pointLight;
    }
    pointLights.clear();

    // Delete all the Material objects
    for (Material* material : materials) 
	{
        delete material;
    }
    materials.clear();

    // Delete all the Texture objects
    for (Texture* texture : textures) 
	{
        delete texture;
    }
    textures.clear();

	// Delete all the Model objects
    for (MyModelClass* model : models) 
	{
        delete model;
    }
    models.clear();
	
}

void initShaders()
{
    shaders.push_back(new Shader2("Shaders/BaseVertexShader.vs", "Shaders/BaseFragmentShader.frag"));
    shaders.push_back(new Shader2("Shaders/BaseVertexShader.vs", "Shaders/AdvancedFragmentShader.frag"));
}

void initTextures()
{
    textures.push_back(new Texture("Textures/bricks2_normal.jpg", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/bricks2_disp.jpg", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/brick_diffuse.jpg", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/Stylized_Stone_Floor_005_normal.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/Stylized_Stone_Floor_005_height.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/Stylized_Stone_Floor_005_basecolor.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/Rock_047_Normal.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/Rock_047_Height.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/Rock_047_BaseColor.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/Rocks/rocks normal.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/Rocks/rocks AO blurred.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/Rocks/rocks color blur.png", GL_TEXTURE_2D));

}

void initMaterials()
{
	materials.push_back(new Material(glm::vec3(0.7f), glm::vec3(1.f), glm::vec3(.8f), 0, 1, 2, 3, 1.0, glm::vec2(1.0f, 1.0f), 32.0)); 
	materials.push_back(new Material(glm::vec3(0.7f), glm::vec3(1.f), glm::vec3(.8f), 0, 1, 2, 3, 1.0, glm::vec2(1.0f, 1.0f), 32.0)); 
	materials.push_back(new Material(glm::vec3(0.7f), glm::vec3(1.f), glm::vec3(.8f), 0, 1, 2, 3, 1.0, glm::vec2(1.0f, 1.0f), 32.0)); 
}

void initPointLights()
{
    pointLights.push_back(new PointLight(glm::vec3(0.f,10.0f,6.0f), 1.0f, glm::vec3(157.0f/256.0f,196.f/256.0f,196.f/256.0f)));
    pointLights.push_back(new PointLight(glm::vec3(0.f,5.0f,3.0f), 0.0f, glm::vec3(255.0f/256.0f,245.f/256.0f,0.f/256.0f)));
	pointLights.push_back(new PointLight(glm::vec3(1.0f,0.0f,1.0f), 0.0f, glm::vec3(213.0f/256.0f,235.f/256.0f,234.f/256.0f)));
}
 
void initModels()
{
	models.push_back(new MyModelClass(materials[0], textures[2], textures[1], textures[0], "Models/CUBE_3.txt", ViewMatrix, ProjectionMatrix));
	models.push_back(new MyModelClass(materials[1], textures[5], textures[4], textures[3], "Models/GroundPlane2.txt", ViewMatrix, ProjectionMatrix));
	//models.push_back(new MyModelClass(materials[2], textures[8], textures[7], textures[6], "Models/GroundPlane2.txt", ViewMatrix, ProjectionMatrix));
	//models.push_back(new MyModelClass(materials[2], textures[8], textures[7], textures[6], "Models/GroundPlane2.txt", ViewMatrix, ProjectionMatrix));
    //models.push_back(new MyModelClass(materials[1], textures[2], nullptr, textures[0], "Models/Sphere.txt", ViewMatrix, ProjectionMatrix));
	//models.push_back(new MyModelClass(materials[2], textures[2], textures[1], textures[0], "Models/Sphere.txt", ViewMatrix, ProjectionMatrix));
	
	//Transformations:
    
	//models[0]->setOrigin(glm::vec3(0.5f, 1.0f, 1.0f));
	//models[0]->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	//models[0]->setRotation(glm::vec3(0.0f, 0.f, 0.0f));
	//models[0]->setScale(glm::vec3(4.0f, 4.0f, 4.0f));
	models[0]->setScale(glm::vec3(4.0f, 4.0f, 4.0f));

	//models[1]->setOrigin(glm::vec3(0.f, 0.0f, 0.0f));
	//models[1]->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	//models[1]->setOrigin(glm::vec3(0.5f, 0.0f, 1.0f));
	//models[1]->setRotation(glm::vec3(-90.0f, 0.f, 0.0f));
	models[1]->setScale(glm::vec3(8.0f, 8.0f, 8.0f));

	//models[2]->setOrigin(glm::vec3(0.0f, -1.0f, 1.0f));
	//models[2]->setRotation(glm::vec3(-90.0f, 0.f, 0.0f));
	//models[2]->setScale(glm::vec3(0.95f, 0.95f, 0.95f));

	//models[3]->setOrigin(glm::vec3(1.0f, 0.0f, 1.0f));
	//models[3]->setRotation(glm::vec3(0.0f, -90.f, 0.0f));
	//models[3]->setScale(glm::vec3(0.95f, 0.95f, 0.95f));

	//models[4]->setOrigin(glm::vec3(0.0f, 0.0f, 20.f));
	//models[4]->setScale(glm::vec3(0.38f, 0.38f, 0.38f));
	//models[3]->setRotation(glm::vec3(0.0f, 0.f, 0.0f));*/

	//models[2]->setOrigin(glm::vec3(2.0f, -0.45f, 1.5f));
	//models[2]->setRotation(glm::vec3(0.0f, 0.f, 0.0f));
}

int main()
{
    //return 0;
	initWindow();

	initShaders();
	initTextures();
	initMaterials();
	initPointLights();
	initModels();
	
	
	camera 		= new Camera(glm::vec3(0.f, 0.f, 15.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), &ViewMatrix);
	fillGBuffer = new FillGBuffer(models);
	fillSSAO    = new FillSSAO(ProjectionMatrix, &ViewMatrix);
	screenQuad  = new ScreenQuad(models);

	int maxAttachments;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttachments);

	cout << maxAttachments << endl;

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
		
        updateDt();
        updateInput();

		glClearColor(0.0,0.0,0.0,1.0);                                 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		fillGBuffer -> RenderToGBuffer();
		fillSSAO    -> GenerateSSAO(fillGBuffer);
		fillSSAO    -> BlurSSAO();



		fillGBuffer -> BindAllGBufferTextures(screenQuad->getScreenQuadShader(), fillSSAO->getSSAOTexture());
		
		screenQuad  -> PassShaderUniforms(camera, pointLights, ViewMatrix);
		screenQuad  -> renderScreenQuad();

		glfwSwapBuffers(window);
		glBindVertexArray(0);
		glUseProgram(0);
		
		glBindTexture(GL_TEXTURE_2D, 0);

    }
    
	cleanUp();

	return 0;
}




