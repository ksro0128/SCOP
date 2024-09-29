#include "sAssimp.h"


sAssimpUPtr sAssimp::Load(const std::string& filename) {
    auto ret = sAssimpUPtr(new sAssimp());
    if (!ret->LoadBysAssimp(filename))
        return nullptr;
    ret->Indexing();
    return std::move(ret);
}

bool sAssimp::LoadBysAssimp(const std::string& filename) {
    auto result = LoadTextFile(filename);

    if (!result.has_value())
        return false;

    if (filename.substr(filename.find_last_of(".") + 1) != "obj") {
        std::cerr << "failed to load model: " << filename << " is not a Wavefront OBJ file" << std::endl;
        return false;
    }

    std::stringstream text(result.value());
    std::string line;

    while (std::getline(text, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;
        if (type == "v") {
            if (!ParseVertex(ss))
                return false;
        }
        else if (type == "vt") {
            if (!ParseTexCoord(ss))
                return false;
        }
        else if (type == "vn") {
            if (!ParseNormal(ss))
                return false;
        }
        else if (type == "f") {
            if (!ParseFace(ss))
                return false;
        }
        else if (type == "mtllib") {
            if (!ParseMtl(ss, filename))
                return false;
        }
    }

    return true;
}

bool sAssimp::ParseVertex(std::stringstream& ss) {
    glm::vec3 vertex;

    std::string token;

    if (!(ss >> token))
        return false;
    auto x = ParseFloat(token);
    if (!x.has_value())
        return false;
    vertex.x = x.value();

    if (!(ss >> token))
        return false;
    auto y = ParseFloat(token);
    if (!y.has_value())
        return false;
    vertex.y = y.value();

    if (!(ss >> token))
        return false;
    auto z = ParseFloat(token);
    if (!z.has_value())
        return false;
    vertex.z = z.value();

    if (ss >> token)
        return false;

    m_vertices.push_back(vertex);
    return true;
}

bool sAssimp::ParseTexCoord(std::stringstream& ss) {
    glm::vec3 texCoord;

    std::string token;
    if (!(ss >> token))
        return false;
    auto u = ParseFloat(token);
    if (!u.has_value())
        return false;
    texCoord.x = u.value();

    if (!(ss >> token))
        return false;
    auto v = ParseFloat(token);
    if (!v.has_value())
        return false;
    texCoord.y = v.value();

    if (ss >> token) {
        auto w = ParseFloat(token);
        if (!w.has_value())
            return false;
        texCoord.z = w.value();

        if (ss >> token)
            return false;
    }
    else {
        texCoord.z = 0.0f;
    }

    m_texCoords.push_back(texCoord);
    return true;
}

bool sAssimp::ParseNormal(std::stringstream& ss) {
    glm::vec3 normal;

    std::string token;
    if (!(ss >> token))
        return false;
    auto x = ParseFloat(token);
    if (!x.has_value())
        return false;
    normal.x = x.value();

    if (!(ss >> token))
        return false;
    auto y = ParseFloat(token);
    if (!y.has_value())
        return false;
    normal.y = y.value();

    if (!(ss >> token))
        return false;
    auto z = ParseFloat(token);
    if (!z.has_value())
        return false;
    normal.z = z.value();

    if (ss >> token)
        return false;

    m_normals.push_back(normal);
    return true;
}

bool sAssimp::ParseFace(std::stringstream& ss) {
    Face face;
    std::string token;
    std::vector<std::string> tokens;

    while (ss >> token) {
        tokens.push_back(token);
    }

    if (tokens.size() < 3)
        return false;
    for (auto t : tokens) {
        bool hasNormal = false;
        bool hasTexCoord = false;
        int index = 0;
        for (int i = 0; i < t.size(); i++) {
            if (t[i] == '/') {
                if (i == 0)
                    return false;
                if (hasTexCoord == false) {
                    hasTexCoord = true;
                    std::string vertex = t.substr(0, i);
                    auto x = ParseInt(vertex);
                    if (!x.has_value() || x.value() == 0)
                        return false;
                    face.indices.push_back(x.value());
                    index = i + 1;
                }
                else if (hasNormal == false) {
                    hasNormal = true;
                    if (i == index) {
                        face.texCoords.push_back(0);
                    }
                    else {
                        std::string texCoord = t.substr(index, i - index);
                        auto x = ParseInt(texCoord);
                        if (!x.has_value() || x.value() == 0)
                            return false;
                        face.texCoords.push_back(x.value());
                    }
                    index = i + 1;
                }
                else
                    return false;
            }
        }
        if (hasTexCoord == false) {
            auto x = ParseInt(t);
            if (!x.has_value() || x.value() == 0)
                return false;
            face.indices.push_back(x.value());
            face.texCoords.push_back(0);
            face.normals.push_back(0);
        }
        else if (hasNormal == false) {
            std::string texCoord = t.substr(index);
            std::cout << texCoord << std::endl;
            auto x = ParseInt(texCoord);
            if (!x.has_value() || x.value() == 0)
                return false;
            face.texCoords.push_back(x.value());
            face.normals.push_back(0);
        }
        else {
            if (index == t.size()) {
                face.normals.push_back(0);
            }
            else {
                std::string normal = t.substr(index);
                auto x = ParseInt(normal);
                if (!x.has_value() || x.value() == 0)
                    return false;
                face.normals.push_back(x.value());
            }
        }
    }

    // if (face.indices.size() > 3) {
    //     for (int i = 1; i < face.indices.size() - 1; i++) {
    //         Face tmp = { {face.indices[0], face.indices[i], face.indices[i + 1]},
    //                      {face.texCoords[0], face.texCoords[i], face.texCoords[i + 1]},
    //                      {face.normals[0], face.normals[i], face.normals[i + 1]} };
    //         m_faces.push_back(tmp);
    //     }
    // }
    // else {
    //     m_faces.push_back(face);
    // }
    m_faces.push_back(face);
    return true;
}

bool sAssimp::ParseMtl(std::stringstream& ss, const std::string& objFilename) {
    std::string filename;
    
    if (!(ss >> filename))
        return false;
    if (!ParseMtlFile(filename, objFilename))
        return false;
    if (ss >> filename)
        return false;

    return true;
}

bool sAssimp::ParseMtlFile(const std::string& filename, const std::string& objFilename) {
    std::string path = filename;
    if (objFilename != "") {
        size_t found = objFilename.find_last_of("/\\");
        if (found != std::string::npos) {
            path = objFilename.substr(0, found + 1) + filename;
        }
    }
    auto result = LoadTextFile(path);
    if (!result.has_value())
        return false;
    
    std::stringstream text(result.value());
    std::string line;
    mtl material = InitMtl();
    while (std::getline(text, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;
        if (type == "newmtl") {
            if (material.name != "") {
                m_mtls.push_back(material);
                material = InitMtl();
            }
            std::string name;
            if (!(ss >> name))
                return false;
            material.name = name;
            if (ss >> name)
                return false;
        }
        else if (type == "Ka") {
            glm::vec3 ambient;
            std::string token;
            if (!(ss >> token))
                return false;
            auto x = ParseFloat(token);
            if (!x.has_value())
                return false;
            ambient.x = x.value();

            if (!(ss >> token))
                return false;
            auto y = ParseFloat(token);
            if (!y.has_value())
                return false;
            ambient.y = y.value();

            if (!(ss >> token))
                return false;
            auto z = ParseFloat(token);
            if (!z.has_value())
                return false;
            ambient.z = z.value();

            if (ss >> token)
                return false;

            material.ambient = ambient;
        }
        else if (type == "Kd") {
            glm::vec3 diffuse;
            std::string token;
            if (!(ss >> token))
                return false;
            auto x = ParseFloat(token);
            if (!x.has_value())
                return false;
            diffuse.x = x.value();

            if (!(ss >> token))
                return false;
            auto y = ParseFloat(token);
            if (!y.has_value())
                return false;
            diffuse.y = y.value();

            if (!(ss >> token))
                return false;
            auto z = ParseFloat(token);
            if (!z.has_value())
                return false;
            diffuse.z = z.value();

            if (ss >> token)
                return false;

            material.diffuse = diffuse;
        }
        else if (type == "Ks") {
            glm::vec3 specular;
            std::string token;
            if (!(ss >> token))
                return false;
            auto x = ParseFloat(token);
            if (!x.has_value())
                return false;
            specular.x = x.value();

            if (!(ss >> token))
                return false;
            auto y = ParseFloat(token);
            if (!y.has_value())
                return false;
            specular.y = y.value();

            if (!(ss >> token))
                return false;
            auto z = ParseFloat(token);
            if (!z.has_value())
                return false;
            specular.z = z.value();

            if (ss >> token)
                return false;

            material.specular = specular;
        }
        else if (type == "d") {
            std::string token;
            if (!(ss >> token))
                return false;
            auto x = ParseFloat(token);
            if (!x.has_value())
                return false;
            if (ss >> token)
                return false;
            material.alpha = x.value();
        }
        else if (type == "Ns") {
            std::string token;
            if (!(ss >> token))
                return false;
            auto x = ParseFloat(token);
            if (!x.has_value())
                return false;
            if (ss >> token)
                return false;
            material.shininess = x.value();
        }
        else if (type == "map_Kd") {
            std::string token;
            if (!(ss >> token))
                return false;
            material.diffuseTex = token;
            if (ss >> token)
                return false;
        }
        else if (type == "map_Ks") {
            std::string token;
            if (!(ss >> token))
                return false;
            material.specularTex = token;
            if (ss >> token)
                return false;
        }
    }
    m_mtls.push_back(material);
    return true;
}

mtl sAssimp::InitMtl() {
    mtl material;
    material.name = "";
    material.ambient = glm::vec3(1.0f);
    material.diffuse = glm::vec3(1.0f);
    material.specular = glm::vec3(0.0f);
    material.shininess = 10.0f;
    material.alpha = 1.0f;
    material.illum = 2;
    material.diffuseTex = "";
    material.specularTex = "";
    return material;
}

void sAssimp::Indexing() {
    uint32_t index = 0;

    for (auto&face : m_faces) {
        for (int i = 0; i < face.indices.size(); i++) {
            m_indexedVertices.push_back(&m_vertices[face.indices[i] - 1]);
            if (face.texCoords[i] != 0)
                m_indexedTexCoords.push_back(&m_texCoords[face.texCoords[i] - 1]);
            if (face.normals[i] != 0)
                m_indexedNormals.push_back(&m_normals[face.normals[i] - 1]);
        }
        for (int i = 0; i < face.indices.size() - 2; i++) {
            m_indices.push_back(index);
            m_indices.push_back(index + i + 1);
            m_indices.push_back(index + i + 2);
        }
        index += face.indices.size();
    }
}