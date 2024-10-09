#include "mesh.h"

MeshUPtr Mesh::Create(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    uint32_t primitiveType) {
    auto mesh = MeshUPtr(new Mesh());
    mesh->Init(vertices, indices, primitiveType);
    return std::move(mesh);
}

void Mesh::Init(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, uint32_t primitiveType) {
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices.data(), sizeof(Vertex), vertices.size());
    m_indexBuffer = Buffer::CreateWithData(
        GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices.data(), sizeof(uint32_t), indices.size());
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false,
        sizeof(Vertex), 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false,
        sizeof(Vertex), offsetof(Vertex, normal));
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false,
        sizeof(Vertex), offsetof(Vertex, texCoord));

    std::vector<float> rgbRandom;
    rgbRandom.resize(indices.size() * 3);
    for (int i = 0; i < indices.size() / 3; i++) {
        float ran = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float gray = 0.5f * ran;
        for (int j = 0; j < 9; j++) {
            rgbRandom[i * 9 + j] = gray;
        }
    }
    m_vertexBufferRGB = Buffer::CreateWithData(
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        rgbRandom.data(), sizeof(float), rgbRandom.size());
    
    m_vertexLayout->SetAttrib(3, 3, GL_FLOAT, false,
        sizeof(float) * 3, 0);
}

void Mesh::Draw(const Program* program) const {
  m_vertexLayout->Bind();
  if (m_material) {
    m_material->SetToProgram(program);
  }
  glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}

void Material::SetToProgram(const Program* program) const {
    int textureCount = 0;
    if (diffuse) {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->SetUniform("material.diffuse", textureCount);
        diffuse->Bind();
        textureCount++;
    }
    if (specular) {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->SetUniform("material.specular", textureCount);
        specular->Bind();
        textureCount++;
    }
    glActiveTexture(GL_TEXTURE0);
    program->SetUniform("material.shininess", shininess);
}

