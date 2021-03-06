#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <soil\SOIL.h>

#include "Model.hpp"
// #include "FPSCamera.hpp"
#include "ParticleSystem.hpp"
// #include "Shader.hpp"

constexpr float roomSizeX = 200.0f;
constexpr float roomSizeY = 60.0f;
constexpr float roomSizeZ = 200.0f;

constexpr float SkyboxSize = 1000.0f;

constexpr float EatAppleDistance = 5.0f;
constexpr float CloseToAppleDistance = 200.0f;

GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[] = { 0.0f, SkyboxSize / 2.f, 0.f, 1.0f };
GLfloat LightPosition2[] = { -SkyboxSize / 2.f, -SkyboxSize / 2.f, SkyboxSize / 2.f, 1.0f };
GLfloat LightPosition3[] = { SkyboxSize / 2.f, -SkyboxSize / 2.f, SkyboxSize / 2.f, 1.0f };
GLfloat LightPosition4[] = { 0.0f, -SkyboxSize / 2.f, -SkyboxSize / 2.f, 1.0f };

extern Shader modelShader;
extern Shader shadowShader;
extern FPSCamera* cam;

vector<glm::vec3> boxPosition;
vector<glm::vec3> boxScale;
static int boxSum = 0;

int appleNum;
vector<Model*> appleSet;
vector<ParticleSystem> appleEatenEffectSet;
vector<bool> isAppleEatenSet;
static int eatenAppleNum = 0;
static int closeToAppleIndex = -1;

vector<arrowModel*> arrowSet;
vector<solidModel*> houseSet;
vector<solidModel*> oakTreeSet;

Model* slime;
static int slimeFrame = 0;

vector<Model*> slimeSet;

static float angle = 0.0f;

void loadTex(int i, char *filename, GLuint* texture)
{
	unsigned char* bitmapData;

	//bitmapData = LoadBitmapFile(filename, &bitmapInfoHeader);
	int width, height;
	bitmapData = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, texture[i]);
	// 指定当前纹理的放大/缩小过滤方式   
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,
		0,         //mipmap层次(通常为，表示最上层)     
		GL_RGB,    //我们希望该纹理有红、绿、蓝数据    
		width, //纹理宽带，必须是n，若有边框+2     
		height, //纹理高度，必须是n，若有边框+2     
		0, //边框(0=无边框, 1=有边框)     
		GL_RGB,    //bitmap数据的格式    
		GL_UNSIGNED_BYTE, //每个颜色数据的类型    
		bitmapData);    //bitmap数据指针    

	SOIL_free_image_data(bitmapData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

class Draw
{
public:
	static inline void drawRect(GLuint texture)
	{
		glEnable(GL_TEXTURE_2D);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);    //天空盒加环境光
		glEnable(GL_COLOR_MATERIAL);

		glBindTexture(GL_TEXTURE_2D, texture);  //选择纹理texture[status]   
		const GLfloat x1 = -0.5, x2 = 0.5;
		const GLfloat y1 = -0.5, y2 = 0.5;
		const GLfloat point[4][2] = { { x1,y1 },{ x1,y2 },{ x2,y2 },{ x2,y1 } };
		int dir[4][2] = { { 1,0 },{ 1,1 },{ 0,1 },{ 0,0 } };
		glBegin(GL_QUADS);

		for (int i = 0; i < 4; i++) {
			glTexCoord2iv(dir[i]);
			glVertex2fv(point[i]);
		}
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_TEXTURE_2D);
	}
};

class Skybox
{
public:
	static void drawSkybox(GLuint* texture)
	{
		//up
		glPushMatrix();
		glTranslatef(0.0f, SkyboxSize / 2.0f, 0.0f);
		glRotatef(270, 1, 0, 0);

		glScalef(SkyboxSize, SkyboxSize, 1);
		Draw::drawRect(texture[1]);
		glPopMatrix();

		//down
		glPushMatrix();
		glTranslatef(0.0f, -SkyboxSize / 2.0f, 0.0f);
		glRotatef(90, 1, 0, 0);

		glScalef(SkyboxSize, SkyboxSize, 1);
		Draw::drawRect(texture[2]);
		glPopMatrix();

		//left
		glPushMatrix();
		glTranslatef(-SkyboxSize / 2.0f, 0.0f, 0.0f);
		glRotatef(270, 0, 1, 0);
		glRotatef(180, 0, 0, 1);

		glScalef(SkyboxSize, SkyboxSize, 1);
		Draw::drawRect(texture[3]);
		glPopMatrix();

		//right
		glPushMatrix();
		glTranslatef(SkyboxSize / 2.0f, 0.0f, 0.0f);
		glRotatef(90, 0, 1, 0);
		glRotatef(180, 0, 0, 1);

		glScalef(SkyboxSize, SkyboxSize, 1);
		Draw::drawRect(texture[4]);
		glPopMatrix();

		//front
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, -SkyboxSize / 2.0);
		glRotatef(180, 1, 0, 0);

