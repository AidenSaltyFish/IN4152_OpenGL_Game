#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>  
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
// #include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

#include "SceneController.hpp"

constexpr float MouseSensitivity = 0.003; // mouse move sensitivity

enum GameStatus { MenuScene, GameScene };

FPSCamera* cam;

static int mouseLastPosX = 0;
static int mouseLastPosY = 0;

static float windowWidth = 600;
static float windowHeight = 600;

GLuint texture[10];
Shader boxShader;

Shader modelShader;
Shader shadowShader;

// Shadow mapping
const int SHADOWTEX_WIDTH = 1024;
const int SHADOWTEX_HEIGHT = 1024;
GLuint texShadow;
GLuint framebuffer;

constexpr float fov = glm::pi<float>() / 4.0f;
constexpr float aspect = static_cast<float>(600) / static_cast<float>(600);
glm::mat4 projectionMatrix = glm::perspective(fov, aspect, -10.0f, 30.0f);
// glm::mat4 projectionMatrix = glm::ortho(0.0f, windowWidth, windowHeight, 0.0f, -10.0f, 30.0f);

struct Light {
	glm::vec3 position;
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
} light;

void drawShadowScene() {
	// Bind the off-screen framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Clear the shadow map and set needed options
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	shadowShader.Use();

	// Set viewport size
	glViewport(0, 0, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f,3.0f, 3.0f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -10.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(shadowShader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	// glUniformMatrix4fv(glGetUniformLocation(shadowShader.Program, "projMatrix"), 1, GL_FALSE, glm::value_ptr(light.projectionMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shadowShader.Program, "projMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shadowShader.Program, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(light.viewMatrix));

	// house
	drawHouseModels(cam, shadowShader);

	// apple
	//drawAppleModels(shadowShader);
	//playAppleEatenEffect(cam);

	// slime
	// drawSlimeModels(cam, shadowShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void drawNormalScene() {
	modelShader.Use();

	glViewport(0, 0, windowWidth, windowHeight);

	glActiveTexture(GL_TEXTURE0 + 10);
	glBindTexture(GL_TEXTURE_2D, texShadow);

	glUniform1i(glGetUniformLocation(modelShader.Program, "shadowMap"), GL_TEXTURE0 + 10);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f, 3.0f, 3.0f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -10.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "projMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	// glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "projMatrix"), 1, GL_FALSE, glm::value_ptr(cam->projectionMatrix));
	glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(cam->viewMatrix));

	glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "light.projectionMatrix"), 1, GL_FALSE, glm::value_ptr(light.projectionMatrix));
	glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "light.viewMatrix"), 1, GL_FALSE, glm::value_ptr(light.viewMatrix));
	glUniform3fv(glGetUniformLocation(modelShader.Program, "light.position"), 1, glm::value_ptr(light.position));
	glUniform3fv(glGetUniformLocation(modelShader.Program, "light.ambientColor"), 1, glm::value_ptr(light.ambientColor));
	glUniform3fv(glGetUniformLocation(modelShader.Program, "light.diffuseColor"), 1, glm::value_ptr(light.diffuseColor));
	glUniform3fv(glGetUniformLocation(modelShader.Program, "light.specularColor"), 1, glm::value_ptr(light.specularColor));

	glUniform3fv(glGetUniformLocation(modelShader.Program, "viewPos"), 1, glm::value_ptr(cam->cameraPos));

	// house
	drawHouseModels(cam, modelShader);

	// apple
	//drawAppleModels(modelShader);
	//playAppleEatenEffect(cam);

	// slime
	// drawSlimeModels(cam, modelShader);

	// ---------------------------------- //
	//defaultShader.bind();

	//glfwGetFramebufferSize(window.windowPointer(), &framebufferWidth, &framebufferHeight);
	//glViewport(0, 0, windowWidth, windowHeight);
	//glViewport(0, 0, framebufferWidth, framebufferHeight);

	//// Bind the shadow map to texture slot 0            
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, texShadow);

	//defaultShader.uniformMatrix4f("projMatrix", game.projMatrix);
	//defaultShader.uniformMatrix4f("viewMatrix", game.characterViewMatrix);

	//defaultShader.uniformMatrix4f("light.projectionMatrix", light.projectionMatrix);
	//defaultShader.uniformMatrix4f("light.viewMatrix", light.viewMatrix);
	//defaultShader.uniform3f("light.position", light.position);
	//defaultShader.uniform3f("light.ambientColor", light.ambientColor);
	//defaultShader.uniform3f("light.diffuseColor", light.diffuseColor);
	//defaultShader.uniform3f("light.specularColor", light.specularColor);

	//defaultShader.uniform3f("viewPos", cameraPos);
	//defaultShader.uniform1i("turboModeOn", game.turboModeOn);

	// // even older
	//glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	//glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//glUniform3fv(glGetUniformLocation(modelShader.Program, "lightPos1"), 1, LightPosition);
	//glUniform3fv(glGetUniformLocation(modelShader.Program, "lightPos2"), 1, LightPosition2);
	//glUniform3fv(glGetUniformLocation(modelShader.Program, "lightPos3"), 1, LightPosition3);
	//glUniform3fv(glGetUniformLocation(modelShader.Program, "lightPos4"), 1, LightPosition4);
	//glUniform3fv(glGetUniformLocation(modelShader.Program, "viewPos_in"), 1, glm::value_ptr(cam->cameraPos));
}

