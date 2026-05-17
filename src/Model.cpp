
#include"stb/stb_image.h"
#include"Model.hpp"

TriboxOverlap tri;

void Model::Draw( Shader& shader) {

	
	
	for (unsigned int i = 0; i < meshes.size(); i++) {


		meshes[i].draw(shader);
	}


}

void Model::loadModel(std::string path) {

	Assimp::Importer import;

	const aiScene * scene = import.ReadFile(
		path,
		aiProcess_Triangulate | aiProcess_FlipUVs
	);

	if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP: " << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);



}

void Model::processNode(aiNode* node, const aiScene* scene) {
	
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	vertices.reserve(mesh->mNumVertices);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex{};

		vertex.Pos = {
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		};

		if (mesh->HasNormals()) {
			vertex.Normal = {
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
			};
		}
		else {
			vertex.Normal = { 0.0f, 1.0f, 0.0f };
		}

		if (mesh->mTextureCoords[0]) {
			vertex.TexCoords = {
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			};
		}
		else {
			vertex.TexCoords = { 0.0f, 0.0f };
		}

		vertices.push_back(vertex);
	}

	for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
		const aiFace& face = mesh->mFaces[f];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		auto normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
		if (normalMaps.empty()) {
			auto heightAsNormal = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			normalMaps.insert(normalMaps.end(), heightAsNormal.begin(), heightAsNormal.end());
		}

		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	return Mesh(vertices, indices, textures);
}



std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; 
				break;
			}
		}
		if (!skip)
		{  
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory,true);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  
		}
	}
	return textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory, bool /*gamma*/)
{
	std::string filename = directory + "/" + std::string(path);

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format = GL_RGB, internal = GL_RGB;
		if (nrComponents == 1) { format = internal = GL_RED; }
		else if (nrComponents == 3) { format = internal = GL_RGB; }
		else if (nrComponents == 4) { format = internal = GL_RGBA; }

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}

void Model::Scale(Shader&shader,const glm::vec3&s) {


	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::scale(modelMatrix, s);

	shader.use();
	GLint loc = glGetUniformLocation(shader.ID, "model");


	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

}

void Model::Scale(Shader& shader, float uniformScale) {
	Scale(shader, glm::vec3(uniformScale));
}





void Model::obstacleFromModel(FlowField& f, const glm::mat4& M_model2world) const {
	const float h = f.g.h;
	int marked = 0;

	for (const Mesh& mesh : meshes) {
		const auto& V = mesh.verticies;
		const auto& I = mesh.indicies;

		for (size_t t = 0; t + 2 < I.size(); t += 3) {
			glm::vec3 v0 = glm::vec3(M_model2world * glm::vec4(V[I[t + 0]].Pos, 1));
			glm::vec3 v1 = glm::vec3(M_model2world * glm::vec4(V[I[t + 1]].Pos, 1));
			glm::vec3 v2 = glm::vec3(M_model2world * glm::vec4(V[I[t + 2]].Pos, 1));

		




			glm::vec3 triMin = glm::min(v0, glm::min(v1, v2));
			glm::vec3 triMax = glm::max(v0, glm::max(v1, v2));

			//domain dýþý
			triMin = glm::clamp(triMin, glm::vec3(0), glm::vec3(f.g.nx * h, f.g.ny * h, f.g.nz * h));
			triMax = glm::clamp(triMax, glm::vec3(0), glm::vec3(f.g.nx * h, f.g.ny * h, f.g.nz * h));

			int i0 = std::max(0, (int)std::floor(triMin.x / h) - 1);
			int j0 = std::max(0, (int)std::floor(triMin.y / h) - 1);
			int k0 = std::max(0, (int)std::floor(triMin.z / h) - 1);
			int i1 = std::min(f.g.nx - 1, (int)std::floor(triMax.x / h) + 1);
			int j1 = std::min(f.g.ny - 1, (int)std::floor(triMax.y / h) + 1);
			int k1 = std::min(f.g.nz - 1, (int)std::floor(triMax.z / h) + 1);

			for (int k = k0; k <= k1; ++k)
				for (int j = j0; j <= j1; ++j)
					for (int i = i0; i <= i1; ++i) {
						glm::vec3 cmin(i * h, j * h, k * h);
						glm::vec3 cmax = cmin + glm::vec3(h);
						glm::vec3 center = 0.5f * (cmin + cmax);
						glm::vec3 half = 0.5f * (cmax - cmin);

						if (tri.triBoxOverlap(center, half, v0, v1, v2)) {
							if (!f.S(i, j, k)) { f.S(i, j, k) = 1; ++marked; }
						}
					}
		}
	}

	std::cout << "[obstacleFromModel] marked cells: " << marked << "\n";
}


void Model::ModelAABB(
	const FlowField& f,
	const glm::mat4& M_model2world,
	glm::vec3& worldMin,
	glm::vec3& worldMax,
	glm::ivec3& cellMin,
	glm::ivec3& cellMax
) const 
{

	glm::vec3 mn(1e9f), mx(-1e9f);
	bool any = false;

	for (const Mesh& mesh : meshes) {
		for (const auto& v : mesh.verticies) {


			glm::vec3 pw = glm::vec3(M_model2world * glm::vec4(v.Pos, 1.0f));
			mn = glm::min(mn, pw);
			mx = glm::max(mx, pw);
			any = true;



		}


	}

	if (!any) {




		worldMin = glm::vec3(0);
		worldMax = glm::vec3(0);
		cellMin = glm::ivec3(0);
		cellMax = glm::ivec3(-1); 
		return;

	}
	worldMin = mn;
	worldMax = mx;

	//Dünya AABB sinin projekte edilmesi


	const float h = f.g.h;

	auto worldToCell = [&](float x, int n)->int {
		
		int c = (int)std::floor(x / h);

		if (c < 0)c = 0;
		if (c > n - 1)c = n - 1;
		return c;

		};

	glm::ivec3 cmin(

		worldToCell(worldMin.x, f.g.nx),
		worldToCell(worldMin.y, f.g.ny),
		worldToCell(worldMin.z, f.g.nz)
	
	);

	glm::ivec3 cmax(
		worldToCell(worldMax.x, f.g.nx),
		worldToCell(worldMax.y, f.g.ny),
		worldToCell(worldMax.z, f.g.nz)
	);


	cellMin = glm::min(cmin, cmax);
	cellMax = glm::max(cmin, cmax);

}