#include "sAssimp.h"


sAssimpUPtr sAssimp::Load(const std::string& filename) {
    auto ret = sAssimpUPtr(new sAssimp());
    if (!ret->LoadBysAssimp(filename))
        return nullptr;
    if (!ret->Indexing())
        return nullptr;
    return std::move(ret);
}

bool sAssimp::LoadBysAssimp(const std::string& filename) {
    auto result = LoadTextFile(filename);

    if (!result.has_value())
        return false;

    size_t lastOfPointIndex = filename.find_last_of(".");
    if (lastOfPointIndex == std::string::npos) {
        std::cerr << "failed to load model: " << filename << " is not a Wavefront OBJ file" << std::endl;
        return false;
    }
    std::string obj = filename.substr(lastOfPointIndex);
    if (obj != ".obj") {
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
            if (!ParseVertex(ss)) {
                std::cout << "failed to parse vertex" << std::endl;
                return false;
            }
        }
        else if (type == "vt") {
            if (!ParseTexCoord(ss)) {
                std::cout << "failed to parse texcoord" << std::endl;
                return false;
            }
        }
        else if (type == "vn") {
            if (!ParseNormal(ss)) {
                std::cout << "failed to parse normal" << std::endl;
                return false;
            }
        }
        else if (type == "f") {
            if (!ParseFace(ss)) {
                std::cout << "failed to parse face" << std::endl;
                return false;
            }
        }
        else if (type == "mtllib") {
            if (!ParseMtl(ss, filename)) {
                std::cout << "failed to parse mtl" << std::endl;
                return false;
            }
        }
    }

    return true;
}

bool sAssimp::ParseVertex(std::stringstream& ss) {
    sglm::vec3 vertex;

    std::string token;

    if (!(ss >> token))
        return false;
    auto x = ParseFloat(token);
    if (!x.has_value())
        return false;
    vertex.x = x.value();
    if (minX > vertex.x) minX = vertex.x;
    if (maxX < vertex.x) maxX = vertex.x;

    if (!(ss >> token))
        return false;
    auto y = ParseFloat(token);
    if (!y.has_value())
        return false;
    vertex.y = y.value();
    if (minY > vertex.y) minY = vertex.y;
    if (maxY < vertex.y) maxY = vertex.y;

    if (!(ss >> token))
        return false;
    auto z = ParseFloat(token);
    if (!z.has_value())
        return false;
    vertex.z = z.value();
    if (minZ > vertex.z) minZ = vertex.z;
    if (maxZ < vertex.z) maxZ = vertex.z;

    if (ss >> token)
        return false;

    m_vertices.push_back(vertex);
    return true;
}

bool sAssimp::ParseTexCoord(std::stringstream& ss) {
    sglm::vec2 texCoord;

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

    if (ss >> token)
    {
        auto w = ParseFloat(token);
        if (!w.has_value())
            return false;
    }

    m_texCoords.push_back(texCoord);
    return true;
}

bool sAssimp::ParseNormal(std::stringstream& ss) {
    sglm::vec3 normal;

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
            sglm::vec3 ambient;
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
            sglm::vec3 diffuse;
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
            sglm::vec3 specular;
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
    material.ambient = sglm::vec3(1.0f);
    material.diffuse = sglm::vec3(1.0f);
    material.specular = sglm::vec3(0.0f);
    material.shininess = 10.0f;
    material.alpha = 1.0f;
    material.illum = 2;
    material.diffuseTex = "";
    material.specularTex = "";
    return material;
}

// bool sAssimp::Indexing() {
//     uint32_t index = 0;

//     for (auto&face : m_faces) {
//         for (int i = 0; i < face.indices.size(); i++) {
//             int fidx = face.indices[i] - 1;
//             if (fidx < 0 || fidx >= m_vertices.size()) {
//                 std::cerr << "vertex index out of range" << std::endl;
//                 return false;
//             }
//             int nidx = face.normals[i] - 1;
//             if (m_normals.size() != 0 && nidx >= m_normals.size()) {
//                 std::cerr << "normal index out of range" << std::endl;
//                 return false;
//             }
//             int tidx = face.texCoords[i] - 1;
//             if (m_texCoords.size() != 0 && tidx >= m_texCoords.size()) {
//                 std::cerr << "texcoord index out of range" << std::endl;
//                 return false;
//             }
            
//             m_indexedVertices.push_back(&m_vertices[fidx]);
//             if (face.texCoords[i] != 0)
//                 m_indexedTexCoords.push_back(&m_texCoords[face.texCoords[i] - 1]);
//             if (face.normals[i] != 0)
//                 m_indexedNormals.push_back(&m_normals[face.normals[i] - 1]);
//         }
//         for (int i = 0; i < face.indices.size() - 2; i++) {
//             m_indices.push_back(index);
//             m_indices.push_back(index + i + 1);
//             m_indices.push_back(index + i + 2);
//         }
//         index += face.indices.size();
//     }
//     return true;
// }


bool sAssimp::Indexing() {
    uint32_t index = 0;
    for (auto&face : m_faces) {
        for (int i = 1; i < face.indices.size() - 1; i++) { 
            uint32_t fidx0 = face.indices[0] - 1;
            uint32_t fidx1 = face.indices[i] - 1;
            uint32_t fidx2 = face.indices[i + 1] - 1;
            if (fidx0 < 0 || fidx0 >= m_vertices.size() \
            || fidx1 < 0 || fidx1 >= m_vertices.size() \
            || fidx2 < 0 || fidx2 >= m_vertices.size()) {
                std::cerr << "vertex index out of range" << std::endl;
                return false;
            }
            m_indexedVertices.push_back(&m_vertices[fidx0]);
            m_indexedVertices.push_back(&m_vertices[fidx1]);
            m_indexedVertices.push_back(&m_vertices[fidx2]);

            if (face.texCoords[0] != 0) {
                uint32_t tidx0 = face.texCoords[0] - 1;
                uint32_t tidx1 = face.texCoords[i] - 1;
                uint32_t tidx2 = face.texCoords[i + 1] - 1;
                if (tidx0 < 0 || tidx1 < 0 || tidx2 < 0 || \
                tidx0 >= m_texCoords.size() || tidx1 >= m_texCoords.size() || tidx2 >= m_texCoords.size()) {
                    std::cerr << "texcoord index out of range" << std::endl;
                    return false;
                }
                m_indexedTexCoords.push_back(&m_texCoords[tidx0]);
                m_indexedTexCoords.push_back(&m_texCoords[tidx1]);
                m_indexedTexCoords.push_back(&m_texCoords[tidx2]);
            }

            if (face.normals[0] != 0) {
                uint32_t nidx0 = face.normals[0] - 1;
                uint32_t nidx1 = face.normals[i] - 1;
                uint32_t nidx2 = face.normals[i + 1] - 1;
                if (nidx0 < 0 || nidx1 < 0 || nidx2 < 0 || \
                nidx0 >= m_normals.size() || nidx1 >= m_normals.size() || nidx2 >= m_normals.size()) {
                    std::cerr << "normal index out of range" << std::endl;
                    return false;
                }
                m_indexedNormals.push_back(&m_normals[nidx0]);
                m_indexedNormals.push_back(&m_normals[nidx1]);
                m_indexedNormals.push_back(&m_normals[nidx2]);
            }
            m_indices.push_back(index++);
            m_indices.push_back(index++);
            m_indices.push_back(index++);
        }
    }
    return true;
}