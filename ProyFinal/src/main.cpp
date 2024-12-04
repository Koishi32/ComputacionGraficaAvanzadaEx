#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

// contains new std::shuffle definition
#include <algorithm>
#include <random>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"

// Font rendering include
#include "Headers/FontTypeRendering.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"

// ShadowBox include
#include "Headers/ShadowBox.h"

// OpenAL include
#include <AL/alut.h>

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

GLFWwindow *window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;
// Shader para dibujar un objeto con solo textura
Shader shaderTexture;
//Shader para dibujar el buffer de profunidad
Shader shaderDepth;
// Shader para visualizar el buffer de profundidad
Shader shaderViewDepth;
//Shader para las particulas de fountain
Shader textureParticlesLazer;

std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromTarget = 7.0;

Sphere skyboxSphere(20, 20);
Box boxCesped;
Box boxWalls;
Box boxHighway;
Box boxLandingPad;
Sphere esfera1(10, 10);
Box boxCollider;
Sphere sphereCollider(10, 10);
Cylinder rayModel(10, 10, 1.0, 1.0, 1.0);
Box boxIntro;
Box boxViewDepth;
// Models complex instances
Model modelRock;
Model modelAircraft;
Model modelAircraftVehicule;
Model modelLampPlant1;
Model modelLampPlant2;
// Modelos animados
Model CarlModelAnimate; 
Model HunterModelAnimate; 
Model ProtagonistModelAnimate; 

// Terrain model instance
Terrain terrain(-1,-1, 200,1, "../Textures/MyTextures/MapaMarsHeightMap2.png");

ShadowBox * shadowBox;

GLuint textureCraterMartsID, textureWallID, textureWindowID, textureHighwayID, textureLandingPadID;
GLuint textureTerrainRID, textureTerrainGID, textureTerrainBID, textureTerrainBlendMapID;
GLuint skyboxTextureID;
GLuint textureInit1ID, textureInit2ID, textureActivaID, textureScreenID;
GLuint textureParticlesLazerID;

bool iniciaPartida = false, presionarOpcion = false;

// Modelo para el render del texto
FontTypeRendering::FontTypeRendering *modelText;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/mp_bloodvalley/blood-valley_ft.tga",
		"../Textures/mp_bloodvalley/blood-valley_bk.tga",
		"../Textures/mp_bloodvalley/blood-valley_up.tga",
		"../Textures/mp_bloodvalley/blood-valley_dn.tga",
		"../Textures/mp_bloodvalley/blood-valley_rt.tga",
		"../Textures/mp_bloodvalley/blood-valley_lf.tga" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
glm::mat4 matrixModelRock = glm::mat4(1.0);
glm::mat4 modelMatrixAircraft = glm::mat4(1.0);
glm::mat4 modelMatrixAircraftVehicule = glm::mat4(1.0f); 
glm::mat4 modelMatrixCarl = glm::mat4(1.0f); 
glm::mat4 modelMatrixHunter = glm::mat4(1.0f); 
glm::mat4 modelMatrixProtagonist = glm::mat4(1.0f);

//Lamps Plant  Positions contro how many and where 
std::vector<glm::vec3> LampPlantPostion ={ // multiple lights 
	glm::vec3(188,0,305),  // COORDANATES IN GIMP CONVERT!!!!!!!!!!!
	glm::vec3(240,0,360) 
}; 
//Lamp Plant Orientation 
std::vector<float> LampPlantOrientation = { 
	90,-90 
}; 
std::vector<glm::vec3> LampPlantPostion2 ={ //one light 
	glm::vec3(262,0,363), 
	glm::vec3(300,0,260) 
}; 
std::vector<float> LampPlant2Orientation={ 
	-90 , 90
}; 
int AditionalLights=0; 

// Blending model unsorted
std::map<std::string, glm::vec3> blendingUnsorted = { // transparentes ordern depues de solidos, se tiene que ordenar desde el mas lejano al mas cercano
		{"aircraft", glm::vec3(10.0, 0.0, -17.5)}, // Set coordinates from Gimp
		{"LazerSource",glm::vec3(0.0)} //Actualizar con modelo de pivote
};

double deltaTime;
double currTime, lastTime;
float velProtagonist=3.5f;
const int IndexAnimationIdle = 9; 
const int IndexAnimationWalk = 16; 
const int IndexAnimationShoot = 1; 
const int IndexAnimationRunBack=17; 
const int IndexAnimationMoveRight = 19; 
const int IndexAnimationMoveLeft = 18; 
int animationProtagonistIndex=9; 
// Variables Joystick 
int presentJoystick; 
GLFWgamepadstate state; 

// Jump and Shoot variables 
bool isJump = false;
float GRAVITY = 1.81;
double tmv = 0;
double startTimeJump = 0;

float rotNave=0.0f; 
float currentNaveHeight=0.0f,NaveStep=0.02f; 
const float LimitNaveHeight=4.0f; 
bool GoUpNave = true;

double animationDuration =0.6f; // Duration in seconds shooting animation
double animationStartTime = 0.0f;
bool animationRuningShootingRunning = false;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

// OpenAL Defines
#define NUM_BUFFERS 3
#define NUM_SOURCES 3
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Source 0
ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };
// Source 1
ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };

// Buffers
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
// Configs
ALsizei size, freq;
ALenum format;
ALvoid *data;
int ch;
ALboolean loop;
std::vector<bool> sourcesPlay = {true, true, true};

// Framesbuffers
GLuint depthMap, depthMapFBO;

//Definicion de variables para el sistema de particulas de Rayo
GLuint initVel,startTime;
GLuint VAOParticles;
GLuint nParticles = 200;
double currTimeParticlesAnimation, lastTimeParticlesAnimation;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void initParticleBuffers();
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void prepareScene();
void prepareDepthScene();
void renderScene();
void WaitTime();
void PointLightGeneratePlant1(glm::vec3 PositionInsidePlant); 
void  generatePointLight(glm::vec3 LampPlantPostion2,int i,glm::vec3 DiffuseColor,glm::vec3 ambient,glm::vec3 specular,float quadratic);
glm::vec3 TransformGIMPCoordenatesToOpenGLPixels(glm::vec3 coordenadas);

glm::vec3 TransformGIMPCoordenatesToOpenGLPixels(glm::vec3 coordenadas){
		glm::vec3 resultado = glm::vec3(0,0,0);
		resultado.x = (coordenadas.x - 256) / 2.56;
		resultado.y = 0;
		resultado.z = (coordenadas.z -256) / 2.56;
		return resultado;
}

void initParticleBuffers(){
	// Generrar los buffers
	glGenBuffers(1,&initVel);
	glGenBuffers(1,&startTime);
	// Reserva de memoria para los buffers
	int sizeInitVel = nParticles * 3 * sizeof(GL_FLOAT);
	int sizeStartTime = nParticles * sizeof(GL_FLOAT);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, sizeInitVel,NULL,GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferData(GL_ARRAY_BUFFER, sizeStartTime,NULL,GL_STATIC_DRAW);

	// Generar la velocidad inicial con velocidades aleatorias
	glm::vec3 v(0.0f);
	float velocity,theta,phi;
	GLfloat *data = new GLfloat[nParticles *3];
	for (unsigned int i=0;i<nParticles;i++){
		theta = glm::mix(0.0f,glm::pi<float>() / 6.0f, ((float) rand() / RAND_MAX)); //aleatorios para la funcin esferica
		phi = glm::mix(0.0f,glm::two_pi<float>() , ((float) rand() / RAND_MAX));
		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta/2.0f); // Cambiar para que se mueva poco en y
		v.z = sinf(theta) * sinf(phi); //direcciones aeatorias

		velocity=glm::mix(0.6f,0.8f,((float) rand() / RAND_MAX)); //magnitud aleatoria
		v = glm::normalize(v) * velocity;

		data[3*i] = v.x;
		data[3*i+1] = v.y;
		data[3*i+2] = v.z;
	}
	
	glBindBuffer(GL_ARRAY_BUFFER,initVel);
	glBufferSubData(GL_ARRAY_BUFFER,0,sizeInitVel,data);
	delete[] data;
	// Tiempo inicial de la particula (Cuando nace)
	data = new GLfloat[nParticles];
	float time =0.0f;
	float rate =0.00075f;
	for (unsigned int i=0; i < nParticles;i++){
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER,startTime);
	glBufferSubData(GL_ARRAY_BUFFER,0,sizeStartTime,data);
	delete[] data;
	glBindBuffer(GL_ARRAY_BUFFER,0);

	glGenVertexArrays(1,&VAOParticles); //con el VAOParticles enlazado ya no se necesita reconfigurar, solo se una VAO
	glBindVertexArray(VAOParticles);
	glBindBuffer(GL_ARRAY_BUFFER,initVel);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER,startTime);
	glVertexAttribPointer (1,1,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}
// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {
	AditionalLights=0; 

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
				glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
				nullptr);

	if (window == nullptr) {
		std::cerr
				<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
				<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox_fog.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation_shadow.vs", "../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs", "../Shaders/terrain_shadow.fs");
	shaderTexture.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado.fs");
	shaderViewDepth.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado_depth_view.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs", "../Shaders/shadow_mapping_depth.fs");
	textureParticlesLazer.initialize("../Shaders/particlesFountain.vs", "../Shaders/particlesFountain.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	rayModel.init();
	rayModel.setShader(&shader);
	rayModel.setColor(glm::vec4(1.0));

	boxCesped.init();
	boxCesped.setShader(&shaderMulLighting);

	boxWalls.init();
	boxWalls.setShader(&shaderMulLighting);

	boxHighway.init();
	boxHighway.setShader(&shaderMulLighting);

	boxLandingPad.init();
	boxLandingPad.setShader(&shaderMulLighting);

	esfera1.init();
	esfera1.setShader(&shaderMulLighting);

	boxIntro.init();
	boxIntro.setShader(&shaderTexture);
	boxIntro.setScale(glm::vec3(2.0, 2.0, 1.0));

	boxViewDepth.init();
	boxViewDepth.setShader(&shaderViewDepth);

	modelRock.loadModel("../models/rock/rock.obj");
	modelRock.setShader(&shaderMulLighting);

	/*modelPistol.loadModel("../models/MyPistol/sb.obj");
	modelPistol.setShader(&shaderMulLighting);*/

	modelAircraft.loadModel("../models/Aircraft_obj/Nave.obj");
	modelAircraft.setShader(&shaderMulLighting);
	
	modelAircraftVehicule.loadModel("../models/Aircraft_obj/flying sacuer.obj"); 
	modelAircraftVehicule.setShader(&shaderMulLighting); 
	//Lamps models
	modelLampPlant1.loadModel("../models/Lamp1/Plant_1.fbx"); 
	modelLampPlant1.setShader(&shaderMulLighting); 
	modelLampPlant2.loadModel("../models/Lamp2/Tree_Light_2.fbx"); 
	modelLampPlant2.setShader(&shaderMulLighting); 


	//New models carl, hunter and  protagonist
	CarlModelAnimate.loadModel("../models/carlRobot/model.fbx"); 
	CarlModelAnimate.setShader(&shaderMulLighting); 
 
	ProtagonistModelAnimate.loadModel("../models/SpaceSuit/SpaceSuit3.fbx"); 
	ProtagonistModelAnimate.setShader(&shaderMulLighting); 
 
	HunterModelAnimate.loadModel("../models/Hunter/model.fbx"); 
	HunterModelAnimate.setShader(&shaderMulLighting);

	// Terreno
	terrain.init();
	terrain.setShader(&shaderTerrain);

	// Se inicializa el model de render text
	modelText = new FontTypeRendering::FontTypeRendering(screenWidth, screenHeight);
	modelText->Initialize();

	camera->setPosition(glm::vec3(0.0,7.0,-4.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(1.0);
	
	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		skyboxTexture.loadImage(true);
		if (skyboxTexture.getData()) {
			glTexImage2D(types[i], 0, skyboxTexture.getChannels() == 3 ? GL_RGB : GL_RGBA, skyboxTexture.getWidth(), skyboxTexture.getHeight(), 0,
			skyboxTexture.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, skyboxTexture.getData());
		} else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage();
	}

	// Definiendo la textura a utilizar
	Texture textureCraterMars("../Textures/MyTextures/Crater.jpg"); 
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	textureCraterMars.loadImage();
	// Creando la textura con id 1
	glGenTextures(1, &textureCraterMartsID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureCraterMartsID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (textureCraterMars.getData()) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		std::cout << "Numero de canales :=> " << textureCraterMars.getChannels() << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, textureCraterMars.getChannels() == 3 ? GL_RGB : GL_RGBA, textureCraterMars.getWidth(), textureCraterMars.getHeight(), 0,
		textureCraterMars.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureCraterMars.getData());
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureCraterMars.freeImage();

	// Definiendo la textura
	Texture textureLandingPad("../Textures/landingPad.jpg");
	textureLandingPad.loadImage(); // Cargar la textura
	glGenTextures(1, &textureLandingPadID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureLandingPadID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(textureLandingPad.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureLandingPad.getChannels() == 3 ? GL_RGB : GL_RGBA, textureLandingPad.getWidth(), textureLandingPad.getHeight(), 0,
		textureLandingPad.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureLandingPad.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureLandingPad.freeImage(); // Liberamos memoria

	// Defininiendo texturas del mapa de mezclas BLEND
	// Definiendo la textura
	Texture textureR("../Textures/MyTextures/Orillas.jpg"); 
	textureR.loadImage(); // Cargar la textura 
	glGenTextures(1, &textureTerrainRID); // Creando el id de la textura del landingpad 
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID); // Se enlaza la textura 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion 
	if(textureR.getData()){ 
		// Transferir los datos de la imagen a la tarjeta 
		glTexImage2D(GL_TEXTURE_2D, 0, textureR.getChannels() == 3 ? GL_RGB : GL_RGBA, textureR.getWidth(), textureR.getHeight(), 0, 
		textureR.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureR.getData()); 
		glGenerateMipmap(GL_TEXTURE_2D); 
	} 
	else  
		std::cout << "Fallo la carga de textura" << std::endl; 
	textureR.freeImage(); // Liberamos memoria 
 
	// Definiendo la textura G iD  
	Texture textureG("../Textures/MyTextures/Land.jpg"); 
	textureG.loadImage(); // Cargar la textura 
	glGenTextures(1, &textureTerrainGID); // Creando el id de la textura del landingpad 
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID); // Se enlaza la textura 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion 
	if(textureG.getData()){ 
		// Transferir los datos de la imagen a la tarjeta 
		glTexImage2D(GL_TEXTURE_2D, 0, textureG.getChannels() == 3 ? GL_RGB : GL_RGBA, textureG.getWidth(), textureG.getHeight(), 0, 
		textureG.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureG.getData()); 
		glGenerateMipmap(GL_TEXTURE_2D); 
	} 
	else  
		std::cout << "Fallo la carga de textura" << std::endl; 
	textureG.freeImage(); // Liberamos memoria 
 
	// Definiendo la textura B iD  
	Texture textureB("../Textures/Mytextures/PathM.jpg"); 
	textureB.loadImage(); // Cargar la textura 
	glGenTextures(1, &textureTerrainBID); // Creando el id de la textura del landingpad 
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID); // Se enlaza la textura 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion 
	if(textureB.getData()){ 
		// Transferir los datos de la imagen a la tarjeta 
		glTexImage2D(GL_TEXTURE_2D, 0, textureB.getChannels() == 3 ? GL_RGB : GL_RGBA, textureB.getWidth(), textureB.getHeight(), 0, 
		textureB.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureB.getData()); 
		glGenerateMipmap(GL_TEXTURE_2D); 
	} 
	else  
		std::cout << "Fallo la carga de textura" << std::endl; 
	textureB.freeImage(); // Liberamos memoria 
 
	// Definiendo la map mezclas 
	Texture textureBlendMap("../Textures/MyTextures/MapaMarBlendtMap.png"); 
	textureBlendMap.loadImage(); // Cargar la textura 
	glGenTextures(1, &textureTerrainBlendMapID); // Creando el id de la textura del landingpad 
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID); // Se enlaza la textura 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion 
	if(textureBlendMap.getData()){ 
		// Transferir los datos de la imagen a la tarjeta 
		glTexImage2D(GL_TEXTURE_2D, 0, textureBlendMap.getChannels() == 3 ? GL_RGB : GL_RGBA, textureBlendMap.getWidth(), textureBlendMap.getHeight(), 0, 
		textureBlendMap.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureBlendMap.getData()); 
		glGenerateMipmap(GL_TEXTURE_2D); 
	} 
	else  
		std::cout << "Fallo la carga de textura" << std::endl; 
	textureBlendMap.freeImage();

	// Definiendo la textura
	Texture textureIntro1("../Textures/Intro1.png");
	textureIntro1.loadImage(); // Cargar la textura
	glGenTextures(1, &textureInit1ID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureInit1ID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(textureIntro1.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureIntro1.getChannels() == 3 ? GL_RGB : GL_RGBA, textureIntro1.getWidth(), textureIntro1.getHeight(), 0,
		textureIntro1.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureIntro1.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureIntro1.freeImage(); // Liberamos memoria

	// Definiendo la textura
	Texture textureIntro2("../Textures/Intro2.png");
	textureIntro2.loadImage(); // Cargar la textura
	glGenTextures(1, &textureInit2ID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureInit2ID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(textureIntro2.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureIntro2.getChannels() == 3 ? GL_RGB : GL_RGBA, textureIntro2.getWidth(), textureIntro2.getHeight(), 0,
		textureIntro2.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureIntro2.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureIntro2.freeImage(); // Liberamos memoria

	// Definiendo la textura
	Texture textureScreen("../Textures/Screen.png");
	textureScreen.loadImage(); // Cargar la textura
	glGenTextures(1, &textureScreenID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureScreenID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(textureScreen.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureScreen.getChannels() == 3 ? GL_RGB : GL_RGBA, textureScreen.getWidth(), textureScreen.getHeight(), 0,
		textureScreen.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureScreen.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureScreen.freeImage(); // Liberamos memoria

	// Definiendo la textura
	Texture textureParticlesLazer("../Textures/bluewater.png");
	textureParticlesLazer.loadImage(); // Cargar la textura
	glGenTextures(1, &textureParticlesLazerID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureParticlesLazerID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(textureParticlesLazer.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureParticlesLazer.getChannels() == 3 ? GL_RGB : GL_RGBA, textureParticlesLazer.getWidth(), textureParticlesLazer.getHeight(), 0,
		textureParticlesLazer.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureParticlesLazer.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureParticlesLazer.freeImage(); // Liberamos memoria

	/*******************************************
	 * OpenAL init
	 *******************************************/
	alutInit(0, nullptr);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}
	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	buffer[0] = alutCreateBufferFromFile("../sounds/fountain.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/fire.wav");
	int errorAlut = alutGetError();
	if (errorAlut != ALUT_ERROR_NO_ERROR){
		printf("- Error open files with alut %d !!\n", errorAlut);
		exit(2);
	}

	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 3.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 2000);

	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 0.5f);
	alSourcefv(source[1], AL_POSITION, source1Pos);
	alSourcefv(source[1], AL_VELOCITY, source1Vel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_TRUE);
	alSourcef(source[1], AL_MAX_DISTANCE, 1000);

	/*******************************************
	 * Inicializacion del framebuffer para
	 * almacenar el buffer de profunidadad
	 *******************************************/
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Inicializacion del los datos de la particula 
	initParticleBuffers();
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();
	textureParticlesLazer.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCesped.destroy();
	boxWalls.destroy();
	boxHighway.destroy();
	boxLandingPad.destroy();
	esfera1.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();
	rayModel.destroy();
	boxIntro.destroy();
	boxViewDepth.destroy();

	// Custom objects Delete
	modelAircraft.destroy();
	modelAircraftVehicule.destroy();
	modelRock.destroy();
	//modelPistol.destroy();
	modelLampPlant1.destroy();
	modelLampPlant2.destroy();

	//Destroy MY NewModels 

	CarlModelAnimate.destroy(); 
	HunterModelAnimate.destroy(); 
	ProtagonistModelAnimate.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCraterMartsID);
	glDeleteTextures(1, &textureWallID);
	glDeleteTextures(1, &textureWindowID);
	glDeleteTextures(1, &textureHighwayID);
	glDeleteTextures(1, &textureLandingPadID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureInit1ID);
	glDeleteTextures(1, &textureInit2ID);
	glDeleteTextures(1, &textureScreenID);
	glDeleteTextures(1, &textureParticlesLazerID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);

	// Liberar los datos del buffer de las particulas
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glDeleteBuffers(1,&initVel);
	glDeleteBuffers(1,&startTime);
	glBindVertexArray(0);
	glDeleteVertexArrays(1,&VAOParticles);
}

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
	distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	if(!iniciaPartida){
		bool presionarEnter = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
		if(textureActivaID == textureInit1ID && presionarEnter){
			iniciaPartida = true;
			textureActivaID = textureScreenID;
		}
		else if(!presionarOpcion && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
			presionarOpcion = true;
			if(textureActivaID == textureInit1ID)
				textureActivaID = textureInit2ID;
			else if(textureActivaID == textureInit2ID)
				textureActivaID = textureInit1ID;
		}
		else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
			presionarOpcion = false;
	}
	//GAME PAD PROTAGONIST CONTROL
	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
		int axesCount, buttonCount;
		const float * axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		//Movimiento axis XZ 
		float AxisLeftUpDonw = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]; 
		float AxisLeftX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]; 
		float AxisRightX= state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
		float AxisRightY= state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
		if( ((AxisLeftUpDonw>0.5 || AxisLeftUpDonw<-0.5) || (AxisLeftX>0.5 || AxisLeftX<-0.5)) && !animationRuningShootingRunning ){ 
			modelMatrixProtagonist = glm::translate(modelMatrixProtagonist, glm::vec3(0.02*AxisLeftX*-1 *velProtagonist, 0.0, 0.02*AxisLeftUpDonw*-1*velProtagonist)); 
			if(AxisLeftX<-0.5){ 
				animationProtagonistIndex= IndexAnimationMoveLeft; 
			}else if(AxisLeftX>0.5){ 
				animationProtagonistIndex=IndexAnimationMoveRight ; 
			}else if (AxisLeftUpDonw>0.5 ){ 
				animationProtagonistIndex=IndexAnimationRunBack; 
			}else if (AxisLeftUpDonw <-0.5){ 
				animationProtagonistIndex= IndexAnimationWalk; 
			} 
		} 
		if(AxisRightX!=0){ 
			modelMatrixProtagonist = glm::rotate(modelMatrixProtagonist,0.02f*AxisRightX*-1,glm::vec3(0,1,0));
			camera->mouseMoveCamera(0.02f*AxisRightX*-1,0.0, deltaTime);
			if(camera->pitch < 0.038 ){
				camera->mouseMoveCamera(0.0,0.3f*abs(AxisRightY)*1, deltaTime);
			}else if(camera->pitch>0.28){
				camera->mouseMoveCamera(0.0,0.3f*abs(AxisRightY)*-1, deltaTime);
			}else{
				camera->mouseMoveCamera(0.0,0.3f*(AxisRightY)*-1, deltaTime);
			}
			//std::cout<< camera->pitch << std::endl;
		} 
		//Boton de ataque 
		if((state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS) && !animationRuningShootingRunning){ 
			animationProtagonistIndex=IndexAnimationShoot;
			animationRuningShootingRunning=true;
		}
		if( !((AxisLeftUpDonw>0.5 || AxisLeftUpDonw<-0.5) || (AxisLeftX>0.5 || AxisLeftX<-0.5))  && !animationRuningShootingRunning && !isJump){
			animationProtagonistIndex=IndexAnimationIdle; 
		}

		const unsigned char * buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		//std::cout << "Número de botones disponibles :=>" << buttonCount << std::endl;
		if(buttons[1] == GLFW_PRESS)
		//	std::cout << "Se presiona A" << std::endl;

		if(!isJump && buttons[1] == GLFW_PRESS && !animationRuningShootingRunning){
			isJump = true;
			startTimeJump = currTime;
			tmv = 0;
		}
	}

	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera->mouseMoveCamera(0.0, offsetY, deltaTime);

	offsetX = 0;
	offsetY = 0;
	glfwPollEvents();
	return continueApplication;
}