		glScalef(SkyboxSize, SkyboxSize, 1);
		Draw::drawRect(texture[5]);
		glPopMatrix();

		//back
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, SkyboxSize / 2.0f);
		glRotatef(180, 0, 0, 1);

		glScalef(SkyboxSize, SkyboxSize, 1);
		Draw::drawRect(texture[6]);
		glPopMatrix();
	}

private:
	
	
};

//struct Vertex
//{
//	glm::vec3 pos;
//	glm::vec3 norm;
//	glm::vec2 texC;
//	glm::vec3 tangent;
//	glm::vec3 bitangent;
//
//	Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 t) : pos(p), norm(n), texC(t){}
//};

GLuint CubeVAO, CubeVBO, CubeEBO;
vector<Vertex> CubeVertices;
vector<unsigned int> CubeIndices =
{
	0, 1, 2, 0, 2 ,3,		//Front
	4, 5, 6, 4, 6, 7,		//Right
	8, 9, 10, 8 ,10, 11,	//Back
	12, 13, 14, 12, 14, 15,	//Left
	16, 17, 18, 16, 18, 19,	//Top
	20, 21, 22, 20, 22, 23	//Bottom
};

void drawCube(GLuint texture)
{
	glEnable(GL_TEXTURE_2D);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);    //盒子碰撞器加环境光
	glEnable(GL_COLOR_MATERIAL);
	int i, j;
	const GLfloat x1 = -0.5, x2 = 0.5;
	const GLfloat y1 = -0.5, y2 = 0.5;
	const GLfloat z1 = -0.5, z2 = 0.5;

	//指定六个面的四个顶点，每个顶点用3个坐标值表示
	//前 后 上 下 左 右  

	GLfloat point[6][4][3] = {
		{ { x1,y1,z1 },{ x1,y2,z1 },{ x2,y2,z1 },{ x2,y1,z1 } },
		{ { x1,y1,z2 },{ x2,y1,z2 },{ x2,y2,z2 },{ x1,y2,z2 } },
		{ { x1,y2,z1 },{ x1,y2,z2 },{ x2,y2,z2 },{ x2,y2,z1 } },
		{ { x1,y1,z1 },{ x2,y1,z1 },{ x2,y1,z2 },{ x1,y1,z2 } },
		{ { x2,y1,z1 },{ x2,y2,z1 },{ x2,y2,z2 },{ x2,y1,z2 } },
		{ { x1,y1,z1 },{ x1,y1,z2 },{ x1,y2,z2 },{ x1,y2,z1 } },
	};

	int dir[6][4][2] = {
		{ { 0,0 },{ 0,1 },{ 1,1 },{ 1,0 } },
		{ { 0,0 },{ 1,0 },{ 1,1 },{ 0,1 } },
		{ { 0,1 },{ 0,0 },{ 1,0 },{ 1,1 } },
		{ { 1,1 },{ 0,1 },{ 0,0 },{ 1,0 } },
		{ { 1,0 },{ 1,1 },{ 0,1 },{ 0,0 } },
		{ { 0,0 },{ 1,0 },{ 1,1 },{ 0,1 } },
	};

	for (i = 0; i < 6; i++) {
		glm::vec3 v1(point[i][0][0], point[i][0][1], point[i][0][2]);
		glm::vec3 v2(point[i][1][0], point[i][1][1], point[i][1][2]);
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUADS);
		for (j = 0; j < 4; j++) {
			glTexCoord2iv(dir[i][j]);
			glNormal3fv(glm::value_ptr(normal));
			glVertex3fv(point[i][j]);
		}
		glEnd();
	}

	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);
}

