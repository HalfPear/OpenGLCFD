#pragma once

#include<glad/glad.h>
#include<glm/glm/glm.hpp>
#include"Mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include"FlowField.hpp"
#include"Shader.hpp"
#include"TriboxOverlap.hpp"



class Model {

	public:

		//mdodel data
		std::vector<Texture> textures_loaded;

		std::vector<Mesh>meshes;
		std::string directory;
		
		bool gammaCorrection;

		Model(std::string const& path, bool gamma = false) : gammaCorrection(gamma) {


			loadModel(path);
		}

		void Draw(  Shader& shader);
		void Scale(Shader&shader,const glm::vec3&s);

		void Scale(Shader& shader, float uniformScale);

		void obstacleFromModel(FlowField& f, const glm::mat4& M_model2world) const;
		void ModelAABB(
			const FlowField& f,
			const glm::mat4& M_model2world,
			glm::vec3& wordlMin,
			glm::vec3& worldMax,
			glm::ivec3&cellMin,
			glm::ivec3&cellMax)const;

private:
	
	

	
	
	
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
		std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);

};