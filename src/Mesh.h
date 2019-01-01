#ifndef MESH_H
#define	MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shader.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh {
public:
	/*  Mesh Data  */
	vector<float> vert;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;
	/*  Functions  */
	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}

	Mesh()
	{
		//setupMesh2();
	}
	~Mesh() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	// render the mesh
	void Draw(Shader* shader, glm::mat4 &model, bool &isFromFile)
	{
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		/*for (Vertex vert : vertices) {
			cout << vert.Position.x << " "<< vert.Position.y << " "<< vert.Position.y << endl;
		}*/
		shader->use();
		/*glm::vec3 lightPosition = glm::vec3(0, 1, 0);
		shader->setVec3("lightPos", lightPosition);*/
		shader->setBool("isFromFile", isFromFile);
		shader->setMat4("model", model);
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
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
			//cout << name << number<< endl;
													 // now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		
		// draw mesh
		glBindVertexArray(VAO);
		if (indices.size() != 0)
		{
			if (isFromFile) {
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
				glActiveTexture(GL_TEXTURE0);
			}
			else
				glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
		}
		else {
			glDrawArrays(GL_POINTS, 0, vert.size());
		}
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		
	}

	void generateTorus(int num_segments, int num_rings, float r, float R) {
		vector<unsigned int> indexBuffer;
		vector<float> vertBuffer;
		int num_vertices = (num_rings + 1) * (num_segments + 1);

		const float pi = 3.1415926535f;
		const float r1 = R;
		const float r2 = r;
		for (int i = 0, index = 0; i <= num_rings; ++i) {
			for (int j = 0; j <= num_segments; ++j, ++index) {

				float u = float(i) / num_rings;
				float v = (float(j) + u) / num_segments;

				// Compute angles
				float u_angle = u * 2 * pi;
				float v_angle = v * 2 * pi;

				// Position
				float x = cos(u_angle) * (r1 + cos(v_angle) * r2);
				float y = sin(u_angle) * (r1 + cos(v_angle) * r2);
				float z = sin(v_angle) * r2;
				vertBuffer.push_back(x);
				vertBuffer.push_back(y);
				vertBuffer.push_back(z);
			}
		}

		// Compute torus indices
		for (int i = 0, index = 0; i <= num_vertices; ++i) {
			indexBuffer.push_back(int(i % num_vertices));
			indexBuffer.push_back(int((i + num_segments + 1) % num_vertices));
		}
		//cout << vert.size() << endl;
		vert = vertBuffer;
		indices = indexBuffer;
		setupMesh2();
	}
private:
	/*  Render data  */
	unsigned int VBO, EBO;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}

	void setupMesh2()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		//cout << "Liczba bajtów w buforze " << vert.size() * sizeof(float) << endl;
		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		//cout << "Stride: " << 3 * sizeof(float) << endl;

		glBindVertexArray(0);
	}
};
#endif