void initCube(Shader shader)
{
	const GLfloat x = 0.5;
	const GLfloat y = 0.5;
	const GLfloat z = 0.5;

	//Front
	CubeVertices.push_back(Vertex(glm::vec3(x, y, z), glm::vec3(0, 0, 1), glm::vec2(1, 0)));
	CubeVertices.push_back(Vertex(glm::vec3(x, -y, z), glm::vec3(0, 0, 1), glm::vec2(1, 1)));
	CubeVertices.push_back(Vertex(glm::vec3(-x, -y, z), glm::vec3(0, 0, 1), glm::vec2(0, 1)));
	CubeVertices.push_back(Vertex(glm::vec3(-x, y, z), glm::vec3(0, 0, 1), glm::vec2(0, 0)));

	//Right
	CubeVertices.push_back(Vertex(glm::vec3(x, y, z), glm::vec3(1, 0, 0), glm::vec2(0, 0)));
	CubeVertices.push_back(Vertex(glm::vec3(x, y, -z), glm::vec3(1, 0, 0), glm::vec2(1, 0)));
	CubeVertices.push_back(Vertex(glm::vec3(x, -y, -z), glm::vec3(1, 0, 0), glm::vec2(1, 1)));
	CubeVertices.push_back(Vertex(glm::vec3(x, -y, z), glm::vec3(1, 0, 0), glm::vec2(0, 1)));

	//Top
	CubeVertices.push_back(Vertex(glm::vec3(x, y, z), glm::vec3(0, 1, 0), glm::vec2(0, 0)));
	CubeVertices.push_back(Vertex(glm::vec3(-x, y, z), glm::vec3(0, 1, 0), glm::vec2(0, 1)));
	CubeVertices.push_back(Vertex(glm::vec3(-x, y, -z), glm::vec3(0, 1, 0), glm::vec2(1, 1)));
	CubeVertices.push_back(Vertex(glm::vec3(x, y, -z), glm::vec3(0, 1, 0), glm::vec2(1, 0)));

	//Back
	CubeVertices.push_back(Vertex(glm::vec3(-x, -y, -z), glm::vec3(0, 0, -1), glm::vec2(1, 0)));
	CubeVertices.push_back(Vertex(glm::vec3(x, -y, -z), glm::vec3(0, 0, -1), glm::vec2(0, 0)));
	CubeVertices.push_back(Vertex(glm::vec3(x, y, -z), glm::vec3(0, 0, -1), glm::vec2(0, 1)));
	CubeVertices.push_back(Vertex(glm::vec3(-x, y, -z), glm::vec3(0, 0, -1), glm::vec2(1, 1)));

	//Left
	CubeVertices.push_back(Vertex(glm::vec3(-x, -y, -z), glm::vec3(-1, 0, 0), glm::vec2(0, 0)));
	CubeVertices.push_back(Vertex(glm::vec3(-x, y, -z), glm::vec3(-1, 0, 0), glm::vec2(0, 1)));
	CubeVertices.push_back(Vertex(glm::vec3(-x, y, z), glm::vec3(-1, 0, 0), glm::vec2(1, 1)));
	CubeVertices.push_back(Vertex(glm::vec3(-x, -y, z), glm::vec3(-1, 0, 0), glm::vec2(1, 0)));

	//Bottom
	CubeVertices.push_back(Vertex(glm::vec3(-x, -y, -z), glm::vec3(0, -1, 0), glm::vec2(0, 0)));
	CubeVertices.push_back(Vertex(glm::vec3(-x, -y, z), glm::vec3(0, -1, 0), glm::vec2(1, 0)));
	CubeVertices.push_back(Vertex(glm::vec3(x, -y, z), glm::vec3(0, -1, 0), glm::vec2(1, 1)));
	CubeVertices.push_back(Vertex(glm::vec3(x, -y, -z), glm::vec3(0, -1, 0), glm::vec2(0, 1)));

	//Tangent space stuff
	//for (int i = 0; i < CubeIndices.size(); i += 3)
	//{
	//	Vertex& a = CubeVertices[CubeIndices[i]];
	//	Vertex& b = CubeVertices[CubeIndices[i + 1]];
	//	Vertex& c = CubeVertices[CubeIndices[i + 2]];

	//	glm::vec3 edge1 = b.pos - a.pos;
	//	glm::vec3 edge2 = c.pos - a.pos;
	//	glm::vec2 deltaUV1 = b.texC - a.texC;
	//	glm::vec2 deltaUV2 = c.texC - a.texC;

	//	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	//	glm::vec3 tangent, bitangent;

	//	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	//	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	//	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	//	tangent = glm::normalize(tangent);

	//	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	//	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	//	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	//	bitangent = glm::normalize(bitangent);

	//	a.tangent = tangent;
	//	b.tangent = tangent;
	//	c.tangent = tangent;

	//	a.bitangent = bitangent;
	//	b.bitangent = bitangent;
	//	c.bitangent = bitangent;
	//}

	glGenVertexArrays(1, &CubeVAO);
	glGenBuffers(1, &CubeVBO);
	glGenBuffers(1, &CubeEBO);

	glBindVertexArray(CubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * CubeVertices.size(), &CubeVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * CubeIndices.size(), &CubeIndices[0], GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	//Tex2D
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texC));

	//Tangent
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	////Bitangent
	//glEnableVertexAttribArray(4);
	//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	glBindVertexArray(0);
}

