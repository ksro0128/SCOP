#include "model.h"
#include <random>

ModelUPtr Model::Load(const std::string& filename) {
    auto model = ModelUPtr(new Model());
    if (!model->LoadByAssimp(filename))
        return nullptr;
    return std::move(model);
}

bool Model::LoadByAssimp(const std::string& filename) {
    Assimp::Importer importer;
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "failed to load model: " << importer.GetErrorString() << std::endl;
        return false;
    }

    auto dirname = filename.substr(0, filename.find_last_of("/"));
    auto LoadTexture = [&](aiMaterial* material, aiTextureType type) -> TexturePtr {
        if (material->GetTextureCount(type) <= 0)
            return nullptr;
        aiString filepath;
        material->GetTexture(type, 0, &filepath);

        std::ostringstream filepathStream;
        filepathStream << dirname << "/" << filepath.C_Str();
        auto image = Image::Load(filepathStream.str());
        if (!image)
            return nullptr;
        return Texture::CreateFromImage(image.get());
    };

    for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
        auto material = scene->mMaterials[i];
        auto glMaterial = Material::Create();
        glMaterial->diffuse = LoadTexture(material, aiTextureType_DIFFUSE);
        glMaterial->specular = LoadTexture(material, aiTextureType_SPECULAR);
        m_materials.push_back(std::move(glMaterial));
    }

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
        if (mesh->HasVertexColors(0)) {
            v.rgb = glm::vec3(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b);
        }
        else {
            // 랜덤 색상 넣기
            v.rgb = glm::vec3(
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX),  // 0.0 ~ 1.0 사이의 난수 생성
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX)
                );
        }
        if (mesh->HasTextureCoords(0)) {
            v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            glm::vec3 p = glm::normalize(v.position); // 정점을 단위 벡터로 변환
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