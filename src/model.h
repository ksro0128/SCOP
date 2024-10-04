#ifndef __MODEL_H__
#define __MODEL_H__

#include "common.h"
#include "mesh.h"
#include "sAssimp.h"

CLASS_PTR(Model);
class Model {
public:
    static ModelUPtr sLoad(const std::string& filename);

    int GetMeshCount() const { return (int)m_meshes.size(); }
    MeshPtr GetMesh(int index) const { return m_meshes[index]; }
    void Model::Draw(const Program* program) const;

private:
    Model() {}
    bool LoadBysAssimp(const std::string& filename);
        
    std::vector<MeshPtr> m_meshes;
    std::vector<MaterialPtr> m_materials;
};

#endif // __MODEL_H__