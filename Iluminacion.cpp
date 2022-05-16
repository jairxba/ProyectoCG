/*
Semestre 2022-2
Práctica 8: Iluminación 1 
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

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;
int contador = 0;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dado8Texture;

Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;
Model Carro_M;
Model Solar_M;
Model Arbol_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

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

	//En create objects: 
	unsigned int vegetacionIndices[] = {
			0, 1, 2,
			0, 2, 3,
			4,5,6,
			4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);


}

void CrearDado8()
{
	unsigned int cubo_indices[] = {
		0,1,2, //Cara 1
		3,4,5, //Cara 2
		6,7,8, //Cara 3
		9,10,11, //Cara 4

		12,13,14, //Cara 5
		15,16,17, //Cara 6
		18,19,20, //Cara 7
		21,22,23, //Cara 8 

	};

	GLfloat cubo_vertices[] = {
		//x		y		z		S		T			NX		NY		NZ
		0.0f,  0.5f,   0.0f,  0.00f,    0.0f,		0.0f,	0.0f,	-1.0f,	//0
		0.0f,  0.0f,   0.5f,  0.5f,	    0.0f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.0f,   0.0f,  0.5f,	    0.5f,		0.0f,	0.0f,	-1.0f,	//2

		0.0f,  0.5f,   0.0f,  0.5f,    0.0f,		0.0f,	0.0f,	-1.0f,	//3
		0.0f,  0.0f,   0.5f,  1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//4
	   -0.5f,  0.0f,   0.0f,  1.0f,	0.5f,		0.0f,	0.0f,	-1.0f,	//5

		0.0f,  0.5f,   0.0f,  0.0f,    0.5f,	    -1.0f,	0.0f,	0.0f,	//6
		0.5f,  0.0f,   0.0f,  0.5f,	0.5f,		-1.0f,	0.0f,	0.0f,	//7
		0.0f,  0.0f,  -0.5f,  0.5f,	1.0f,		-1.0f,	0.0f,	0.0f,	//8

		0.0f,  0.5f,   0.0f,  0.5f,    0.5f,		1.0f,	0.0f,	0.0f,	//9
	   -0.5f,  0.0f,   0.0f,  1.0f,	0.5f,		1.0f,	0.0f,	0.0f,	//10
		0.0f,  0.0f,  -0.5f,  1.0f,	1.0f,		1.0f,	0.0f,	0.0f,	//11

		0.0f, -0.5f,   0.0f,  0.5f,    0.5f,		0.0f,	0.0f,	-1.0f,	//12
		0.0f,  0.0f,   0.5f,  1.0f,	0.5f,		0.0f,	0.0f,	-1.0f,	//13
		0.5f,  0.0f,   0.0f,  1.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//14

		0.0f, -0.5f,   0.0f,  0.0f,    0.5f,		0.0f,	0.0f,	-1.0f,	//15
		0.0f,  0.0f,   0.5f,  0.5f,	0.5f,		0.0f,	0.0f,	-1.0f,	//16
		-0.5f, 0.0f,   0.0f,  0.5f,	1.0f,		0.0f,	0.0f,	-1.0f,	//17

		0.0f, -0.5f,   0.0f,  0.5f,    0.0f,		-1.0f,	0.0f,	0.0f,	//18
		0.5f,  0.0f,   0.0f,  1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,	//19
		0.0f,  0.0f,  -0.5f,  1.0f,	0.5f,		-1.0f,	0.0f,	0.0f,	//20

		0.0f, -0.5f,   0.0f,  0.0f,    0.0f,		1.0f,	0.0f,	0.0f,	//21
		-0.5f, 0.0f,   0.0f,  0.5f,	0.0f,		1.0f,	0.0f,	0.0f,	//22
		0.0f,  0.0f,  -0.5f,  0.5f,	0.5f,		1.0f,	0.0f,	0.0f,	//23
	};
	Mesh* dado8 = new Mesh();
	dado8->CreateMesh(cubo_vertices, cubo_indices, 192, 24);
	meshList.push_back(dado8);

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
	CrearDado8();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	dado8Texture = Texture("Textures/dado8.tga");
	dado8Texture.LoadTexture();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/k_rueda.3ds");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/railroad track.obj");
	Carro_M = Model();
	Carro_M.LoadModel("Models/carroVox.obj");
	Solar_M = Model();
	Solar_M.LoadModel("Models/Solar lamp.obj");
	Arbol_M = Model();
	Arbol_M.LoadModel("Models/SakuraTree.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir Dia y noche
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f, //componenteAmbiental------componenteDifusa
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual Luz de la lampara
	pointLights[0] = PointLight(0.0f, 0.5f, 1.0f,
		0.0f, 1.0f,
		0.0f, 1.0f, 36.0f, //Vector de posición
	  // c     b     a 
		0.3f, 0.2f, 0.1f); //Ecuacion cuadrada a,c pequeñas 
	pointLightCount++;
	//Declaración de segunda luz puntual Luz del arbol
	pointLights[1] = PointLight(1.0f, 0.0f, 0.5f,
		0.0f, 1.0f,
		0.0f, 5.0f, 60.0f, //Vector de posición
	  // c     b     a 
		0.3f, 0.1f, 0.4f); //Ecuacion cuadrada a,c pequeñas 
	pointLightCount++;

	//Declaración de tercera luz puntual Luz 1 del dado
	pointLights[2] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-9.0f, 1.0f, -10.0f, //Vector de posición
	  // c     b     a 
		0.3f, 0.1f, 0.4f); //Ecuacion cuadrada a,c pequeñas 
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f, //Vector de posición
		0.0f, -1.0f, 0.0f, //Vector de dirección
		1.0f, 0.0f, 0.0f, //Normal
		5.0f); //Radio de la linterna
	spotLightCount++;

	////luz de helicóptero
	//spotLights[1] = SpotLight(0.0f, 1.0f, 1.0f,
	//	1.0f, 2.0f,
	//	-5.0f, 3.0f, -1.0f,
	//	0.0f, -1.0f, 0.0f,
	//	1.0f, 0.0f, 0.0f,
	//	20.0f);
	//spotLightCount++;

	////luz del faro1 del carro
	//spotLights[2] = SpotLight(1.0f, 0.0f, 0.0f,
	//	1.0f, 2.0f,
	//	60.0f, 2.0f, 0.0f,
	//	1.0f, 0.0f, 0.0f,
	//	1.0f, 0.0f, 0.0f,
	//	20.0f);
	//spotLightCount++;

	////luz del faro2 del carro
	//spotLights[3] = SpotLight(1.0f, 0.0f, 1.0f,
	//	1.0f, 2.0f,
	//	60.0f, 2.0f, 0.0f,
	//	1.0f, 0.0f, 0.0f,
	//	1.0f, 0.0f, 0.0f,
	//	20.0f);
	//spotLightCount++;

	spotLights[1] = SpotLight(0.0f, 0.0f, 0.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f, //Vector de posición
		0.0f, -1.0f, 0.0f, //Vector de dirección
		1.0f, 0.0f, 0.0f, //Normal
		0.0f); //Radio de la linterna
	spotLightCount++;

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
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
			glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection()); //Posicion | Direccion

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		
		if (mainWindow.getcontador()==0) {
			
			printf("apagar \n");
			//contador
			spotLightCount = 1;
		}
		else {
			printf("Contador: %i \n", contador);
			contador += 1;
		}



		glm::mat4 model(1.0);
		glm::mat4 model2(1.0);
		glm::mat4 carroV(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 dado(1.0);
		glm::mat4 solar(1.0);
		glm::mat4 arbol(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -1.5f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();*/

		//Helicoptero  Se mueve con las teclas Z,X,C,V
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 5.0f, 2.0f));
		model2 = model;
		model = glm::translate(model, glm::vec3(mainWindow.getmuevex(), mainWindow.getmuevey(), -1.0));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		// luz ligada al helicoptero
		spotLights[1].SetFlash((glm::vec3(-5.0f, 5.0f, -1.0f)), glm::vec3(mainWindow.getmuevex(), mainWindow.getmuevey(), -1.0)); //Posicion | Direccion
		Blackhawk_M.RenderModel();

		//Carro Modelo VoxelArt Se mueve con las teclas Y,U,I,O
		carroV = glm::mat4(1.0);
		carroV = glm::translate(carroV, glm::vec3(60.0f, 2.0f, 0.0f));
		carroV = glm::translate(carroV, glm::vec3(mainWindow.getmuevex0(), 2.0f, mainWindow.getmuevez()));
		carroV = glm::rotate(carroV, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(carroV));
		// luz ligada al faro1
		spotLights[2].SetFlash((glm::vec3(60.0f, 2.0f, 0.0f)), glm::vec3(mainWindow.getmuevex0(), 2.0, -1.0));
		// luz ligada al faro2
		spotLights[3].SetFlash((glm::vec3(60.0f, 2.0f, 0.0f)), glm::vec3(mainWindow.getmuevex0(), 2.0, -1.0));
		Carro_M.RenderModel();

		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.53f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Camino_M.RenderModel();*/

		//Lampara
		solar = glm::mat4(1.0);
		solar = glm::translate(solar, glm::vec3(0.0f, -2.0f, 40.0f));
		solar = glm::scale(solar, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(solar));
		Solar_M.RenderModel();
		//Arbol
		arbol = glm::mat4(1.0);
		arbol = glm::translate(arbol, glm::vec3(0.0f, 5.0f, 60.0f));
		arbol = glm::scale(arbol, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(arbol));
		Arbol_M.RenderModel();

		//Crear dado 8 caras
		dado = glm::mat4(1.0);
		dado = glm::translate(dado, glm::vec3(-10.0f, 4.0f, -10.0f));
		dado = glm::scale(dado, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(dado));
		//dadoTexture.UseTexture();
		dado8Texture.UseTexture();
		meshList[4]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
