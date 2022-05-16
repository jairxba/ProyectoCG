/*
Semestre 2021-1
Animación por keyframes
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
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
//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"
#include"SpotLight.h"

FILE *guardarFrames;
const float toRadians = 3.14159265f / 180.0f;
//Variables para animación
float movCoche;
float movOffset;
float rotapato;
float rotLuna;
float rotLunaOffset;
int contador2, contador2aux; //Para controlar la luz del dia y noche
bool avanza,patoadelante,activaCamara; //Banderas de acción
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

//Se declaran las dos cámaras a utilizar
Camera camera;
Camera cameraIsometrica;

float reproduciranimacion,habilitaranimacion, guardoFrame,reinicioFrame,ciclo,ciclo2,contador=0;
Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture dadoTexture;
Texture pisoTexture;
Texture pastoTexture;
Texture Tagave;

//Materiales
Material Material_brillante;
Material Material_opaco;

//Luz direccional
DirectionalLight mainLight;
DirectionalLight mainNight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

//------Modelos 3D-------
//Model Kitt_M;
//Model Llanta_M;
//Model Camino_M;
Model Blackhawk_M;
Model Helice_M;
Model Personaje_M;
Model Carrusel_M;
Model LunaCarrusel_M;
Model Pared_M;
Model Banca_M;
Model Farol_M;
Model Jaula_M;
Model Calle_M;
Model CalleEntrada_M;
Model Arbol1_M;
Model Helipuerto_M;

Skybox skybox;
Skybox skyboxDia;
Skybox skyboxNoche;

Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

//void my_input(GLFWwindow *window);
void inputKeyframes(bool* keys);

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";
//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount, 
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

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.27f,  0.35f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.48f,	0.35f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.48f,	0.64f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.27f,	0.64f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.52f,  0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.73f,	0.64f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.52f,	0.64f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.77f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.98f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.98f,	0.64f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.77f,	0.64f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.23f,  0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.23f,	0.64f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.64f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.27f,	0.02f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.48f,  0.02f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.48f,	0.31f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.27f,	0.31f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.27f,	0.68f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.48f,  0.68f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.48f,	0.98f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.27f,	0.98f,		0.0f,	-1.0f,	0.0f,

	};
	
	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


///////////////////////////////KEYFRAMES/////////////////////
bool animacion = false;



//NEW// Keyframes
float posXavion = -25.0, posYavion = -0.8, posZavion = 0;
float	movAvion_x = 0.0f, movAvion_y = 0.0f, movAvion_z = 0.0f;
float giroAvion = 0, giroHelice = 0.0f;

#define MAX_FRAMES 50
int i_max_steps = 270;
int i_curr_steps = 30;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_z;		//Variable para PosicionZ
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float movAvion_zInc;		//Variable para IncrementoZ
	float giroAvion;
	float giroHelice;
	float giroAvionInc;
	float giroHeliceInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 46;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void){

	printf("frameindex %d\n", FrameIndex);
	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	printf("KeyFrame[FrameIndex].X = Valor; %d\n", KeyFrame[FrameIndex].movAvion_x);
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	printf("KeyFrame[FrameIndex].Y = Valor; %d\n", KeyFrame[FrameIndex].movAvion_y);
	KeyFrame[FrameIndex].movAvion_z = movAvion_z;
	printf("KeyFrame[FrameIndex].Z = Valor; %d\n", KeyFrame[FrameIndex].movAvion_z);
	KeyFrame[FrameIndex].giroAvion = giroAvion;
	printf("KeyFrame[FrameIndex].giroAvion = Valor; %d\n", KeyFrame[FrameIndex].giroAvion);
	KeyFrame[FrameIndex].giroHelice = giroHelice;
	FrameIndex++;
}

void resetElements(void){
	
	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	movAvion_y = KeyFrame[0].movAvion_z;
	giroAvion = KeyFrame[0].giroAvion;
	giroHelice = KeyFrame[0].giroHelice;
}

void interpolation(void){
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].movAvion_zInc = (KeyFrame[playIndex + 1].movAvion_z - KeyFrame[playIndex].movAvion_z) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;
	KeyFrame[playIndex].giroHeliceInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroHelice) / i_max_steps;
}

void animate(void)
{
	guardarFrames = fopen("frames.txt", "w");
	//Movimiento del objeto
	if (play)
	{	
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			printf("Frame [%d] reproducido \n", playIndex-1);
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("Ultimo frame es [%d] \n", FrameIndex-1);
				//fprintf(guardarFrames,"FrameIndex [%d]" ,FrameIndex);
				printf("Termina animación\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				//printf("entro aquí\n");
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//printf("se quedó aqui\n");
			//printf("max steps: %f", i_max_steps);
			//Draw animation
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			movAvion_z += KeyFrame[playIndex].movAvion_zInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			giroHelice += KeyFrame[playIndex].giroHeliceInc;
			i_curr_steps++;
		}

	}
}

/* FIN KEYFRAMES*/