void prepareScene(){

	terrain.setShader(&shaderTerrain);
	
	modelRock.setShader(&shaderMulLighting);

	//modelPistol.setShader(&shaderMulLighting);

	modelAircraft.setShader(&shaderMulLighting);
	modelAircraftVehicule.setShader(&shaderMulLighting);

	//Lamp models
	modelLampPlant1.setShader(&shaderMulLighting);
	modelLampPlant2.setShader(&shaderMulLighting);


	//My characters
	CarlModelAnimate.setShader(&shaderMulLighting);
	ProtagonistModelAnimate.setShader(&shaderMulLighting);
	HunterModelAnimate.setShader(&shaderMulLighting);
}

void prepareDepthScene(){

	terrain.setShader(&shaderDepth);
	
	modelRock.setShader(&shaderDepth);

	//modelPistol.setShader(&shaderDepth);

	modelAircraft.setShader(&shaderDepth);
	modelAircraftVehicule.setShader(&shaderDepth);

	//Lamp models
	modelLampPlant1.setShader(&shaderDepth);
	modelLampPlant2.setShader(&shaderDepth);

	//My charcters Depth
	CarlModelAnimate.setShader(&shaderDepth);
	ProtagonistModelAnimate.setShader(&shaderDepth);
	HunterModelAnimate.setShader(&shaderDepth);
}
void generatePointLight(glm::vec3 LampPlantPostion2,int i,glm::vec3 DiffuseColor,glm::vec3 ambient,glm::vec3 specular,float quadratic){

	shaderMulLighting.setVectorFloat3("pointLights["+std::to_string(i)+"].light.ambient",glm::value_ptr(ambient)); // Queda como "pointLights[0].light.ambient" 
	shaderMulLighting.setVectorFloat3("pointLights["+std::to_string(i)+"].light.diffuse",glm::value_ptr(DiffuseColor)); 
	shaderMulLighting.setVectorFloat3("pointLights["+std::to_string(i)+"].light.specular",glm::value_ptr(specular)); 
	shaderMulLighting.setVectorFloat3("pointLights["+std::to_string(i)+"].position",glm::value_ptr(glm::vec3(LampPlantPostion2))); 
	shaderMulLighting.setFloat("pointLights["+std::to_string(i)+"].constant",1.0); 
	shaderMulLighting.setFloat("pointLights["+std::to_string(i)+"].linear",0.09); 
	shaderMulLighting.setFloat("pointLights["+std::to_string(i)+"].quadratic", quadratic); 
	shaderTerrain.setVectorFloat3("pointLights["+std::to_string(i)+"].light.ambient",glm::value_ptr(ambient)); // Queda como "pointLights[0].light.ambient" 
	shaderTerrain.setVectorFloat3("pointLights["+std::to_string(i)+"].light.diffuse",glm::value_ptr(DiffuseColor)); 
	shaderTerrain.setVectorFloat3("pointLights["+std::to_string(i)+"].light.specular",glm::value_ptr(specular)); 
	shaderTerrain.setVectorFloat3("pointLights["+std::to_string(i)+"].position",glm::value_ptr(glm::vec3(LampPlantPostion2))); 
	shaderTerrain.setFloat("pointLights["+std::to_string(i)+"].constant",1.0); 
	shaderTerrain.setFloat("pointLights["+std::to_string(i)+"].linear",0.09); 
	shaderTerrain.setFloat("pointLights["+std::to_string(i)+"].quadratic", quadratic); 
} 
void PointLightGeneratePlant1(glm::vec3 PositionInsidePlant){ 
	for(int i = 0; i < LampPlantPostion.size();i++){ // Recordar transformacion de abajo haca arribase aplican 
			glm::mat4 matrixAdjustLampPlant = glm::mat4(1.0); 
			matrixAdjustLampPlant = glm::translate(matrixAdjustLampPlant,LampPlantPostion[i]); 
			matrixAdjustLampPlant = glm::rotate(matrixAdjustLampPlant,glm::radians(LampPlantOrientation[i]),glm::vec3(0,1,0)); 
			matrixAdjustLampPlant = glm::scale(matrixAdjustLampPlant, glm::vec3(1.25)); 
			matrixAdjustLampPlant= glm::translate(matrixAdjustLampPlant,glm::vec3(PositionInsidePlant)); 
			glm::vec3 LampPlantPostion = glm::vec3(matrixAdjustLampPlant[3]); 
			generatePointLight(LampPlantPostion,LampPlantPostion2.size()+i+AditionalLights,glm::vec3(0.25,0.25,0.0),glm::vec3(0.25,0.25,0.25),glm::vec3(1.0,0.5,0.0),0.08); 
			} 
			AditionalLights+=LampPlantPostion.size(); 
}
void renderSolidScene(){
	/*******************************************
	 * Terrain Cesped
	 *******************************************/
	// Se activa la textura del agua
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureCraterMartsID);
	shaderTerrain.setInt("backgroundTexture", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	shaderTerrain.setInt("rTexture", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	shaderTerrain.setInt("gTexture", 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	shaderTerrain.setInt("bTexture", 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	shaderTerrain.setInt("blendMapTexture", 4);
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(80, 80)));
	terrain.setPosition(glm::vec3(100, 0, 100));
	terrain.render();
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
	glBindTexture(GL_TEXTURE_2D, 0);

	/*******************************************
	 * Custom objects obj
	 *******************************************/
	//Rock render
	matrixModelRock[3][1] = terrain.getHeightTerrain(matrixModelRock[3][0], matrixModelRock[3][2]);
	modelRock.render(matrixModelRock);

	//PISTOL
	//glm::mat4 ModelMatrixBodyPistol =  glm::mat4(modelMatrixProtagonist);
	//modelPistol.render(ModelMatrixBodyPistol);

	// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
	glActiveTexture(GL_TEXTURE0);
	

	for (int i =0; i < LampPlantPostion.size();i++){ 
			LampPlantPostion[i].y = terrain.getHeightTerrain(LampPlantPostion[i].x,LampPlantPostion[i].z); 
			modelLampPlant1.setPosition(LampPlantPostion[i]); 
			modelLampPlant1.setScale(glm::vec3(1.25)); 
			modelLampPlant1.setOrientation(glm::vec3(0,LampPlantOrientation[i],0)); 
			modelLampPlant1.render(); 
		} 
		for (int i =0; i < LampPlantPostion2.size();i++){ 
			LampPlantPostion2[i].y = terrain.getHeightTerrain(LampPlantPostion2[i].x,LampPlantPostion2[i].z); 
			modelLampPlant2.setPosition(LampPlantPostion2[i]); 
			modelLampPlant2.setScale(glm::vec3(1.25)); 
			modelLampPlant2.setOrientation(glm::vec3(0,LampPlant2Orientation[i],0)); 
			modelLampPlant2.render(); 
		} 
	/*****************************************
	 * Objetos animados por huesos
	 * **************************************/
	glm::vec3 ejey = glm::normalize(terrain.getNormalTerrain(modelMatrixProtagonist[3][0], modelMatrixProtagonist[3][2]));
	glm::vec3 ejex = glm::vec3(modelMatrixProtagonist[0]);
	glm::vec3 ejez = glm::normalize(glm::cross(ejex, ejey));
	ejex = glm::normalize(glm::cross(ejey, ejez));
	modelMatrixProtagonist[0] = glm::vec4(ejex, 0.0);
	modelMatrixProtagonist[1] = glm::vec4(ejey, 0.0);
	modelMatrixProtagonist[2] = glm::vec4(ejez, 0.0);
	modelMatrixProtagonist[3][1] = -GRAVITY * tmv * tmv + 3.5 * tmv + terrain.getHeightTerrain(modelMatrixProtagonist[3][0], modelMatrixProtagonist[3][2]);
	tmv = currTime - startTimeJump;
	
	if(modelMatrixProtagonist[3][1] < terrain.getHeightTerrain(modelMatrixProtagonist[3][0], modelMatrixProtagonist[3][2])){
		isJump = false;
		modelMatrixProtagonist[3][1] = terrain.getHeightTerrain(modelMatrixProtagonist[3][0], modelMatrixProtagonist[3][2]);
	}
	glm::mat4 modelMatrixProtaBody = glm::mat4(modelMatrixProtagonist);
	ProtagonistModelAnimate.setAnimationIndex(animationProtagonistIndex);
	modelMatrixProtaBody = glm::scale(modelMatrixProtaBody,glm::vec3(1.0f));
	ProtagonistModelAnimate.render(modelMatrixProtaBody);
	//animationProtagonistIndex=IndexAnimationIdle;

	glm::mat4 modelMatrixCarlBody = glm::mat4(modelMatrixCarl); // new variable for scaling 
	modelMatrixCarlBody = glm::scale(modelMatrixCarlBody,glm::vec3(0.04f)); 
	CarlModelAnimate.render(modelMatrixCarlBody); 

	glm::mat4 modelMatrixHunterBody = glm::mat4(modelMatrixHunter); // new variable for scaling 
	modelMatrixHunterBody = glm::scale(modelMatrixHunterBody,glm::vec3(0.01f)); 
	HunterModelAnimate.setAnimationIndex(1); 
	HunterModelAnimate.render(modelMatrixHunterBody);

	glm::mat4 modelMatrixmodelAircraftVehiculeBody = glm::mat4(modelMatrixAircraft); 
	modelMatrixmodelAircraftVehiculeBody[3][1]= terrain.getHeightTerrain(modelMatrixmodelAircraftVehiculeBody[3][0], modelMatrixmodelAircraftVehiculeBody[3][2]) +3.0+ currentNaveHeight; 
	modelMatrixmodelAircraftVehiculeBody = glm::rotate(modelMatrixmodelAircraftVehiculeBody, rotNave, glm::vec3(0, 1, 0)); 
	modelAircraftVehicule.render(modelMatrixmodelAircraftVehiculeBody);

	/*******************************************
	 * Skybox
	 *******************************************/
	GLint oldCullFaceMode;
	GLint oldDepthFuncMode;
	// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
	glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
	glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
	shaderSkybox.setFloat("skybox", 0);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);
	glActiveTexture(GL_TEXTURE0);
	skyboxSphere.render();
	glCullFace(oldCullFaceMode);
	glDepthFunc(oldDepthFuncMode);
}

