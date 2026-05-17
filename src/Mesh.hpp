
#include<glm/glm/glm.hpp>

#include<vector>
#include <string>
#include <fstream>
#include <sstream>
#include"Shader.hpp"
#include <iostream>
struct Vertex {
	glm::vec3 Pos;
	glm::vec3 Normal;
	glm::vec2 TexCoords;  
};

struct Texture {

	unsigned int id;
	std::string type;
	std::string path;

};


class Mesh {

	public:


		//mesh data

		std::vector<Vertex>verticies;
		std::vector<unsigned int>indicies;
		std::vector<Texture>textures;

		Mesh(std::vector<Vertex> verticies, std::vector<unsigned int>indicies, std::vector<Texture>textures);


		void draw(Shader&shader);
		private:

			unsigned int VAO, VBO, EBO;

			void setupMesh();

};