void drawCube(Shader shader, GLuint diffuse, GLuint bump, GLuint spec)	//modern gl draw, init before use.
{
	//Textures
	glActiveTexture(GL_TEXTURE0);	//0th texture unit
	glUniform1i(glGetUniformLocation(shader.Program, "diffuse_map"), 0);	//Bind "diffuse_map" in shader to 0th texture unit.
	glBindTexture(GL_TEXTURE_2D, diffuse);	//Bind texture to texture unit.

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(shader.Program, "bump_map"), 1);
	glBindTexture(GL_TEXTURE_2D, bump);

	glActiveTexture(GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(shader.Program, "spec_map"), 2);
	glBindTexture(GL_TEXTURE_2D, spec);

	//Draw
	glBindVertexArray(CubeVAO);
	glDrawElements(GL_TRIANGLES, CubeIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//Release Textures: in reverse order!
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_TEXTURE0);
}

void initAppleNum(int _appleNum) {
	appleNum = _appleNum;
}

void initSingleBoxCollider(glm::vec3 pos, glm::vec3 scalar)    //设置单个盒子的位置和大小
{
	boxPosition.push_back(pos);
	boxScale.push_back(scalar);
	boxSum++;
	isAppleEatenSet.push_back(false);
}

void initBoxCollidersProperty()                   //设置盒子的位置和大小
{
	//1-0
	initSingleBoxCollider(glm::vec3(-60.f, -1.0f * roomSizeY / 2.0f + 2.5f, 60.f),	glm::vec3(5, 5, 40));//2-1
	initSingleBoxCollider(glm::vec3(-70.f, -1.0f * roomSizeY / 2.0f + 7.5f, 40.f),	glm::vec3(20, 5, 5));//3-2
	initSingleBoxCollider(glm::vec3(-80.f, -1.0f * roomSizeY / 2.0f + 12.5f, 0.f),	glm::vec3(5, 5, 80));//4-1
	initSingleBoxCollider(glm::vec3(-50.f, -1.0f * roomSizeY / 2.0f + 7.5f, -40.f),glm::vec3(60, 5, 5));//5-2
	initSingleBoxCollider(glm::vec3(-20.f, -1.0f * roomSizeY / 2.0f + 12.5f, -20.f),glm::vec3(5, 5, 40));//5.1-3
	initSingleBoxCollider(glm::vec3(-30.f, -1.0f * roomSizeY / 2.0f + 17.5f, 0.f),glm::vec3(20, 5, 5));//6-4
	initSingleBoxCollider(glm::vec3(-40.f, -1.0f * roomSizeY / 2.0f + 22.5f, 10.f),glm::vec3(5, 5, 20));//7-3
	initSingleBoxCollider(glm::vec3(-10.f, -1.0f * roomSizeY / 2.0f + 17.5f, 20.f),glm::vec3(60, 5, 5));//8-4
	initSingleBoxCollider(glm::vec3(20.f, -1.0f * roomSizeY / 2.0f + 22.5f, -30.f),glm::vec3(5, 5, 100));//9-5
	initSingleBoxCollider(glm::vec3(-20.f, -1.0f * roomSizeY / 2.0f + 27.5f, -80.f),glm::vec3(80, 5, 5));//10-3
	initSingleBoxCollider(glm::vec3(50.f, -1.0f * roomSizeY / 2.0f + 17.5f, -80.f),glm::vec3(60, 5, 5));//11-2
	initSingleBoxCollider(glm::vec3(80.f, -1.0f * roomSizeY / 2.0f + 12.5f, -60.f),glm::vec3(5, 5, 40));//12-1
	initSingleBoxCollider(glm::vec3(60.f, -1.0f * roomSizeY / 2.0f + 7.5f, -40.f),	glm::vec3(40, 5, 5));//13-2
	initSingleBoxCollider(glm::vec3(40.f, -1.0f * roomSizeY / 2.0f + 12.5f, 10.f),	glm::vec3(5, 5, 100));//14-3
	initSingleBoxCollider(glm::vec3(20.f, -1.0f * roomSizeY / 2.0f + 17.5f, 60.f),	glm::vec3(40, 5, 5));//15-2
	initSingleBoxCollider(glm::vec3(-20.f, -1.0f * roomSizeY / 2.0f + 12.5f, 60.f),glm::vec3(40, 5, 5));//16-4
	initSingleBoxCollider(glm::vec3(0.f, -1.0f * roomSizeY / 2.0f + 22.5f, 70.f),glm::vec3(5, 5, 20));	//17-5
	initSingleBoxCollider(glm::vec3(40.f, -1.0f * roomSizeY / 2.0f + 27.5f, 80.f),	glm::vec3(80, 5, 5));//18-6
	initSingleBoxCollider(glm::vec3(80.f, -1.0f * roomSizeY / 2.0f + 32.5f, 40.f),	glm::vec3(5, 5, 80));
}

