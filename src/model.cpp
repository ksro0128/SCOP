#include "model.h"
#include <random>

ModelUPtr Model::sLoad(const std::string& filename) {
    auto model = ModelUPtr(new Model());
    if (!model->LoadBysAssimp(filename))
        return nullptr;
    return std::move(model);
}

bool Model::LoadBysAssimp(const std::string& filename) {
    auto aAssimp = sAssimp::Load(filename);
    if (!aAssimp)
        return false;
    
    auto vertices = aAssimp->GetIndexedVertices();
    auto normals = aAssimp->GetIndexedNormals();
    auto texCoords = aAssimp->GetIndexedTexCoords();
    auto mtls = aAssimp->GetMtls();
    auto minX = aAssimp->GetMinX();
    auto maxX = aAssimp->GetMaxX();
    auto minY = aAssimp->GetMinY();
    auto maxY = aAssimp->GetMaxY();
    auto minZ = aAssimp->GetMinZ();
    auto maxZ = aAssimp->GetMaxZ();

    float avgX = (minX + maxX) / 2.0f;
    float avgY = (minY + maxY) / 2.0f;
    float avgZ = (minZ + maxZ) / 2.0f;


    std::vector<Vertex> vert;
    vert.resize(vertices.size());
    Vertex v;
    for (int i = 0; i < vertices.size(); i++) {
        v.position = *vertices[i];
        v.position.x -= avgX;
        v.position.y -= avgY;
        v.position.z -= avgZ; 
        if (i < texCoords.size())
            v.texCoord = *texCoords[i];
        else {
            float uf = (v.position.x - minX + avgX) / (maxX - minX);
            float vf = (v.position.y - minY + avgY) / (maxY - minY);
            v.texCoord = sglm::vec2(uf, vf);
        }
        if (i < normals.size())
            v.normal = *normals[i];
        else
            v.normal = sglm::vec3(0.0f, 0.0f, 0.0f);
        vert[i] = v;
    }

    auto indices = aAssimp->GetIndices();

    auto glMesh = Mesh::Create(vert, indices, GL_TRIANGLES);
    m_meshes.push_back(std::move(glMesh));

    std::string dirname = filename.substr(0, filename.find_last_of("/"));

    if (mtls.size() > 0) {
        for (auto& m : mtls) {
            auto glMaterial = Material::Create();
            std::string diffusePath = dirname + "/" + m.diffuseTex;
            std::string specularPath = dirname + "/" + m.specularTex;
            if (m.specularTex != "")
                glMaterial->specular = Texture::CreateFromImage(Image::LoadBmp(specularPath).get());
            if (m.diffuseTex != "")
                glMaterial->diffuse = Texture::CreateFromImage(Image::LoadBmp(diffusePath).get());
            m_materials.push_back(std::move(glMaterial));
        }
    }
    if (m_materials.size() > 0)
        m_meshes[0]->SetMaterial(m_materials[0]);

    return true;
}

void Model::Draw(const Program* program) const {
    for (auto& mesh: m_meshes) {
        mesh->Draw(program);
    }
}