void drawScene()
{
	//light.position = cam->cameraPos;

	//light.ambientColor = glm::vec3{ 0.7f, 0.7f, 0.7f };
	//light.diffuseColor = glm::vec3{ 0.8f, 0.8f, 0.8f };
	//light.specularColor = glm::vec3{ 1.0f, 1.0f, 1.0f };

	//light.viewMatrix = glm::lookAt(light.position, cam->targetPos, glm::vec3(0.0f, 1.0f, 0.0f));

	float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
	projectionMatrix = glm::perspective(fov, aspect, -10.0f, 30.0f);
	
	// shadow
	drawShadowScene();

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// normal scene
	drawNormalScene();

	// glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glColor3f(1, 1, 1);

	//文字
	drawGameSceneUIText(cam, 50, 550);

	// box
	boxShader.Use();
	glStencilMask(0x00);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	// drawBoxColliders(boxShader, texture[7], texture[8], texture[9], cam);	//Modern GL
	glDisable(GL_CULL_FACE);
	glUseProgram(NULL);

	//天空盒
	glStencilMask(0x00);
	Skybox::drawSkybox(texture);

	//地板  
	glPushMatrix();
	//glStencilMask(0x00);
	glTranslatef(0.0f, -roomSizeY / 2.0f, 0.0f);
	glRotatef(90, 1, 0, 0);
	glScalef(roomSizeX, roomSizeZ, 1);
	//Draw::drawRect(texture[0]);
	glPopMatrix();

	cam->updateCameraMovement();
	detectAppleBeingEaten(cam);
}

void reshape(int width, int height)
{
	cam->resetWinSize(width, height);    
	windowWidth = width;
	windowHeight = height;
}

void idle()
{
	glutPostRedisplay();
}