void renderAlphaScene(bool render = true){
	/**********
	 * Update the position with alpha objects
	 */
	// Update the aircraft
	blendingUnsorted.find("aircraft")->second = glm::vec3(modelMatrixAircraft[3]);
	//Update Lazer qith player possition
	blendingUnsorted.find("LazerSource")-> second= glm::vec3(modelMatrixAircraft[3]);

	/**********
	 * Sorter with alpha objects
	 */
	std::map<float, std::pair<std::string, glm::vec3>> blendingSorted;
	std::map<std::string, glm::vec3>::iterator itblend;
	for(itblend = blendingUnsorted.begin(); itblend != blendingUnsorted.end(); itblend++){
		float distanceFromView = glm::length(camera->getPosition() - itblend->second);
		blendingSorted[distanceFromView] = std::make_pair(itblend->first, itblend->second);
	}

	/**********
	 * Render de las transparencias
	 */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	for(std::map<float, std::pair<std::string, glm::vec3> >::reverse_iterator it = blendingSorted.rbegin(); it != blendingSorted.rend(); it++){
		if(it->second.first.compare("aircraft") == 0){
			// Render for the aircraft model
			glm::mat4 modelMatrixAircraftBlend = glm::mat4(modelMatrixAircraft);
			modelMatrixAircraftBlend[3][1] = terrain.getHeightTerrain(modelMatrixAircraftBlend[3][0], modelMatrixAircraftBlend[3][2]) + 3.0+currentNaveHeight;
			modelAircraft.render(modelMatrixAircraftBlend);
		}
		else if (render && it->second.first.compare("LazerSource")==0){
			//Se renderiza el sistema de particulas
			glm::mat4 modelMatrixParticlesLazer = glm::mat4(modelMatrixAircraft);
			modelMatrixParticlesLazer = glm::translate(modelMatrixParticlesLazer,it->second.second);
			modelMatrixParticlesLazer[3][1] = terrain.getHeightTerrain(modelMatrixParticlesLazer[3][0],modelMatrixParticlesLazer[3][2]) + 1.0;
			modelMatrixParticlesLazer = glm::scale(modelMatrixParticlesLazer,glm::vec3(1.0f));
			currTimeParticlesAnimation = TimeManager::Instance().GetTime();
			if(currTimeParticlesAnimation - lastTimeParticlesAnimation > 0.5f){ //tiempo actual menos ultima medicion mayor a 10, termino animacion
				lastTimeParticlesAnimation = currTimeParticlesAnimation; // Reiniciar animacion
			}
			glDepthMask(GL_FALSE);
			// Particle size
			glPointSize (5.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticlesLazerID);
			textureParticlesLazer.turnOn();
			textureParticlesLazer.setFloat("Time",float (currTimeParticlesAnimation - lastTimeParticlesAnimation)); // segundo desde que inicio la animacion
			textureParticlesLazer.setFloat("ParticleLifetime",0.5f);
			textureParticlesLazer.setInt("ParticleTex",0);
			textureParticlesLazer.setVectorFloat3("Gravity",glm::value_ptr(glm::vec3(0.0f,-0.1f,0.0f)));
			textureParticlesLazer.setMatrix4("model",1,false,glm::value_ptr(modelMatrixParticlesLazer));
			glBindVertexArray(VAOParticles);
			glDrawArrays(GL_POINTS,0,nParticles);
			glDepthMask(GL_TRUE);
			textureParticlesLazer.turnOff();
		}
	}
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	if(render){
		/************Render de imagen de frente**************/
		shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
		shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureActivaID);
		shaderTexture.setInt("outTexture", 0);
		glEnable(GL_BLEND);
		boxIntro.render();
		glDisable(GL_BLEND);

		modelText->render("Texto en OpenGL", -1,0,0,1,0,35);
	}
}

