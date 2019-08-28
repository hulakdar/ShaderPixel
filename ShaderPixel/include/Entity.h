#include "Mesh.h"

class Entity {
private:
    std::vector<Mesh> mMeshes;
	glm::mat4	mModelMatrix;
public:
	const glm::mat4& GetModelMatrix() const ;
};