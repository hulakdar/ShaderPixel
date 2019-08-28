#include "Mesh.h"

class Entity {
private:
    std::vector<Mesh> m_Meshes;
	glm::mat4	m_ModelMatrix;
public:
	const glm::mat4& GetModelMatrix() const ;
};