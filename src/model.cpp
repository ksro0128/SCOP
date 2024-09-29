#include "model.h"
#include <random>

ModelUPtr Model::Load(const std::string& filename) {
    auto model = ModelUPtr(new Model());
    if (!model->LoadByAssimp(filename))
        return nullptr;
    return std::move(model);
}

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

    float minX = vertices[0]->x, maxX = vertices[0]->x;
    float minY = vertices[0]->y, maxY = vertices[0]->y;

    for (const auto& vertex : vertices) {
        if (vertex->x < minX) minX = vertex->x;
        if (vertex->x > maxX) maxX = vertex->x;
        if (vertex->y < minY) minY = vertex->y;
        if (vertex->y > maxY) maxY = vertex->y;
    }


    std::vector<Vertex> vert;
    vert.resize(vertices.size());
    Vertex v;
    for (int i = 0; i < vertices.size(); i++) {
        v.position = *vertices[i];
        if (i < texCoords.size())
            v.texCoord = *texCoords[i];
        else {
            float uf = (v.position.x - minX) / (maxX - minX);
            float vf = (v.position.y - minY) / (maxY - minY);
            v.texCoord = glm::vec2(uf, vf);
        }
        if (i < normals.size())
            v.normal = *normals[i];
        else
            v.normal = glm::vec3(0.0f, 0.0f, 0.0f);
        // std::cout << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << std::endl;
        vert[i] = v;
    }

    auto indices = aAssimp->GetIndices();

    auto glMesh = Mesh::Create(vert, indices, GL_TRIANGLES);
    m_meshes.push_back(std::move(glMesh));

    std::string dirname = filename.substr(0, filename.find_last_of("/"));

    // if (mtls.size() > 0) {
    //     for (auto& m : mtls) {
    //         auto glMaterial = Material::Create();
    //         std::string diffusePath = dirname + "/" + m.diffuseTex;
    //         std::string specularPath = dirname + "/" + m.specularTex;
    //         if (m.specularTex != "")
    //             glMaterial->specular = Texture::CreateFromImage(Image::LoadBmp(specularPath).get());
    //         if (m.diffuseTex != "")
    //             glMaterial->diffuse = Texture::CreateFromImage(Image::LoadBmp(diffusePath).get());
    //         m_materials.push_back(std::move(glMaterial));
    //     }
    // }

    // 빛을 사용하지 않기 때문에 재질을 설정하지 않음
    // if (m_materials.size() > 0)
    //     m_meshes[0]->SetMaterial(m_materials[0]);

    return true;
}



bool Model::LoadByAssimp(const std::string& filename) {
    Assimp::Importer importer;
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "failed to load model: " << importer.GetErrorString() << std::endl;
        return false;
    }

    auto dirname = filename.substr(0, filename.find_last_of("/"));
    // auto LoadTexture = [&](aiMaterial* material, aiTextureType type) -> TexturePtr {
    //     if (material->GetTextureCount(type) <= 0)
    //         return nullptr;
    //     aiString filepath;
    //     material->GetTexture(type, 0, &filepath);

    //     std::ostringstream filepathStream;
    //     filepathStream << dirname << "/" << filepath.C_Str();
    //     auto image = Image::Load(filepathStream.str());
    //     if (!image)
    //         return nullptr;
    //     return Texture::CreateFromImage(image.get());
    // };

    // for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
    //     auto material = scene->mMaterials[i];
    //     auto glMaterial = Material::Create();
    //     glMaterial->diffuse = LoadTexture(material, aiTextureType_DIFFUSE);
    //     glMaterial->specular = LoadTexture(material, aiTextureType_SPECULAR);
    //     m_materials.push_back(std::move(glMaterial));
    // }

    ProcessNode(scene->mRootNode, scene);
    return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        auto meshIndex = node->mMeshes[i];
        auto mesh = scene->mMeshes[meshIndex];
        ProcessMesh(mesh, scene);
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    vertices.resize(mesh->mNumVertices);
    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        auto& v = vertices[i];
        v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        // std::cout << "v " << mesh->mVertices[i].x << " " << mesh->mVertices[i].y << " " << mesh->mVertices[i].z << std::endl;
        if (mesh->HasNormals()) {
            v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        else {
            v.normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        if (mesh->HasTextureCoords(0)) {
            v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            glm::vec3 p = glm::normalize(v.position);
            float uf = v.position.x;
            float vf = v.position.y;
            v.texCoord = glm::vec2(uf, vf);
        }
    }

    std::vector<uint32_t> indices;
    indices.resize(mesh->mNumFaces * 3);
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        indices[3*i  ] = mesh->mFaces[i].mIndices[0];
        indices[3*i+1] = mesh->mFaces[i].mIndices[1];
        indices[3*i+2] = mesh->mFaces[i].mIndices[2];
        // std::cout << "f " << mesh->mFaces[i].mIndices[0] << " " << mesh->mFaces[i].mIndices[1] << " " << mesh->mFaces[i].mIndices[2] << std::endl;
    }

    auto glMesh = Mesh::Create(vertices, indices, GL_TRIANGLES);
    if (mesh->mMaterialIndex >= 0)
        glMesh->SetMaterial(m_materials[mesh->mMaterialIndex]);
    m_meshes.push_back(std::move(glMesh));
}

void Model::Draw(const Program* program) const {
    for (auto& mesh: m_meshes) {
        mesh->Draw(program);
    }
}