void setBoxColliderBoundary(FPSCamera* cam)       //设置盒子碰撞边缘
{
	for (int i = 0; i < boxPosition.size(); i++)
	{
		cam->setSceneInnerBoundary(
			boxPosition[i].x - boxScale[i].x / 2.f,
			boxPosition[i].y - boxScale[i].y / 2.f,
			boxPosition[i].z - boxScale[i].z / 2.f,
			boxPosition[i].x + boxScale[i].x / 2.f,
			boxPosition[i].y + boxScale[i].y / 2.f,
			boxPosition[i].z + boxScale[i].z / 2.f);
	}
}

void drawBoxColliders(GLuint* texture)            //绘制盒子
{
	for (int i = 0; i < boxPosition.size(); i++)
	{
		glStencilMask(0x00);
		glPushMatrix();
		glTranslatef(boxPosition[i].x, boxPosition[i].y, boxPosition[i].z);
		glScalef(boxScale[i].x, boxScale[i].y, boxScale[i].z);
		drawCube(texture[0]);
		glPopMatrix();
	}
}

void drawBoxColliders(Shader shader, GLuint diffuse, GLuint bump, GLuint spec, FPSCamera* cam)	//Override with modernGL method
{
	glUniform3fv(glGetUniformLocation(shader.Program, "lightPos1"), 1, LightPosition);
	glUniform3fv(glGetUniformLocation(shader.Program, "lightPos2"), 1, LightPosition2);
	glUniform3fv(glGetUniformLocation(shader.Program, "lightPos3"), 1, LightPosition3);
	glUniform3fv(glGetUniformLocation(shader.Program, "lightPos4"), 1, LightPosition4);
	glUniform3fv(glGetUniformLocation(shader.Program, "viewPos"), 1, glm::value_ptr(cam->cameraPos));

	float P[16];
	glGetFloatv(GL_PROJECTION_MATRIX, P);
	//Projection does not change
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, P);

	for (int i = 0; i < boxPosition.size(); i++)
	{
		glm::mat4 boxPos = glm::translate(glm::mat4(1.0), glm::vec3(boxPosition[i].x, boxPosition[i].y, boxPosition[i].z));
		glm::mat4 boxScl = glm::scale(glm::mat4(1.0), glm::vec3(boxScale[i].x, boxScale[i].y, boxScale[i].z));

		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(boxPos * boxScl));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(cam->viewMatrix));

		drawCube(shader, diffuse, bump, spec);
	}
}

