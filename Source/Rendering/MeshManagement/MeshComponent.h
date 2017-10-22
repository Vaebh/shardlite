#ifndef MESH_COMPONENT_SHARDLITE
#define MESH_COMPONENT_SHARDLITE

#include "../../EntityComponent/Component.h"

#include "../Shaders/VertexAttribute.h"
#include "../Shaders/ShaderUniform.h"

#include "../DrawCallObject.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <vector>

class Mesh;
class Shader;

class MeshComponent : public Component
{
public:
	MeshComponent(Mesh* mesh, Shader* shader, Entity* parentEntity);

	inline Shader* GetShader() { return m_shader; }
	inline std::vector<int> GetTextureIds() { return m_textureIds; }

	inline Mesh* GetMesh() { return m_mesh; }
	int GetVertexCount();

	inline GLuint GetVAO() { return m_vao; }

	void CreateVertexBuffers();
	void BindVertexData(AttributeType attributeType, VertexAttribute& vertexAttribute, const std::vector<GLfloat>& attributeData);

	void BindUniformData(UniformType uniformType, float uniformData);
	void BindUniformData(UniformType uniformType, double uniformData);
	void BindUniformData(UniformType uniformType, int uniformData);
	void BindUniformData(UniformType uniformType, glm::mat4 uniformData, GLint count = 1);

	DrawCallObject MakeDrawCallObject();

	void ShutDown();

private:
	Mesh* m_mesh;
    
	Shader* m_shader;
	std::vector<int> m_textureIds;

	GLuint m_vao;
	GLuint m_vertexAttributeVBOs[AllowedAttributeArraySize] {0};
};

#endif
