/*
Semestre 2022-2
Práctica 5: Carga de Modelos
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture carro1,carro2;

Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;
Model Lego_M;
Model carroRZR;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
//GLfloat deltaTime = 0.0f;
GLfloat deltaTime = 0.1f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";





void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	//carro1 = Texture("Textures/bourak.jpg");
	//carro2 = Texture("Textures/bourak_i.jpg");
	//carro1.LoadTexture();
	//carro2.LoadTexture();
	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llantaOp.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/railroad track.obj");
	Lego_M = Model();
	Lego_M.LoadModel("Models/lego1.obj");
	carroRZR = Model();
	carroRZR.LoadModel("Models/carroRZR.obj");
	

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0);
		glm::mat4 llanta(1.0);
		glm::mat4 carro(1.0);
		glm::mat4 lego(1.0);
		glm::mat4 pista(1.0);
		glm::mat4 helicoptero(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 llantaaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();

		//Modelo del carro
		model = glm::mat4(1.0);
		glm::vec3 color6 = glm::vec3(0.0f, 0.0f, 0.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -1.5f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color6));
		Kitt_M.RenderModel();

		//Llanta1 delantera izquierda
		llanta = glm::mat4(1.0);
		glm::vec3 color1 = glm::vec3(0.0f, 1.0f, 0.0f);
		llanta = glm::translate(llanta, glm::vec3(7.0f, -1.8f, 6.0f));
		llanta = glm::rotate(llanta, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(llanta));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color1));
		Llanta_M.RenderModel();

		//Llanta2 trasera izquierda
		llanta = glm::mat4(1.0);
		glm::vec3 color2 = glm::vec3(0.0f, 0.0f, 1.0f);
		llanta = glm::translate(llanta, glm::vec3(15.7f, -1.8f, 6.0f));
		llanta = glm::rotate(llanta, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(llanta));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color2));
		Llanta_M.RenderModel();

		//Llanta3 delantera derecha
		llanta = glm::mat4(1.0);
		glm::vec3 color3 = glm::vec3(0.5f, 1.0f, 1.0f);
		llanta = glm::translate(llanta, glm::vec3(7.0f, -1.8f, 0.5f));
		llanta = glm::rotate(llanta, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(llanta));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color3));
		Llanta_M.RenderModel();

		//Llanta4 trasera derecha
		llanta = glm::mat4(1.0);
		glm::vec3 color4 = glm::vec3(1.0f, 0.5f, 1.0f);
		llanta = glm::translate(llanta, glm::vec3(15.7f, -1.8f, 0.5f));
		llanta = glm::rotate(llanta, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(llanta));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color4));
		Llanta_M.RenderModel();


		//Helicoptero
		helicoptero = glm::mat4(1.0);
		helicoptero = glm::translate(helicoptero, glm::vec3(0.0f, 3.0f, -15.0));
		helicoptero = glm::scale(helicoptero, glm::vec3(0.3f, 0.3f, 0.3f));
		helicoptero = glm::rotate(helicoptero, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(helicoptero));
		Blackhawk_M.RenderModel();

		////Lego
		//lego = glm::mat4(1.0);
		//lego = glm::translate(lego, glm::vec3(-15.0f, -1.53f, -15.0f));
		//lego = glm::translate(lego, glm::vec3(mainWindow.getmoverXpos(), 0.0f, mainWindow.getmoverZpos()));
		//lego = glm::translate(lego, glm::vec3(mainWindow.getmoverXneg(), 0.0f, mainWindow.getmoverZneg()));
		//lego = glm::scale(lego, glm::vec3(0.2f, 0.2f, 0.2f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(lego));
		//Lego_M.RenderModel();

		//Tramo de pista, escalada en eje X
		pista = glm::mat4(1.0);
		glm::vec3 color5 = glm::vec3(0.1f, 0.4f, 0.5f);
		pista = glm::translate(pista, glm::vec3(-10.0f, -1.53f, 3.0f));
		pista = glm::scale(pista, glm::vec3(20.0f, 0.3f, 1.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(pista));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color5));
		Camino_M.RenderModel();

		//CarroImportado  Con Z,X mueve en eje X.  Con C,V mueve en eje Z
		carro = glm::mat4(1.0);
		carro = glm::translate(carro, glm::vec3(0.0f, 2.0f, 3.0f));
		carro = glm::translate(carro, glm::vec3(mainWindow.getmoverXpos(), 0.0f, mainWindow.getmoverZpos()));
		carro = glm::translate(carro, glm::vec3(mainWindow.getmoverXneg(), 0.0f, mainWindow.getmoverZneg()));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(carro));
		carroRZR.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