void initAppleModels()        //初始化面包集
{
	Model* myAppleModel = new Model;

	if (!myAppleModel->importModel("./ModelRes/Apple/Apple.obj"))
		cout << "Import model error!" << endl;

	for (int i = 0; i < appleNum; i++) {
		appleSet.push_back(myAppleModel);
		isAppleEatenSet.push_back(false);
	}

	float offset = 10.0f;
	//Init Bread Eaten Effect and position
	for (int i = 0; i < appleNum; i++) {
		ParticleSystem eaten(80, GravityAcc);
		eaten.init();
		appleEatenEffectSet.push_back(eaten);

		appleSet[i]->setModelPosition(glm::vec3{ -60.f, -1.0f * roomSizeY / 2.0f + 12.5f, 60.f });
		appleSet[i]->setModelScale(glm::vec3{ 5.0f, 5.0f, 5.0f });
	}
}

void initArrowModels()        
{
	int arrowNum = appleNum;

	arrowModel* myArrowModel = new arrowModel;

	if (!myArrowModel->importModel("./ModelRes/Arrow/Arrow.obj"))
		cout << "Import model error!" << endl;

	for (int i = 0; i < arrowNum; i++) {
		arrowSet.push_back(myArrowModel);
		arrowSet[i]->setModelPosition(appleSet[i]->getModelPosition() + glm::vec3{ 0.0f, 2.0f, 0.0f });
		arrowSet[i]->setModelScale(appleSet[i]->getModelScale());
	}
}

void deleteArrowModels()
{
	for (int i = 0; i < arrowSet.size(); i++)
		delete arrowSet[i];
}

void drawAppleModels(Shader shader)        //绘制面包集
{
	for (int i = 0; i < appleSet.size(); i++)
	{
		if (!isAppleEatenSet[i])
		{
			glPushMatrix();

			// 设置模板缓冲为可写状态，把箭头放入模板缓冲（设为1）
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

			glTranslatef(appleSet[i]->getModelPosition().x, appleSet[i]->getModelPosition().y, appleSet[i]->getModelPosition().z);
			glRotatef(angle, 0.f, 1.f, 0.f);
			glScalef(appleSet[i]->getModelScale().x, appleSet[i]->getModelScale().y, appleSet[i]->getModelScale().z);

			//glTranslatef(boxPosition[i].x, boxPosition[i].y + 12.0f, boxPosition[i].z);
			//glRotatef(angle, 0.f, 1.f, 0.f);
			//glScalef(5.f, 5.f, 5.f);

			appleSet[i]->renderModel(shader);

			glPopMatrix();
		}

		//模板检测，绘制边缘
		if (i == closeToAppleIndex)
		{
			glPushMatrix();

			// 设置模板缓冲为不可写状态，只绘制 != 1 的部分
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);    //边框不会被其他物体覆盖

			glTranslatef(arrowSet[i]->getModelPosition().x, arrowSet[i]->getModelPosition().y, arrowSet[i]->getModelPosition().z);
			glRotatef(angle, 0.f, 1.f, 0.f);
			glScalef(arrowSet[i]->getModelScale().x, arrowSet[i]->getModelScale().y, arrowSet[i]->getModelScale().z);
			arrowSet[i]->renderModel(shader);

			glStencilMask(0xFF);
			glEnable(GL_DEPTH_TEST);

			glPopMatrix();
		}
	}
	angle += 1.5f;
}

void playAppleEatenEffect(FPSCamera* cam)
{
	for (int i = 0; i < appleSet.size(); i++) {
		if (isAppleEatenSet[i]) {
			glPushMatrix();
			glm::vec3 pPos = appleSet[i]->getModelPosition() + cam->getForward() * 10.f;
			glTranslatef(pPos.x, pPos.y, pPos.z);
			appleEatenEffectSet[i].simulate(0.01);
			appleEatenEffectSet[i].render();
			glPopMatrix();
		}
	}
}

void deleteAppleModels()      //销毁面包集
{
	for (int i = 0; i < appleSet.size(); i++)
		delete appleSet[i];
}

void detectAppleBeingEaten(FPSCamera* cam)      //检测面包是否被吃掉
{
	for (int i = 0; i < appleSet.size(); ++i)
	{
		if (!isAppleEatenSet[i])
		{
			glm::vec3 applePos(appleSet[i]->getModelPosition());

			//先检测是否靠近面包
			if (cam->detectPlayerCloseToApple(applePos, CloseToAppleDistance)) {
				closeToAppleIndex = i;

				//再检测是否能吃掉面包
				if (cam->detectPlayerEatingApple(applePos, EatAppleDistance)) {
					isAppleEatenSet[i] = true;
					eatenAppleNum++;
					closeToAppleIndex = -1;
					break;
				}
				break;
			}
			else
				closeToAppleIndex = -1;
		}
	}
}

