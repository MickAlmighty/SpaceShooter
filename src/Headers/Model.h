#ifndef MODEL_H
#define MODEL_H

#include <assimp/scene.h>

#include <Mesh.h>
#include <Shader.h>
#include <string>
#include <vector>

class Model{
private:
	/*  Dane modelu  */
	std::string directory;
	Shader* shader;
	bool isFromFile;
	float ao = 0.1f;
	float metaliness = 0.5f;
	float roughness = 0.9f;
	float reflection = 0.0f;
	float refraction = 0.0f;

	/*  Funkcje   */
	void loadModel(std::string const &path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

public:
	vector<Mesh*> meshes;
	vector<Texture> textures_loaded;
	void SetShader(Shader* s);
	/*  Funkcje   */
	Model(std::string path, float _ao = 0.1f, float _metaliness = 0.5f,
		float _roughness = 0.2f, float _reflection = 0.0f, float _refraction = 0.0f);
	
	Model();
	
	Model(Mesh* mesh);
	
	~Model();

	void Draw(glm::mat4 &model);
	
};
#endif