void renderScene(){
	renderSolidScene();
	renderAlphaScene(false);
}
float currentTime=0;
float elapsedTime=0;
void WaitTime() {
    if (animationRuningShootingRunning) {
        currentTime = deltaTime;
        elapsedTime = currentTime + elapsedTime;

        if (elapsedTime >= animationDuration) {
			animationProtagonistIndex = IndexAnimationIdle;
			if (elapsedTime >= animationDuration + 0.4f){
				std::cout << "Actual reset!" << std::endl;
				animationRuningShootingRunning = false;
				currentTime=0;
				elapsedTime=0;
			}
            std::cout << "Animation shoot ended!" << std::endl;
        } else {
            float progress = elapsedTime / animationDuration;
            // Update your animation state based on progress
            std::cout << "Animation progress: " << progress * 100.0f << "%" << std::endl;
        }
    }
}
glm::vec3 CoordanatesGimpTemp = glm::vec3(0);
void applicationLoop() {
	bool psi = true;

	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	matrixModelRock = glm::translate(matrixModelRock, glm::vec3(-3.0, 0.0, 2.0));

	CoordanatesGimpTemp = glm::vec3(233,0,376);
	CoordanatesGimpTemp = TransformGIMPCoordenatesToOpenGLPixels(CoordanatesGimpTemp);
	modelMatrixAircraft = glm::translate(modelMatrixAircraft, CoordanatesGimpTemp);
	modelMatrixAircraft = glm::translate(modelMatrixAircraft, glm::vec3(0.0f, 3.0, 0.0)); 


	//New models hunter carl positions 
	
	CoordanatesGimpTemp = glm::vec3(243,0,244);
	CoordanatesGimpTemp = TransformGIMPCoordenatesToOpenGLPixels(CoordanatesGimpTemp);
	modelMatrixCarl = glm::translate(modelMatrixCarl,CoordanatesGimpTemp);

	CoordanatesGimpTemp = glm::vec3(226,0,334);
	CoordanatesGimpTemp = TransformGIMPCoordenatesToOpenGLPixels(CoordanatesGimpTemp);
	modelMatrixHunter = glm::translate(modelMatrixHunter,CoordanatesGimpTemp); 
	modelMatrixHunter = glm::rotate(modelMatrixHunter, glm::radians(180.0f), glm::vec3(0.0,1.0,0.0)); 
	
	CoordanatesGimpTemp = glm::vec3(250,0,403);
	CoordanatesGimpTemp = TransformGIMPCoordenatesToOpenGLPixels(CoordanatesGimpTemp);
	modelMatrixProtagonist = glm::translate(modelMatrixProtagonist, CoordanatesGimpTemp);
	modelMatrixProtagonist = glm::rotate(modelMatrixProtagonist, glm::radians(180.0f), glm::vec3(0.0,1.0,0.0)); 

	lastTime = TimeManager::Instance().GetTime();

	textureActivaID = textureInit1ID;

	glm::vec3 lightPos = glm::vec3(10.0, 10.0, -10.0);

	shadowBox = new ShadowBox(-lightPos, camera.get(), 30.0f, 0.1f, 45.0f);

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if(currTime - lastTime < 0.016666667){
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		WaitTime();
		psi = processInput(true);

		std::map<std::string, bool> collisionDetection;
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Camera Setting for protagonist

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
				(float) screenWidth / (float) screenHeight, 0.01f, 100.0f);

		axis = glm::axis(glm::quat_cast(modelMatrixProtagonist));
		angleTarget = glm::angle(glm::quat_cast(modelMatrixProtagonist));
		target = modelMatrixProtagonist[3];
	

		if(std::isnan(angleTarget))
			angleTarget = 0.0;
		if(axis.y < 0)
			angleTarget = -angleTarget;

		camera->setCameraTarget(target);
		camera->setAngleTarget(angleTarget);
		camera->updateCamera();
		glm::mat4 view = camera->getViewMatrix();

		shadowBox->update(screenWidth, screenHeight);
		glm::vec3 centerBox = shadowBox->getCenter();

		// Projection light shadow mapping
		glm::mat4 lightProjection = glm::mat4(1.0f), lightView = glm::mat4(1.0f);
		glm::mat4 lightSpaceMatrix;
		lightProjection[0][0] = 2.0f / shadowBox->getWidth();
		lightProjection[1][1] = 2.0f / shadowBox->getHeight();
		lightProjection[2][2] = -2.0f / shadowBox->getLength();
		lightProjection[3][3] = 1.0f;
		lightView = glm::lookAt(centerBox, centerBox + glm::normalize(-lightPos), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		shaderDepth.setMatrix4("lightSpaceMatrix", 1, false, glm::value_ptr(lightSpaceMatrix));

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
				glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
					glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
				glm::value_ptr(view));
		shaderMulLighting.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderTerrain.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false,
				glm::value_ptr(view));
		shaderTerrain.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader para el fountain
		textureParticlesLazer.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		textureParticlesLazer.setMatrix4("view", 1, false,
				glm::value_ptr(view));

		/*******************************************
		 * Propiedades de neblina
		 *******************************************/
		shaderMulLighting.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(1.0, 0.1, 0.1)));
		shaderTerrain.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderSkybox.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		shaderMulLighting.setInt("spotLightCount", 1); 
		shaderTerrain.setInt("spotLightCount", 1); 
		glm::vec3 spotPosition = glm::vec3{modelMatrixAircraft*glm::vec4(0,0,0,1.0)}; 
		shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient",glm::value_ptr(glm::vec3(0.4,1.0,0.4))); // Queda como "pointLights[0].light.ambient" 
		shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse",glm::value_ptr(glm::vec3(0.0,1.0,0.0))); 
		shaderMulLighting.setVectorFloat3("spotLights[0].light.specular",glm::value_ptr(glm::vec3(0.5,0.5,0.5))); 
		shaderMulLighting.setVectorFloat3("spotLights[0].position",glm::value_ptr(spotPosition)); 
		shaderMulLighting.setVectorFloat3("spotLights[0].direction",glm::value_ptr(glm::vec3(0,-1,0))); 
		shaderMulLighting.setFloat("spotLights[0].constant",1.0); 
		shaderMulLighting.setFloat("spotLights[0].linear",0.07); 
		shaderMulLighting.setFloat("spotLights[0].quadratic",0.1); 
		shaderMulLighting.setFloat("spotLights[0].cutOff",cos(glm::radians(5.0f))); 
		shaderMulLighting.setFloat("spotLights[0].outerCutOff",cos(glm::radians(20.0f))); 
		shaderTerrain.setVectorFloat3("spotLights[0].light.ambient",glm::value_ptr(glm::vec3(0.4,1.0,0.4))); // Queda como "pointLights[0].light.ambient" 
		shaderTerrain.setVectorFloat3("spotLights[0].light.diffuse",glm::value_ptr(glm::vec3(0.0,1.0,0.0))); 
		shaderTerrain.setVectorFloat3("spotLights[0].light.specular",glm::value_ptr(glm::vec3(0.5,0.5,0.5))); 
		shaderTerrain.setVectorFloat3("spotLights[0].position",glm::value_ptr(spotPosition)); 
		shaderTerrain.setVectorFloat3("spotLights[0].direction",glm::value_ptr(glm::vec3(0,-1,0))); 
		shaderTerrain.setFloat("spotLights[0].constant",1.0); 
		shaderTerrain.setFloat("spotLights[0].linear",0.07); 
		shaderTerrain.setFloat("spotLights[0].quadratic",0.1); 
		shaderTerrain.setFloat("spotLights[0].cutOff",cos(glm::radians(5.0f))); 
		shaderTerrain.setFloat("spotLights[0].outerCutOff",cos(glm::radians(20.0f)));

		/*******************************************
		 * Propiedades PointLights
		 *******************************************/
		shaderMulLighting.setInt("pointLightCount",LampPlantPostion2.size()+1+ LampPlantPostion.size()*3); 
		shaderTerrain.setInt("pointLightCount", LampPlantPostion2.size()+1 + LampPlantPostion.size()*3); 
		for(int i = 0; i < LampPlantPostion2.size();i++){ // Recordar transformacion de abajo haca arribase aplican 
			glm::mat4 matrixAdjustLampPlant2 = glm::mat4(1.0); 
			matrixAdjustLampPlant2 = glm::translate(matrixAdjustLampPlant2,LampPlantPostion2[i]); 
			matrixAdjustLampPlant2 = glm::rotate(matrixAdjustLampPlant2,glm::radians(LampPlant2Orientation[i]),glm::vec3(0,1,0)); 
			matrixAdjustLampPlant2 = glm::scale(matrixAdjustLampPlant2, glm::vec3(1.25)); 
			matrixAdjustLampPlant2= glm::translate(matrixAdjustLampPlant2,glm::vec3(-0.265,2.048,0.82)); 
			glm::vec3 LampPlantPostion2 = glm::vec3(matrixAdjustLampPlant2[3]); 
			generatePointLight(LampPlantPostion2,i,glm::vec3(1.0,1.0,0.0),glm::vec3(0.25,0.25,0.25),glm::vec3(1.0,0.5,0.0),0.08); 
			} 
		//Generate Lights for inside the plant 
		PointLightGeneratePlant1(glm::vec3(-0.132,1.906,-0.163)); 
		PointLightGeneratePlant1(glm::vec3(-0.584,1.236,-0.147)); 
		PointLightGeneratePlant1(glm::vec3(0.704,1.325,-0.227)); 
		glm::vec3 spotPosition2 = glm::vec3{modelMatrixAircraft*glm::vec4(0.0,0.5f,0.0,1.0)};  // Localization light on ship
		generatePointLight(spotPosition2,LampPlantPostion2.size() +LampPlantPostion.size()*3,glm::vec3(0.0,1.0,0.0),glm::vec3(1.0,1.0,0.0),glm::vec3(0.4,1.0,0.4),2.0); 
		
		/************Render de imagen de frente**************/
		if(!iniciaPartida){
			shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
			shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureActivaID);
			shaderTexture.setInt("outTexture", 0);
			boxIntro.render();
			glfwSwapBuffers(window);
			continue;
		}

		/*******************************************
		 * 1.- We render the depth buffer
		 *******************************************/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render scene from light's point of view
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		prepareDepthScene();
		renderScene();
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*******************************************
		 * Debug to view the texture view map
		 *******************************************/
		// reset viewport
		/*glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render Depth map to quad for visual debugging
		shaderViewDepth.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
		shaderViewDepth.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
		shaderViewDepth.setFloat("near_plane", near_plane);
		shaderViewDepth.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		boxViewDepth.setScale(glm::vec3(2.0, 2.0, 1.0));
		boxViewDepth.render();*/

		/*******************************************
		 * 2.- We render the normal objects
		 *******************************************/
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		prepareScene();
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shaderMulLighting.setInt("shadowMap", 10);
		shaderTerrain.setInt("shadowMap", 10);
		renderSolidScene();

		/*******************************************
		 * Creacion de colliders
		 * IMPORTANT do this before interpolations
		 *******************************************/

		// Collider del aricraft
		glm::mat4 modelMatrixColliderAircraft = glm::mat4(modelMatrixAircraft);
		AbstractModel::OBB aircraftCollider;
		// Set the orientation of collider before doing the scale
		aircraftCollider.u = glm::quat_cast(modelMatrixAircraft);
		modelMatrixColliderAircraft = glm::scale(modelMatrixColliderAircraft,
				glm::vec3(1.0, 1.0, 1.0));
		modelMatrixColliderAircraft = glm::translate(
				modelMatrixColliderAircraft, modelAircraft.getObb().c);
		aircraftCollider.c = glm::vec3(modelMatrixColliderAircraft[3]);
		aircraftCollider.e = modelAircraft.getObb().e * glm::vec3(1.0, 1.0, 1.0);
		addOrUpdateColliders(collidersOBB, "aircraft", aircraftCollider, modelMatrixAircraft);

		//Collider del la rock
		AbstractModel::SBB rockCollider;
		glm::mat4 modelMatrixColliderRock= glm::mat4(matrixModelRock);
		modelMatrixColliderRock = glm::scale(modelMatrixColliderRock, glm::vec3(1.0, 1.0, 1.0));
		modelMatrixColliderRock = glm::translate(modelMatrixColliderRock, modelRock.getSbb().c);
		rockCollider.c = glm::vec3(modelMatrixColliderRock[3]);
		rockCollider.ratio = modelRock.getSbb().ratio * 1.0;
		addOrUpdateColliders(collidersSBB, "rock", rockCollider, matrixModelRock);

		// Lamps1 colliders
		for (int i = 0; i < LampPlantPostion.size(); i++){
			AbstractModel::OBB lampCollider;
			glm::mat4 modelMatrixColliderLamp = glm::mat4(1.0);
			modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp, LampPlantPostion[i]);
			modelMatrixColliderLamp = glm::rotate(modelMatrixColliderLamp, glm::radians(LampPlantOrientation[i]),
					glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "lamp1-" + std::to_string(i), lampCollider, modelMatrixColliderLamp);
			// Set the orientation of collider before doing the scale
			lampCollider.u = glm::quat_cast(modelMatrixColliderLamp);
			modelMatrixColliderLamp = glm::scale(modelMatrixColliderLamp, glm::vec3(1.0));
			modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp, modelLampPlant1.getObb().c);
			lampCollider.c = glm::vec3(modelMatrixColliderLamp[3]);
			lampCollider.e = modelLampPlant1.getObb().e * glm::vec3(1.0);
			std::get<0>(collidersOBB.find("lamp1-" + std::to_string(i))->second) = lampCollider;
		}

		// Lamps2 colliders
		for (int i = 0; i < LampPlantPostion2.size(); i++){
			AbstractModel::OBB lampCollider;
			glm::mat4 modelMatrixColliderLamp = glm::mat4(1.0);
			modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp, LampPlantPostion2[i]);
			modelMatrixColliderLamp = glm::rotate(modelMatrixColliderLamp, glm::radians(LampPlant2Orientation[i]),
					glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "lamp2-" + std::to_string(i), lampCollider, modelMatrixColliderLamp);
			// Set the orientation of collider before doing the scale
			lampCollider.u = glm::quat_cast(modelMatrixColliderLamp);
			modelMatrixColliderLamp = glm::scale(modelMatrixColliderLamp, glm::vec3(1.0, 1.0, 1.0));
			modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp, modelLampPlant2.getObb().c);
			lampCollider.c = glm::vec3(modelMatrixColliderLamp[3]);
			lampCollider.e = modelLampPlant2.getObb().e * glm::vec3(1.0, 1.0, 1.0);
			std::get<0>(collidersOBB.find("lamp2-" + std::to_string(i))->second) = lampCollider;
		}

		// Collider de Protagonista
		AbstractModel::OBB ProtgonistCollider;
		glm::mat4 modelmatrixColliderProtagonist = glm::mat4(modelMatrixProtagonist);
		modelmatrixColliderProtagonist = glm::rotate(modelmatrixColliderProtagonist,
				glm::radians(-90.0f), glm::vec3(1, 0, 0));
		// Set the orientation of collider before doing the scale
		ProtgonistCollider.u = glm::quat_cast(modelmatrixColliderProtagonist);
		modelmatrixColliderProtagonist = glm::scale(modelmatrixColliderProtagonist,glm::vec3(1.0));
		modelmatrixColliderProtagonist = glm::translate(modelmatrixColliderProtagonist,
				glm::vec3(ProtagonistModelAnimate.getObb().c.x,
						ProtagonistModelAnimate.getObb().c.y,
						ProtagonistModelAnimate.getObb().c.z));
		ProtgonistCollider.e = ProtagonistModelAnimate.getObb().e * glm::vec3(1.0) * glm::vec3(1.0);
		ProtgonistCollider.c = glm::vec3(modelmatrixColliderProtagonist[3]);
		addOrUpdateColliders(collidersOBB, "Protagonist", ProtgonistCollider, modelMatrixProtagonist);

		/*******************************************
		 * Render de colliders
		 *******************************************/
		for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
				collidersOBB.begin(); it != collidersOBB.end(); it++) {
			glm::mat4 matrixCollider = glm::mat4(1.0);
			matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
			matrixCollider = matrixCollider * glm::mat4(std::get<0>(it->second).u);
			matrixCollider = glm::scale(matrixCollider, std::get<0>(it->second).e * 2.0f);
			boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
			boxCollider.enableWireMode();
			boxCollider.render(matrixCollider);
		}

		for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.begin(); it != collidersSBB.end(); it++) {
			glm::mat4 matrixCollider = glm::mat4(1.0);
			matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
			matrixCollider = glm::scale(matrixCollider, glm::vec3(std::get<0>(it->second).ratio * 2.0f));
			sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
			sphereCollider.enableWireMode();
			sphereCollider.render(matrixCollider);
		}

		/**********Render de transparencias***************/
		renderAlphaScene();

		/*********************Prueba de colisiones****************************/
		for (std::map<std::string,
			std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4>>::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4>>::iterator jt =
				collidersSBB.begin(); jt != collidersSBB.end(); jt++) {
				if (it != jt && testSphereSphereIntersection(
					std::get<0>(it->second), std::get<0>(jt->second))) {
					std::cout << "Hay collision entre " << it->first <<
						" y el modelo " << jt->first << std::endl;
					isCollision = true;
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		for (std::map<std::string,
			std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>>::iterator it =
			collidersOBB.begin(); it != collidersOBB.end(); it++) {
			bool isColision = false;
			for (std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>>::iterator jt =
				collidersOBB.begin(); jt != collidersOBB.end(); jt++) {
				if (it != jt && 
					testOBBOBB(std::get<0>(it->second), std::get<0>(jt->second))) {
					std::cout << "Hay colision entre " << it->first << " y el modelo" <<
						jt->first << std::endl;
					isColision = true;
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isColision);
		}

		for (std::map<std::string,
			std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4>>::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>>::iterator jt =
				collidersOBB.begin(); jt != collidersOBB.end(); jt++) {
				if (testSphereOBox(std::get<0>(it->second), std::get<0>(jt->second))) {
					std::cout << "Hay colision del " << it->first << " y el modelo" <<
						jt->first << std::endl;
					isCollision = true;
					addOrUpdateCollisionDetection(collisionDetection, jt->first, true);
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		std::map<std::string, bool>::iterator itCollision;
		for (itCollision = collisionDetection.begin(); 
			itCollision != collisionDetection.end(); itCollision++) {
			std::map<std::string, std::tuple<AbstractModel::SBB, 
				glm::mat4, glm::mat4>>::iterator sbbBuscado = 
				collidersSBB.find(itCollision->first);
			std::map<std::string, std::tuple<AbstractModel::OBB,
				glm::mat4, glm::mat4>>::iterator obbBuscado =
				collidersOBB.find(itCollision->first);
			if (sbbBuscado != collidersSBB.end()) {
				if (!itCollision->second) 
					addOrUpdateColliders(collidersSBB, itCollision->first);
			}
			if (obbBuscado != collidersOBB.end()) {
				if (!itCollision->second) 
					addOrUpdateColliders(collidersOBB, itCollision->first);
				else {
					if (itCollision->first.compare("Protagonist") == 0)
						modelMatrixProtagonist = std::get<1>(obbBuscado->second);
					if (itCollision->first.compare("Enemy") == 0){} //use to meke them stop
						//modelMatrixDart = std::get<1>(obbBuscado->second);
				}
			}
		}

		glm::mat4 modelMatrixRayMay = glm::mat4(modelMatrixProtagonist);
		modelMatrixRayMay = glm::translate(modelMatrixRayMay, glm::vec3(0, 1, 0));
		float maxDistanceRay = 10.0;
		glm::vec3 rayDirection = modelMatrixRayMay[2];
		glm::vec3 ori = modelMatrixRayMay[3];
		glm::vec3 rmd = ori + rayDirection * (maxDistanceRay / 2.0f);
		glm::vec3 targetRay = ori + rayDirection * maxDistanceRay;
		modelMatrixRayMay[3] = glm::vec4(rmd, 1.0);
		modelMatrixRayMay = glm::rotate(modelMatrixRayMay, glm::radians(180.0f), 
			glm::vec3(0, 1, 0));
		modelMatrixRayMay = glm::scale(modelMatrixRayMay, 
			glm::vec3(1.0, maxDistanceRay,1.0));
		rayModel.render(modelMatrixRayMay);

		std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4>>::
			iterator itSBB;
		for (itSBB = collidersSBB.begin(); itSBB != collidersSBB.end(); itSBB++) {
			float tRint;
			if (raySphereIntersect(ori, targetRay, rayDirection,
				std::get<0>(itSBB->second), tRint)) {
				std::cout << "Collision del rayo con el modelo " << itSBB->first 
				<< std::endl;
			}
		}
		/*std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>>::
			iterator itOBB;
		for (itOBB = collidersOBB.begin(); itOBB != collidersOBB.end(); itOBB++) {
			if (testRayOBB(ori, targetRay, std::get<0>(itOBB->second))) {
				std::cout << "Collision del rayo con el modelo " << itOBB->first
					<< std::endl;
			}
		}*/

		// Esto es para ilustrar la transformacion inversa de los coliders
		/*glm::vec3 cinv = glm::inverse(ProtgonistCollider.u) * glm::vec4(rockCollider.c, 1.0);
		glm::mat4 invColliderS = glm::mat4(1.0);
		invColliderS = glm::translate(invColliderS, cinv);
		invColliderS =  invColliderS * glm::mat4(ProtgonistCollider.u);
		invColliderS = glm::scale(invColliderS, glm::vec3(rockCollider.ratio * 2.0, rockCollider.ratio * 2.0, rockCollider.ratio * 2.0));
		sphereCollider.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
		sphereCollider.enableWireMode();
		sphereCollider.render(invColliderS);
		glm::vec3 cinv2 = glm::inverse(ProtgonistCollider.u) * glm::vec4(ProtgonistCollider.c, 1.0);
		glm::mat4 invColliderB = glm::mat4(1.0);
		invColliderB = glm::translate(invColliderB, cinv2);
		invColliderB = glm::scale(invColliderB, ProtgonistCollider.e * 2.0f);
		boxCollider.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
		boxCollider.enableWireMode();
		boxCollider.render(invColliderB);
		// Se regresa el color blanco
		sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
		boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));*/
		
		// Para salvar los keyframes
		
		/**********Maquinas de estado*************/
		
		//animationProtagonistIndex = IndexAnimationIdle;

		glfwSwapBuffers(window);

		/****************************+
		 * Open AL sound data
		 */
		source0Pos[0] = modelMatrixColliderRock[3].x;
		source0Pos[1] = modelMatrixColliderRock[3].y;
		source0Pos[2] = modelMatrixColliderRock[3].z;
		alSourcefv(source[0], AL_POSITION, source0Pos);

		source1Pos[0] = modelMatrixAircraft[3].x;
		source1Pos[1] = modelMatrixAircraft[3].y;
		source1Pos[2] = modelMatrixAircraft[3].z;
		alSourcefv(source[1], AL_POSITION, source1Pos);

		// Listener for the Thris person camera
		listenerPos[0] = modelMatrixProtagonist[3].x;
		listenerPos[1] = modelMatrixProtagonist[3].y;
		listenerPos[2] = modelMatrixProtagonist[3].z;
		alListenerfv(AL_POSITION, listenerPos);

		glm::vec3 upModel = glm::normalize(modelMatrixProtagonist[1]);
		glm::vec3 frontModel = glm::normalize(modelMatrixProtagonist[2]);

		listenerOri[0] = frontModel.x;
		listenerOri[1] = frontModel.y;
		listenerOri[2] = frontModel.z;
		listenerOri[3] = upModel.x;
		listenerOri[4] = upModel.y;
		listenerOri[5] = upModel.z;

		// Listener for the First person camera
		// listenerPos[0] = camera->getPosition().x;
		// listenerPos[1] = camera->getPosition().y;
		// listenerPos[2] = camera->getPosition().z;
		// alListenerfv(AL_POSITION, listenerPos);
		// listenerOri[0] = camera->getFront().x;
		// listenerOri[1] = camera->getFront().y;
		// listenerOri[2] = camera->getFront().z;
		// listenerOri[3] = camera->getUp().x;
		// listenerOri[4] = camera->getUp().y;
		// listenerOri[5] = camera->getUp().z;
		alListenerfv(AL_ORIENTATION, listenerOri);

		for(unsigned int i = 0; i < sourcesPlay.size(); i++){
			if(sourcesPlay[i]){
				sourcesPlay[i] = false;
				alSourcePlay(source[i]);
			}
		}

		// Animation Machine States
		rotNave+=0.02; 
			if(!GoUpNave){ 
				NaveStep = abs(NaveStep)*-1; 
				currentNaveHeight+=NaveStep; 
				if(currentNaveHeight <0){ 
					GoUpNave=true; 
				} 
			}else if(GoUpNave){ 
				NaveStep = abs(NaveStep); 
				currentNaveHeight+=NaveStep; 
				if(currentNaveHeight > LimitNaveHeight){ 
					GoUpNave=false; 
				} 
			}

	}
}

int main(int argc, char **argv) {
	//Conversin de coordenadas GIMP a open GL
	for (int i=0;i < LampPlantPostion.size();i++){
		LampPlantPostion[i] = TransformGIMPCoordenatesToOpenGLPixels(LampPlantPostion[i]);
		LampPlantPostion2[i] = TransformGIMPCoordenatesToOpenGLPixels(LampPlantPostion2[i]);
	}
	//std::cout << "Vector: (" << LampPlantPostion[0].x << ", " << LampPlantPostion[0].y << ", " << LampPlantPostion[0].z << ")" << std::endl;

	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}