void initHouseModels()
{
	int houseNum = 1;

	solidModel* myHouseModel = new solidModel;

	// if (!myHouseModel->importModel("ModelRes/Houses/houseSF.obj"))
	//if (!myHouseModel->importModel("./ModelRes/Trees/Fir_Tree.obj"))
	if (!myHouseModel->importModel("./ModelRes/scene.obj"))
		cout << "Import model error!" << endl;

	for (int i = 0; i < houseNum; ++i) {
		houseSet.push_back(myHouseModel);
	}
}

void drawHouseModels(FPSCamera* cam, Shader shader)
{
	for (int i = 0; i < houseSet.size(); i++) {
		glPushMatrix();

		// 设置模板缓冲为可写状态，把箭头放入模板缓冲（设为1）
		//glStencilFunc(GL_ALWAYS, 1, 0xFF);
		//glStencilMask(0xFF);

		houseSet[i]->setModelPosition(glm::vec3{ -60.f, -1.0f * roomSizeY / 2.0f + 32.5f, 60.f });
		// glScalef(5.f, 5.f, 5.f);
		houseSet[i]->setModelScale(glm::vec3{5.0f, 5.0f, 5.0f});

		glTranslatef(houseSet[i]->getModelPosition().x, houseSet[i]->getModelPosition().y, houseSet[i]->getModelPosition().z);

		houseSet[i]->renderModel(shader);

		houseSet[i]->setSolidColliderBoundary(glm::vec3{ 0.5f, 0.5f, 0.5f }, glm::vec3{ 0.5f, 0.5f, 0.5f }, cam);

		glPopMatrix();
	}
}

void deleteHouseModels()
{
	for (int i = 0; i < houseSet.size(); i++)
		delete houseSet[i];
}

void initSlimeModels() {
	int slimeNum = 60;

	for (int i = 1; i <= slimeNum; ++i) {
		Model* mySlimeModel = new Model;

		if (i < 10) {
			if (!mySlimeModel->importModel("./ModelRes/Slime/Slime_00000" + to_string(i) + ".obj")) {
				cout << "Import model error!" << endl;
			}
		}
		else if (i >= 10) {
			if (!mySlimeModel->importModel("./ModelRes/Slime/Slime_0000" + to_string(i) + ".obj")) {
				cout << "Import model error!" << endl;
			}
		}

		slimeSet.push_back(mySlimeModel);
	}
}

void drawSlimeModels(FPSCamera* cam, Shader shader) {
	if (slimeFrame >= 60) slimeFrame = 0;

	glPushMatrix();

	// 设置模板缓冲为可写状态，把箭头放入模板缓冲（设为1）
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);
	//glStencilMask(0xFF);

	slimeSet[slimeFrame]->setModelPosition(glm::vec3{ -30.f, -1.0f * roomSizeY / 2.0f + 5.5f, 10.f });
	// glScalef(5.f, 5.f, 5.f);
	slimeSet[slimeFrame]->setModelScale(glm::vec3{ 5.0f, 5.0f, 5.0f });

	// glTranslatef(slimeSet[slimeFrame]->getModelPosition().x, slimeSet[slimeFrame]->getModelPosition().y, slimeSet[slimeFrame]->getModelPosition().z);
	glTranslatef(cam->cameraPos.x + cam->getForward().x * 5, cam->cameraPos.y + cam->getForward().y * 5, cam->cameraPos.z + cam->getForward().z * 5);

	slimeSet[slimeFrame]->renderModel(shader);

	glPopMatrix();

	slimeFrame++;
}

void deleteSlimeModels()
{
	for (int i = 0; i < slimeSet.size(); i++)
		delete slimeSet[i];
}

float redAmbient[4] = { 1, 0, 0, 1 };
float redDiffuse[4] = { 1, 0, 0, 1 };
float redSpecular[4] = { 1, 0, 0, 1 };
float redEmission[4] = { 1, 0, 0, 1 };

void applyRedMaterial()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, redAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, redDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, redSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, redEmission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
}