void initShadowMap() {
	//glEnable(GL_DEPTH_TEST);
	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	//////////////////// SHADOW MAP
	//// Create Shadow Texture
	//glGenTextures(1, &texShadow);
	//glActiveTexture(GL_TEXTURE0 + 10);
	//glBindTexture(GL_TEXTURE_2D, texShadow);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	//// Set behaviour for when texture coordinates are outside the [0, 1] range
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//// Set interpolation for texture sampling (GL_NEAREST for no interpolation)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

	////// Create framebuffer for extra texture
	//glGenFramebuffers(1, &framebuffer);

	////// Set shadow texure as depth buffer for this framebuffer
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texShadow, 0);
	//glClearDepth(1.0f);
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glActiveTexture(GL_TEXTURE0);

	glCreateTextures(GL_TEXTURE_2D, 1, &texShadow);
	glTextureStorage2D(texShadow, 1, GL_DEPTH_COMPONENT32F, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);

	// Set behaviour for when texture coordinates are outside the [0, 1] range.
	glTextureParameteri(texShadow, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texShadow, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Set interpolation for texture sampling (GL_NEAREST for no interpolation).
	glTextureParameteri(texShadow, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texShadow, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// === Create framebuffer for extra texture ===
	glCreateFramebuffers(1, &framebuffer);
	glNamedFramebufferTexture(framebuffer, GL_DEPTH_ATTACHMENT, texShadow, 0);
}

void initLight() {
	//GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	//GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//GLfloat LightPosition[] = { 0.0f, SkyboxSize / 2.f, 0.f, 1.0f };
	//GLfloat LightPosition2[] = { -SkyboxSize / 2.f, -SkyboxSize / 2.f, SkyboxSize / 2.f, 1.0f };
	//GLfloat LightPosition3[] = { SkyboxSize / 2.f, -SkyboxSize / 2.f, SkyboxSize / 2.f, 1.0f };
	//GLfloat LightPosition4[] = { 0.0f, -SkyboxSize / 2.f, -SkyboxSize / 2.f, 1.0f };

	light.position = glm::vec3{ 0.0f, /*SkyboxSize / 5.0f*/-5.0f, 0.0f };

	light.ambientColor = glm::vec3{ 0.7f, 0.7f, 0.7f };
	light.diffuseColor = glm::vec3{ 0.8f, 0.8f, 0.8f };
	light.specularColor = glm::vec3{ 1.0f, 1.0f, 1.0f };

	light.viewMatrix = glm::lookAt(light.position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	GLfloat near_plane = 0.5f, far_plane = 7.5f;
	light.projectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	// light.projectionMatrix = projectionMatrix;
}

void initTexture()
{
	glEnable(GL_DEPTH_TEST);
	glGenTextures(10, texture);
	loadTex(0, "Textures/17.bmp", texture);

	//skybox
	loadTex(1, "Textures/Skybox1/up.png", texture);
	loadTex(2, "Textures/Skybox1/down.png", texture);
	loadTex(3, "Textures/Skybox1/left.png", texture);
	loadTex(4, "Textures/Skybox1/right.png", texture);
	loadTex(5, "Textures/Skybox1/front.png", texture);
	loadTex(6, "Textures/Skybox1/back.png", texture);

	boxShader.Use();
	loadTex(7, "Textures/19d.bmp", texture);	//Box Diffuse
	loadTex(8, "Textures/20b.bmp", texture);	//Box Bump
	loadTex(9,"Textures/21s.bmp", texture);		//Box Specular
	glUseProgram(NULL);
}

#pragma region UserInterface

void normalKeyPress(unsigned char key, int x, int y)
{
	cam->keyPressed(key);
}

void normalKeyUp(unsigned char key, int x, int y)
{
	cam->keyUp(key);
}

void mouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouseLastPosY = y;
		mouseLastPosX = x;
	}
}

void mouseMove(int x, int y)
{
	float pitch = (float)(mouseLastPosY - y) * MouseSensitivity;
	float yaw = (float)(mouseLastPosX - x) * MouseSensitivity;
	mouseLastPosY = y;
	mouseLastPosX = x;
	cam->rotate(pitch, yaw);
}

#pragma endregion

void redraw()
{
	//Redraw only runs for every 10ms
	static auto last_time = std::chrono::system_clock::now();
	std::this_thread::sleep_until(last_time + std::chrono::milliseconds(10));
	last_time = std::chrono::system_clock::now();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	drawScene();
	glutSwapBuffers();
}

void initializeGL()
{
	glewInit();
	glewExperimental = GL_TRUE;

	cam = new FPSCamera();
	//添加碰撞边缘
	cam->setSceneOuterBoundary(-roomSizeX / 2.0, -roomSizeZ / 2.0, roomSizeX / 2.0, roomSizeZ / 2.0);

	cam->setSceneInnerBoundary(-roomSizeX / 2.0, -roomSizeY / 2.0f - 1.f, -roomSizeZ / 2.0,	roomSizeX / 2.0, -roomSizeY / 2.0f, roomSizeZ / 2.0);    //地板collider

	// initBoxCollidersProperty();
	// setBoxColliderBoundary(cam);
	initShadowMap();
	initLight();

	initAppleNum(1);
	
	initSlimeModels();
	initAppleModels();
	initHouseModels();
	initArrowModels();
	
	//setupLights();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	boxShader.load("shader.vert", "shader.frag");
	modelShader.load("ModelShader_vert.glsl", "ModelShader_frag.glsl");
	shadowShader.load("ShadowShader_vert.glsl", "ShadowShader_frag.glsl");
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);    
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);
	glutInitWindowPosition(250, 100);
	glutInitWindowSize(600, 600);
	int windowHandle = glutCreateWindow("IN4152 3D Computer Graphics and Animation final project");

	initializeGL();

	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(normalKeyPress);
	glutKeyboardUpFunc(normalKeyUp);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	glutIdleFunc(idle);

	boxShader.Use();
	initCube(boxShader);
	glUseProgram(NULL);

	initTexture();
	glutMainLoop();

	delete cam;
	//deleteAppleModels();
	//deleteArrowModels();
	deleteHouseModels();
	deleteSlimeModels();

	return 0;
}