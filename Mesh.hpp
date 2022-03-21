#pragma once
#ifndef MESH_H
#define MESH_H
 
// #include <glad/glad.h> // holds all OpenGL type declarations
 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
 
// #include "shader_m.h"
#include "Shader.hpp"
#include "FPSCamera.hpp"
 
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;
 
struct Vertex {
	// position
	glm::vec3 pos;
	// normal
	glm::vec3 normal;
	// texCoords
	glm::vec2 texC;
	// tangent
	//glm::vec3 tangent;
	//// bitangent
	//glm::vec3 bitangent;

	Vertex() = default;
	Vertex(glm::vec3 p, glm::vec3 n) : pos(p), normal(n) {}
	Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 t) : pos(p), normal(n), texC(t) {}
};
 
struct Material {
	 // Material color lighting
	glm::vec3 Ka;
	 // Diffuse reflection
	glm::vec3 Kd;
	 // Mirror reflection
	glm::vec3 Ks;

	GLfloat shinness;
};
 
struct Texture {
	unsigned int id;
	string type;
	string path;
};
 
class Mesh {
public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	Material materials;
	GLuint VAO;
	unsigned int uniformBlockIndex;

	/*  Functions  */
	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material materials)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->materials = materials;
		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}
 
	// render the mesh
	void renderMesh(Shader shader)
	{
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
											  // retrieve texture number (the N in texture_diffuseN)
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream
 
													 // now set the sampler to the correct texture unit
 
			glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glUniform3fv(glGetUniformLocation(shader.Program, "ambientColor"), 1, glm::value_ptr(materials.Ka));
		glUniform3fv(glGetUniformLocation(shader.Program, "diffuseColor"), 1, glm::value_ptr(materials.Kd));
		glUniform3fv(glGetUniformLocation(shader.Program, "specularColor"), 1, glm::value_ptr(materials.Ks));
		glUniform1f(glGetUniformLocation(shader.Program, "shininessValue"), materials.shinness);

		// draw mesh
		glBindVertexArray(VAO);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniformBlockIndex, 0, sizeof(Material));
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
 
		//// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}
 
private:
	/*  Render data  */
	GLuint VBO, EBO;
 
	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenBuffers(1, &uniformBlockIndex);
 
		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		// std::cout << vertices.size() << std::endl;
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex) + sizeof(materials), &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockIndex);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(materials), (void*)(&materials), GL_STATIC_DRAW);
 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
 
		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texC));
	}
};
#endif