float whiteAmbient[4] = { 1, 1, 1, 1 };
float whiteDiffuse[4] = { 1, 1, 1, 1 };
float whiteSpecular[4] = { 1, 1, 1, 1 };
float whiteEmission[4] = { 1, 1, 1, 1 };

void applyWhiteMaterial()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, whiteAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, whiteDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, whiteEmission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
}

float blackAmbient[4] = { 0, 0, 0, 1 };
float blackDiffuse[4] = { 0, 0, 0, 1 };
float blackSpecular[4] = { 0, 0, 0, 1 };
float blackEmission[4] = { 0, 0, 0, 1 };

void applyBlackMaterial()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, blackAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blackDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, blackSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, blackEmission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
}

#define FONT_ComicSansMS "Comic Sans MS"
#define FONT_KaiTi "楷体"
#define FONT_FangSong "仿宋"

string GameTitle = "Bread Eating Game";
string GameRule = "Game Rule: ";
string GameRuleCtrl = "Control:\n   1. Press 'w/a/s/d' to move.\n   2. Press 'space' to jump.\n   3. Use 'mouse' to look around.";
string GameRuleTarget = "Target:\n    Eat as more bread as possible!";
string GameStartTitle = "Start";
string GameMaker = "——由第26组制作";
string GameVictory = "Win!!";

//选择font
void selectFont(int size, int charset, const char* face)
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}

vector<GLuint> listOfTexts;
void initEnString(const char* str)
{

}

//绘制英文文字
#define MAX_CHAR 128
void drawEnString(const char* str)
{
	static int isFirstCall = 1;
	GLuint lists;

	lists = glGenLists(MAX_CHAR);
	wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);

	//if (isFirstCall == 1) { // 如果是第一次调用，执行初始化, 为每一个ASCII字符产生一个显示列表
	//	isFirstCall = 0;

	//	// 申请MAX_CHAR个连续的显示列表编号
	//	lists = glGenLists(MAX_CHAR);

	//	// 把每个字符的绘制命令都装到对应的显示列表中
	//	wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	//}

	// 调用每个字符对应的显示列表，绘制每个字符
	for (; *str != '\0'; ++str)
		glCallList(lists + *str);

	glDeleteLists(lists, MAX_CHAR);
}



#define VictoryTextSizeLow 60    //闪烁文字最小字号
#define VictoryTextSizeUp 200    //闪烁文字最大字号
#define VictoryTextSizeGap 6     //闪烁文字放缩规模

string GameSceneUIText = "Apple: ";
static int victoryTextSize = VictoryTextSizeLow;
int dSize = VictoryTextSizeGap;

#include <glm/gtx/string_cast.hpp>
bool once = false;
void debugOnce(bool& once, glm::mat3 d)
{
	if (!once) {
		cout << glm::to_string(d) << endl;
		once = true;
	}
}

void drawGameSceneUIText(FPSCamera* cam, int x, int y)
{

	glm::mat3 vpMatI(
		300, 0, 300,
		0, 300, 300,
		0, 0, 1
	);

	vpMatI = glm::inverse(glm::transpose(vpMatI));
	glm::vec3 world = (vpMatI)*glm::vec3(x, y, 1);
	debugOnce(once, vpMatI);

	//why to add this ??????
	drawEnString("tt");


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, 0, 2);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	selectFont(36, ANSI_CHARSET, FONT_ComicSansMS);
	applyBlackMaterial();
	glRasterPos3f(world[0], world[1], 0);

	char strBuffer[80];
	const char* UIText1c = GameSceneUIText.c_str();
	string UIText2 = " / ";
	const char* UIText2c = UIText2.c_str();
	sprintf(strBuffer, "%s%d%s%d", UIText1c, eatenAppleNum, UIText2c, appleNum);
	//glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)strBuffer);
	drawEnString(strBuffer);




	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

void setupLights()
{
	glClearDepth(1.0f);				// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);		// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);			// The Type Of Depth Test To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// 启用光照计算  
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);

	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, LightPosition2);

	glEnable(GL_LIGHT3);
	glLightfv(GL_LIGHT3, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT3, GL_POSITION, LightPosition3);

	glEnable(GL_LIGHT4);
	glLightfv(GL_LIGHT4, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT4, GL_POSITION, LightPosition4);
}