int main() 
{
	mainWindow = Window(1024, 640); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCubo();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 10.0f, 0.5f);
	cameraIsometrica = Camera(glm::vec3(25.0f, 30.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -120.0f, -30.0f, 25.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado.tga");
	dadoTexture.LoadTextureA();
	pisoTexture= Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	pastoTexture = Texture("Textures/pasto.png");
	pastoTexture.LoadTextureA();
	Tagave = Texture("Textures/Agave.tga");
	Tagave.LoadTextureA();
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//Kitt_M = Model();
	//Kitt_M.LoadModel("Models/kitt.obj");
	//Llanta_M = Model();
	//Llanta_M.LoadModel("Models/k_rueda.3ds");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Helice_M = Model();
	Helice_M.LoadModel("Models/helice.obj");
	//Camino_M = Model();
	//Camino_M.LoadModel("Models/railroad track.obj");
	
	Personaje_M = Model();
	Personaje_M.LoadModel("Models/personaje.obj");
	Calle_M = Model();
	Calle_M.LoadModel("Models/calle.obj");
	CalleEntrada_M = Model();
	CalleEntrada_M.LoadModel("Models/calle_entrada.obj");
	Pared_M = Model();
	Pared_M.LoadModel("Models/pared.obj");
	Carrusel_M = Model();
	Carrusel_M.LoadModel("Models/carrusel_modificado.obj");
	LunaCarrusel_M = Model();
	LunaCarrusel_M.LoadModel("Models/luna_modificada.obj");
	Banca_M = Model();
	Banca_M.LoadModel("Models/banca.obj");
	Jaula_M = Model();
	Jaula_M.LoadModel("Models/jaula.obj");
	Farol_M = Model();
	Farol_M.LoadModel("Models/farol.obj");
	Arbol1_M = Model();
	Arbol1_M.LoadModel("Models/arbol1.obj");
	Helipuerto_M = Model();
	Helipuerto_M.LoadModel("Models/helipuerto.obj");

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, //Luz del dia
								0.3f, 0.3f,
								0.0f, 0.0f, -1.0f);

	mainNight = DirectionalLight(0.1f, 0.1f, 0.1f, //Luz de la noche
		0.3f, 0.3f,
		0.0f, -1.0f, 0.0f);

//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.1f, 0.0f,
								0.0f, 1.0f,
								15.0f, 8.0f,1.5f,
								0.1f, 0.8f, 0.1f);
	pointLightCount++;
	

	
	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 2.0f,
		10.0f, 0.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;
	//luz de faro
	spotLights[2] = SpotLight(0.0f, 1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, -1.5f, 0.0f,
		-4.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;
	//luz de helicoptero
	spotLights[3] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 1.0f,
		2.0 - movCoche, 2.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;
	glm::vec3 posblackhawk = glm::vec3(-25.0f, -0.8f, 0.0f);

	//En esta parte se cargan las texturas para el Skybox de dia y noche
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	std::vector<std::string> skyboxFacesDia;
	skyboxFacesDia.push_back("Textures/Skybox/dia1_rt.tga");
	skyboxFacesDia.push_back("Textures/Skybox/dia1_lf.tga");
	skyboxFacesDia.push_back("Textures/Skybox/dia1_dn.tga");
	skyboxFacesDia.push_back("Textures/Skybox/dia1_up.tga");
	skyboxFacesDia.push_back("Textures/Skybox/dia1_bk.tga");
	skyboxFacesDia.push_back("Textures/Skybox/dia1_ft.tga");

	skybox = Skybox(skyboxFaces);
	skyboxDia = Skybox(skyboxFacesDia);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);
	
	contador2 = 0;
	contador2aux = 1;
	movCoche = 0.0f;
	movOffset = 1.0f;
	rotLuna = 0.0f;
	rotLunaOffset = 10.0f;
	avanza = 1;
	activaCamara = 1;
	
	//KEYFRAMES DECLARADOS INICIALES
	
	KeyFrame[0].movAvion_x = 0.0f;
	KeyFrame[0].movAvion_y = 0.0f;
	KeyFrame[0].movAvion_z = 0.0f;
	KeyFrame[0].giroAvion = 0; 
	KeyFrame[0].giroHelice = 0;

	KeyFrame[1].movAvion_x = 0.0f;
	KeyFrame[1].movAvion_y = 10.0f;
	KeyFrame[1].movAvion_z = 0.0f;
	KeyFrame[1].giroAvion = 0;
	KeyFrame[1].giroHelice = 10;

	KeyFrame[2].movAvion_x = 5.0f;
	KeyFrame[2].movAvion_y = 6.0f;
	KeyFrame[2].movAvion_z = 0.0f;
	KeyFrame[2].giroAvion = 0;
	KeyFrame[2].giroHelice = 20;

	KeyFrame[3].movAvion_x = 7.0f;
	KeyFrame[3].movAvion_y = 10.0f;
	KeyFrame[3].movAvion_z = 0.0f;
	KeyFrame[3].giroAvion = 0;
	KeyFrame[3].giroHelice = 20;

	KeyFrame[4].movAvion_x = 9.0f;
	KeyFrame[4].movAvion_y = 6.0f;
	KeyFrame[4].movAvion_z = 0.0f;
	KeyFrame[4].giroAvion = 0.0f;
	KeyFrame[4].giroHelice = 20;

	KeyFrame[5].movAvion_x = 11.0f;
	KeyFrame[5].movAvion_y = 10.0f;
	KeyFrame[5].movAvion_z = 0.0f;
	KeyFrame[5].giroAvion = 0;
	KeyFrame[5].giroHelice = 20;
	
	KeyFrame[6].movAvion_x = 13.0f;
	KeyFrame[6].movAvion_y = 6.0f;
	KeyFrame[6].movAvion_z = 0.0f;
	KeyFrame[6].giroAvion = 0;
	KeyFrame[6].giroHelice = 20;

	KeyFrame[7].movAvion_x = 15.0f;
	KeyFrame[7].movAvion_y = 10.0f;
	KeyFrame[7].movAvion_z = 0.0f;
	KeyFrame[7].giroAvion = 0;
	KeyFrame[7].giroHelice = 20;

	KeyFrame[8].movAvion_x = 17.0f;
	KeyFrame[8].movAvion_y = 6.0f;
	KeyFrame[8].movAvion_z = 0.0f;
	KeyFrame[8].giroAvion = 90;
	KeyFrame[8].giroHelice = 20;

	KeyFrame[9].movAvion_x = 19.0f;
	KeyFrame[9].movAvion_y = 10.0f;
	KeyFrame[9].movAvion_z = -4.0f;
	KeyFrame[9].giroAvion = 90;
	KeyFrame[9].giroHelice = 20;

	KeyFrame[10].movAvion_x = 19.0f;
	KeyFrame[10].movAvion_y = 6.0f;
	KeyFrame[10].movAvion_z = -8.0f;
	KeyFrame[10].giroAvion = 90;
	KeyFrame[10].giroHelice = 20;

	KeyFrame[11].movAvion_x = 19.0f;
	KeyFrame[11].movAvion_y = 10.0f;
	KeyFrame[11].movAvion_z = -12.0f;
	KeyFrame[11].giroAvion = 90;
	KeyFrame[11].giroHelice = 20;

	KeyFrame[12].movAvion_x = 19.0f;
	KeyFrame[12].movAvion_y = 6.0f;
	KeyFrame[12].movAvion_z = -16.0f;
	KeyFrame[12].giroAvion = 90;
	KeyFrame[12].giroHelice = 20;

	KeyFrame[13].movAvion_x = 19.0f;
	KeyFrame[13].movAvion_y = 10.0f;
	KeyFrame[13].movAvion_z = -20.0f;
	KeyFrame[13].giroAvion = 90;
	KeyFrame[13].giroHelice = 20;

	KeyFrame[14].movAvion_x = 19.0f;
	KeyFrame[14].movAvion_y = 6.0f;
	KeyFrame[14].movAvion_z = -24.0f;
	KeyFrame[14].giroAvion = 90;
	KeyFrame[14].giroHelice = 20;

	KeyFrame[15].movAvion_x = 19.0f;
	KeyFrame[15].movAvion_y = 10.0f;
	KeyFrame[15].movAvion_z = -28.0f;
	KeyFrame[15].giroAvion = 90;
	KeyFrame[15].giroHelice = 20;

	KeyFrame[16].movAvion_x = 19.0f;
	KeyFrame[16].movAvion_y = 6.0f;
	KeyFrame[16].movAvion_z = -32.0f;
	KeyFrame[16].giroAvion = 90;
	KeyFrame[16].giroHelice = 20;

	KeyFrame[17].movAvion_x = 19.0f;
	KeyFrame[17].movAvion_y = 10.0f;
	KeyFrame[17].movAvion_z = -36.0f;
	KeyFrame[17].giroAvion = 90;
	KeyFrame[17].giroHelice = 20;

	KeyFrame[18].movAvion_x = 19.0f;
	KeyFrame[18].movAvion_y = 6.0f;
	KeyFrame[18].movAvion_z = -40.0f;
	KeyFrame[18].giroAvion = 90;
	KeyFrame[18].giroHelice = 20;

	KeyFrame[19].movAvion_x = 19.0f;
	KeyFrame[19].movAvion_y = 5.0f;
	KeyFrame[19].movAvion_z = -44.0f;
	KeyFrame[19].giroAvion = 90;
	KeyFrame[19].giroHelice = 20;

	KeyFrame[20].movAvion_x = 19.0f;
	KeyFrame[20].movAvion_y = 4.0f;
	KeyFrame[20].movAvion_z = -48.0f;
	KeyFrame[20].giroAvion = 90;
	KeyFrame[20].giroHelice = 20;

	KeyFrame[21].movAvion_x = 19.0f;
	KeyFrame[21].movAvion_y = 3.0f;
	KeyFrame[21].movAvion_z = -52.0f;
	KeyFrame[21].giroAvion = 90;
	KeyFrame[21].giroHelice = 20;

	KeyFrame[22].movAvion_x = 19.0f;
	KeyFrame[22].movAvion_y = 2.0f;
	KeyFrame[22].movAvion_z = -56.0f;
	KeyFrame[22].giroAvion = 90;
	KeyFrame[22].giroHelice = 20;

	KeyFrame[23].movAvion_x = 19.0f;
	KeyFrame[23].movAvion_y = 1.0f;
	KeyFrame[23].movAvion_z = -60.0f;
	KeyFrame[23].giroAvion = 90;
	KeyFrame[23].giroHelice = 20;

	KeyFrame[24].movAvion_x = 19.0f;
	KeyFrame[24].movAvion_y = 0.0f;
	KeyFrame[24].movAvion_z = -64.0f;
	KeyFrame[24].giroAvion = 90;
	KeyFrame[24].giroHelice = 10;

	//Aqui llega al escenario y esta en el suelo
	KeyFrame[25].movAvion_x = 19.0f;
	KeyFrame[25].movAvion_y = -0.85f;
	KeyFrame[25].movAvion_z = -68.0f;
	KeyFrame[25].giroAvion = 90;
	KeyFrame[25].giroHelice = 10;
	//Aqui esta en el suelo y gira
	KeyFrame[26].movAvion_x = 19.0f;
	KeyFrame[26].movAvion_y = -0.85f;
	KeyFrame[26].movAvion_z = -68.0f;
	KeyFrame[26].giroAvion = 180;
	KeyFrame[26].giroHelice = 0;
	//Comienza a elevarse
	KeyFrame[27].movAvion_x = 15.0f;
	KeyFrame[27].movAvion_y = 1.0f;
	KeyFrame[27].movAvion_z = -68.0f;
	KeyFrame[27].giroAvion = 180;
	KeyFrame[27].giroHelice = 10;

	KeyFrame[28].movAvion_x = 11.0f;
	KeyFrame[28].movAvion_y = 2.f;
	KeyFrame[28].movAvion_z = -68.0f;
	KeyFrame[28].giroAvion = 180;
	KeyFrame[28].giroHelice = 20;

	KeyFrame[29].movAvion_x = 7.0f;
	KeyFrame[29].movAvion_y = 3.0f;
	KeyFrame[29].movAvion_z = -68.0f;
	KeyFrame[29].giroAvion = 180;
	KeyFrame[29].giroHelice = 20;

	KeyFrame[30].movAvion_x = 3.0f;
	KeyFrame[30].movAvion_y = 4.0f;
	KeyFrame[30].movAvion_z = -68.0f;
	KeyFrame[30].giroAvion = 180;
	KeyFrame[30].giroHelice = 20;

	KeyFrame[31].movAvion_x = -1.0f;
	KeyFrame[31].movAvion_y = 5.0f;
	KeyFrame[31].movAvion_z = -68.0f;
	KeyFrame[31].giroAvion = 180;
	KeyFrame[31].giroHelice = 20;

	KeyFrame[32].movAvion_x = -5.0f;
	KeyFrame[32].movAvion_y = 6.0f;
	KeyFrame[32].movAvion_z = -68.0f;
	KeyFrame[32].giroAvion = 180;
	KeyFrame[32].giroHelice = 20;
	//Gira de nuevo
	KeyFrame[33].movAvion_x = -5.0f;
	KeyFrame[33].movAvion_y = 6.0f;
	KeyFrame[33].movAvion_z = -68.0f;
	KeyFrame[33].giroAvion = 270;
	KeyFrame[33].giroHelice = 20;
	//Se desplaza para ir al helipuerto
	KeyFrame[34].movAvion_x = -5.0f;
	KeyFrame[34].movAvion_y = 10.0f;
	KeyFrame[34].movAvion_z = -60.0f;
	KeyFrame[34].giroAvion = 270;
	KeyFrame[34].giroHelice = 20;

	KeyFrame[35].movAvion_x = -5.0f;
	KeyFrame[35].movAvion_y = 6.0f;
	KeyFrame[35].movAvion_z = -52.0f;
	KeyFrame[35].giroAvion = 270;
	KeyFrame[35].giroHelice = 20;

	KeyFrame[36].movAvion_x = -5.0f;
	KeyFrame[36].movAvion_y = 10.0f;
	KeyFrame[36].movAvion_z = -44.0f;
	KeyFrame[36].giroAvion = 270;
	KeyFrame[36].giroHelice = 20;

	KeyFrame[37].movAvion_x = -5.0f;
	KeyFrame[37].movAvion_y = 6.0f;
	KeyFrame[37].movAvion_z = -36.0f;
	KeyFrame[37].giroAvion = 270;
	KeyFrame[37].giroHelice = 20;

	KeyFrame[38].movAvion_x = -5.0f;
	KeyFrame[38].movAvion_y = 10.0f;
	KeyFrame[38].movAvion_z = -28.0f;
	KeyFrame[38].giroAvion = 270;
	KeyFrame[38].giroHelice = 20;

	KeyFrame[39].movAvion_x = -5.0f;
	KeyFrame[39].movAvion_y = 6.0f;
	KeyFrame[39].movAvion_z = -20.0f;
	KeyFrame[39].giroAvion = 270;
	KeyFrame[39].giroHelice = 20;

	KeyFrame[40].movAvion_x = -5.0f;
	KeyFrame[40].movAvion_y = 10.0f;
	KeyFrame[40].movAvion_z = -12.0f;
	KeyFrame[40].giroAvion = 270;
	KeyFrame[40].giroHelice = 20;

	KeyFrame[41].movAvion_x = -5.0f;
	KeyFrame[41].movAvion_y = 6.0f;
	KeyFrame[41].movAvion_z = -4.0f;
	KeyFrame[41].giroAvion = 270;
	KeyFrame[41].giroHelice = 20;

	KeyFrame[42].movAvion_x = -5.0f;
	KeyFrame[42].movAvion_y = 5.0f;
	KeyFrame[42].movAvion_z = 0.0f;
	KeyFrame[42].giroAvion = 270;
	KeyFrame[42].giroHelice = 20;
	//Gira 
	KeyFrame[43].movAvion_x = -5.0f;
	KeyFrame[43].movAvion_y = 5.0f;
	KeyFrame[43].movAvion_z = 0.0f;
	KeyFrame[43].giroAvion = 360;
	KeyFrame[43].giroHelice = 10;
	//Desciende
	KeyFrame[44].movAvion_x = -5.0f;
	KeyFrame[44].movAvion_y = 0.0f;
	KeyFrame[44].movAvion_z = 0.0f;
	KeyFrame[44].giroAvion = 360;
	KeyFrame[44].giroHelice = 10;
	//Termina ciclo de vuelo
	KeyFrame[45].movAvion_x = 0.0f;
	KeyFrame[45].movAvion_y = 0.0f;
	KeyFrame[45].movAvion_z = 0.0f;
	KeyFrame[45].giroAvion = 360;
	KeyFrame[45].giroHelice = 0;
	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose()){
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime; 
		//deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		rotLuna += rotLunaOffset * deltaTime * 5;

		if (avanza){
			if (movCoche < 10.0f){
//				movCoche += movOffset*deltaTime;
				movCoche += movOffset;
				avanza = 1;
			}else{
				avanza = 0;
			}
		}
		else{
			if (movCoche > -10.0f){
				movCoche -= movOffset*deltaTime;
			}else{
				avanza = 1;
			}
		}
		
		
		//Recibir eventos del usuario
		glfwPollEvents();

		//Si se pulsa la tecla N, se activa camara isometrica, en caso contrario se pulse M, camara normal
		if (mainWindow.getactivaCamara()) {
			//activaCamara = 1;
			cameraIsometrica.keyControl(mainWindow.getsKeys(), deltaTime);
			//cameraIsometrica.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		else {
			//activaCamara = 0;
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		
		//para keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();
		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		//skyboxDia.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();
		
		//// luz ligada a la cámara de tipo flash
		//glm::vec3 lowerLight = camera.getCameraPosition();
		//lowerLight.y -= 0.3f;
		//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		contador2 += contador2aux;
		//printf("\nContador: %d \n", contador2);
			if (contador2 < 8000) {
				shaderList[0].SetDirectionalLight(&mainLight);
			}else if(contador2 > 8000) {
				shaderList[0].SetDirectionalLight(&mainNight);
			}
			else {
				contador2 = 0;
			}


		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		if (mainWindow.getactivaCamara()) {
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(cameraIsometrica.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, cameraIsometrica.getCameraPosition().x, cameraIsometrica.getCameraPosition().y, cameraIsometrica.getCameraPosition().z);
		}
		else {
			//activaCamara = 0;
			glm::vec3 posPerso = glm::vec3(0.0f, -2.0f, -4.0f);
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		}


		glm::mat4 model(1.0);
		glm::mat4 personaje(1.0);
		glm::mat4 carrusel(1.0);
		glm::mat4 lunaCarrusel(1.0);
		glm::mat4 lunaCarrusel_aux(1.0);
		glm::mat4 pared(1.0);
		glm::mat4 banca(1.0);
		glm::mat4 jaula(1.0);
		glm::mat4 farol(1.0);
		glm::mat4 arbol1(1.0);
		glm::mat4 calle(1.0);
		glm::mat4 calleEntrada(1.0);
		glm::mat4 helipuerto(1.0);
		glm::mat4 helice(1.0);
		glm::mat4 helice_aux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f,30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		dirtTexture.UseTexture();
		//plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Personaje
		personaje = glm::mat4(1.0);
		personaje = glm::translate(personaje, glm::vec3(0.0f, -2.0f, -4.0f));
		personaje = glm::scale(personaje, glm::vec3(1.0f, 1.0f, 1.0f));
		personaje = glm::rotate(personaje, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(personaje));
		Personaje_M.RenderModel();

		//CalleEntrada
		calleEntrada = glm::mat4(1.0);
		calleEntrada = glm::translate(calleEntrada, glm::vec3(1.25f, -1.9f, -21.25f));
		calleEntrada = glm::scale(calleEntrada, glm::vec3(0.61f, 0.3f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(calleEntrada));
		CalleEntrada_M.RenderModel();

		//Calle
		calle = glm::mat4(1.0);
		calle = glm::translate(calle, glm::vec3(4.4f, -1.9f, -16.0f));
		calle = glm::scale(calle, glm::vec3(0.3f, 0.3f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(calle));
		Calle_M.RenderModel();
		//Calle
		calle = glm::mat4(1.0);
		calle = glm::translate(calle, glm::vec3(-1.8f, -1.9f, -16.0f));
		calle = glm::scale(calle, glm::vec3(0.3f, 0.3f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(calle));
		Calle_M.RenderModel();

		//Pared que cubre todo el escenario
		pared = glm::mat4(1.0);
		pared = glm::scale(pared, glm::vec3(25.0f, 6.0f, 17.0f));
		pared = glm::rotate(pared, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		pared = glm::translate(pared, glm::vec3(-3.5f, -0.32f, 0.0f));//-0.24f para estar arriba del piso
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(pared));
		Pared_M.RenderModel();

		//Carrusel
		carrusel = glm::mat4(1.0);
		carrusel = glm::translate(carrusel, glm::vec3(-28.0f, -1.9f, -38.0f));
		lunaCarrusel_aux = carrusel;
		carrusel = glm::scale(carrusel, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(carrusel));
		Carrusel_M.RenderModel();

		//Luna Carrusel
		lunaCarrusel = glm::mat4(1.0);
		//lunaCarrusel = lunaCarrusel_aux;
		lunaCarrusel = glm::translate(carrusel, glm::vec3(-0.1f, 2.77f, 0.05f));
		lunaCarrusel = glm::scale(lunaCarrusel, glm::vec3(0.5f, 0.5f, 0.7f));
		lunaCarrusel = glm::rotate(lunaCarrusel, rotLuna * toRadians , glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(lunaCarrusel));
		LunaCarrusel_M.RenderModel();
		
		//Banca 1
		banca = glm::mat4(1.0);
		banca = glm::translate(banca, glm::vec3(-30.0f, -1.9f, -50.0f));
		//banca = glm::scale(banca, glm::vec3(0.1f, 0.1f, 0.1f));
		banca = glm::rotate(banca, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(banca));
		Banca_M.RenderModel();

		//Banca 2
		banca = glm::mat4(1.0);
		banca = glm::translate(banca, glm::vec3(-26.0f, -1.9f, -50.0f));
		banca = glm::rotate(banca, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(banca));
		Banca_M.RenderModel();

		//Banca 3
		banca = glm::mat4(1.0);
		banca = glm::translate(banca, glm::vec3(-22.0f, -1.9f, -50.0f));
		banca = glm::rotate(banca, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(banca));
		Banca_M.RenderModel();

		//Arbol tipo 1
		arbol1 = glm::mat4(1.0);
		arbol1 = glm::translate(arbol1, glm::vec3(-30.0f, -1.9f, -49.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(arbol1));
		Arbol1_M.RenderModel();
		//Arbol tipo 1
		arbol1 = glm::mat4(1.0);
		arbol1 = glm::translate(arbol1, glm::vec3(-22.0f, -1.9f, -49.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(arbol1));
		Arbol1_M.RenderModel();
		//Arbol tipo 1
		arbol1 = glm::mat4(1.0);
		arbol1 = glm::translate(arbol1, glm::vec3(-30.0f, -1.9f, -59.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(arbol1));
		Arbol1_M.RenderModel();
		//Arbol tipo 1
		arbol1 = glm::mat4(1.0);
		arbol1 = glm::translate(arbol1, glm::vec3(-22.0f, -1.9f, -59.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(arbol1));
		Arbol1_M.RenderModel();

		//Farol 3
		farol = glm::mat4(1.0);
		farol = glm::translate(farol, glm::vec3(-31.0f, -2.0f, -47.0f));
		farol = glm::scale(farol, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(farol));
		Farol_M.RenderModel();
		//Farol 4
		farol = glm::mat4(1.0);
		farol = glm::translate(farol, glm::vec3(-10.0f, -2.0f, -47.0f));
		farol = glm::scale(farol, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(farol));
		Farol_M.RenderModel();
		//Farol 5
		farol = glm::mat4(1.0);
		farol = glm::translate(farol, glm::vec3(-31.0f, -2.0f, -65.0f));
		farol = glm::scale(farol, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(farol));
		Farol_M.RenderModel();
		//Farol 6
		farol = glm::mat4(1.0);
		farol = glm::translate(farol, glm::vec3(-10.0f, -2.0f, -65.0f));
		farol = glm::scale(farol, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(farol));
		Farol_M.RenderModel();
		//Farol 7
		farol = glm::mat4(1.0);
		farol = glm::translate(farol, glm::vec3(37.0f, -2.0f, -55.0f));
		farol = glm::scale(farol, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(farol));
		Farol_M.RenderModel();
		//Farol 8 Entrada Izquierda
		farol = glm::mat4(1.0);
		farol = glm::translate(farol, glm::vec3(-7.0f, -2.0f, -36.0f));
		farol = glm::scale(farol, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(farol));
		Farol_M.RenderModel();
		//Farol 9 Entrada Derecha
		farol = glm::mat4(1.0);
		farol = glm::translate(farol, glm::vec3(10.0f, -2.0f, -36.0f));
		farol = glm::scale(farol, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(farol));
		Farol_M.RenderModel();


		//Banca 4
		banca = glm::mat4(1.0);
		banca = glm::translate(banca, glm::vec3(-30.0f, -1.9f, -55.0f));
		banca = glm::rotate(banca, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(banca));
		Banca_M.RenderModel();

		//Banca 5
		banca = glm::mat4(1.0);
		banca = glm::translate(banca, glm::vec3(-26.0f, -1.9f, -55.0f));
		banca = glm::rotate(banca, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(banca));
		Banca_M.RenderModel();

		//Banca 6
		banca = glm::mat4(1.0);
		banca = glm::translate(banca, glm::vec3(-22.0f, -1.9f, -55.0f));
		banca = glm::rotate(banca, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(banca));
		Banca_M.RenderModel();

		//Jaula 1
		jaula = glm::mat4(1.0);
		jaula = glm::translate(jaula, glm::vec3(-29.0f, -2.0f, -75.0f));
		jaula = glm::scale(jaula, glm::vec3(4.0f, 4.0f, 4.0f));
		jaula = glm::rotate(jaula, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(jaula));
		Jaula_M.RenderModel();

		//Farol 1
		farol = glm::mat4(1.0);
		farol = glm::translate(farol, glm::vec3(-22.0f, -2.0f, -78.0f));
		farol = glm::scale(farol, glm::vec3(2.0f, 2.0f, 2.0f));
		//farol = glm::rotate(farol, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(farol));
		Farol_M.RenderModel();

		//Jaula 2
		jaula = glm::mat4(1.0);
		jaula = glm::translate(jaula, glm::vec3(-17.0f, -2.0f, -75.0f));
		jaula = glm::scale(jaula, glm::vec3(4.0f, 4.0f, 4.0f));
		jaula = glm::rotate(jaula, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(jaula));
		Jaula_M.RenderModel();

		//Farol 2 Se encuentra al lado del esqueleto
		farol = glm::mat4(1.0);
		farol = glm::translate(farol, glm::vec3(10.0f, -2.0f, -78.0f));
		farol = glm::scale(farol, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(farol));
		Farol_M.RenderModel();

		//Jaula 3 Se encuentra al lado de volcan
		jaula = glm::mat4(1.0);
		jaula = glm::translate(jaula, glm::vec3(15.0f, -2.0f, -75.0f));
		jaula = glm::scale(jaula, glm::vec3(4.0f, 4.0f, 4.0f));
		jaula = glm::rotate(jaula, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(jaula));
		Jaula_M.RenderModel();

		//Jaula 4
		jaula = glm::mat4(1.0);
		jaula = glm::translate(jaula, glm::vec3(15.0f, -2.0f, -60.0f));
		jaula = glm::scale(jaula, glm::vec3(4.0f, 4.0f, 4.0f));
		jaula = glm::rotate(jaula, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(jaula));
		Jaula_M.RenderModel();

		//Jaula 5
		jaula = glm::mat4(1.0);
		jaula = glm::translate(jaula, glm::vec3(32.0f, -2.0f, -60.0f));
		jaula = glm::scale(jaula, glm::vec3(4.0f, 4.0f, 4.0f));
		jaula = glm::rotate(jaula, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(jaula));
		Jaula_M.RenderModel();


		
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(movCoche, -1.4f, 0.5f));
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		////model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		////model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//Kitt_M.RenderModel();
		//spotLights[2].SetPos(glm::vec3(movCoche, -1.5f, 0.0f));
		


		model = glm::mat4(1.0);
		posblackhawk = glm::vec3(posXavion+movAvion_x,posYavion+movAvion_y, posZavion+movAvion_z);
		model = glm::translate(model, posblackhawk);
		helice_aux = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::rotate(model, -90* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, giroAvion * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Blackhawk_M.RenderModel();
		spotLights[3].SetPos(posblackhawk);

		helice = glm::mat4(1.0);
		helice = helice_aux;
		//helice = glm::translate(helice, glm::vec3(0.0f, -1.6f, -4.2f));
		//helice = glm::translate(helice, glm::vec3(25.0f, -0.8f, -4.2f));
		helice = glm::translate(helice, glm::vec3(0.05f, 0.3f, 0.0f));
		//helice = glm::translate(helice, posblackhawk);
		helice = glm::scale(helice, glm::vec3(0.2f, 0.2f, 0.2f));
		helice = glm::rotate(helice, giroHelice * 2 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(helice));
		Helice_M.RenderModel();
		
		helipuerto = glm::mat4(1.0);
		helipuerto = glm::translate(helipuerto, glm::vec3(-25.0f,-2.0f,0.0f));

		helipuerto = glm::scale(helipuerto, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(helipuerto));
		Helipuerto_M.RenderModel();


		



		////Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, -1.7f, -2.0f));
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		////blending: transparencia o traslucidez
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//Tagave.UseTexture();
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[3]->RenderMesh();
		//glDisable(GL_BLEND);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}//Cierra ciclo while

	return 0;
}

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if(reproduciranimacion<1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("Presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;
			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
		}
	}
	
	if (keys[GLFW_KEY_L]){
		if (guardoFrame < 1){
			saveFrame();
			printf("movAvion_x es: %f\n", movAvion_x);
			//printf("movAvion_y es: %f\n", movAvion_y);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P]){
		if (reinicioFrame < 1){
			guardoFrame=0;
		}
	}
	
	//Implementación de teclado para la variable de traslación en X positivo
	if (keys[GLFW_KEY_1]){
		if (ciclo < 1){
			//printf("movAvion_x(+) es: %f\n", movAvion_x);
			movAvion_x += 1.0f;
			printf("KeyFrame[FrameIndex].X = %d; %d\n", KeyFrame[FrameIndex].movAvion_x, FrameIndex);
			printf("\nmovAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}
		
	}
	//Implementación de teclado para la variable de traslación en X negativo
	if (keys[GLFW_KEY_3]) {
		if (ciclo < 1) {
			//printf("movAvion_x(-) es: %f\n", movAvion_x);
			movAvion_x -= 1.0f;
			printf("\nmovAvion_x(-) es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}
	}

	//Implementación de teclado para la variable de traslación en Y positivo
	if (keys[GLFW_KEY_4]) {
		if (ciclo < 1) {
			//printf("movAvion_y(+) es: %f\n", movAvion_y);
			movAvion_y += 1.0f;
			printf("\nmovAvion_y(+) es: %f\n", movAvion_y);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}

	}
	//Implementación de teclado para la variable de traslación en Y negativo
	if (keys[GLFW_KEY_5]) {
		if (ciclo < 1) {
			//printf("movAvion_y(-) es: %f\n", movAvion_y);
			movAvion_y -= 1.0f;
			printf("\nmovAvion_y(-) es: %f\n", movAvion_y);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}
	}

	//Implementación de teclado para la variable GIRO DEL AVIÓN
	if (keys[GLFW_KEY_6]) {
		if (ciclo < 1) {
			//printf("movAvion_y(-) es: %f\n", movAvion_y);
			giroAvion += 10.0f;
			printf("\ngiroAvion(+) es: %f\n", giroAvion);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}
	}

	//Implementación de teclado para la variable GIRO DEL AVIÓN
	if (keys[GLFW_KEY_7]) {
		if (ciclo < 1) {
			//printf("movAvion_y(-) es: %f\n", movAvion_y);
			giroAvion -= 10.0f;
			printf("\ngiroAvion(-) es: %f\n", giroAvion);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}
	}

	if (keys[GLFW_KEY_2]){
		if(ciclo2<1)
		{
		ciclo = 0;
		}
	}



}
