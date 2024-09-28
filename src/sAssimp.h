#ifndef SASSIMP_H
#define SASSIMP_H

#include "common.h"
#include "mesh.h"

struct Face {
    std::vector<uint32_t> indices;
    std::vector<uint32_t> texCoords;
    std::vector<uint32_t> normals;
};

struct mtl {
    std::string name;           // newmtl
    glm::vec3 ambient;          // Ka
    glm::vec3 diffuse;          // Kd
    glm::vec3 specular;         // Ks
    float shininess;            // Ns
    float alpha;                // d
    int illum;                  // illum
    std::string diffuseTex;     // map_Kd
    std::string specularTex;    // map_Ks
};

CLASS_PTR(sAssimp);
class sAssimp {
    public:
    static sAssimpUPtr Load(const std::string& filename);
    ~sAssimp() {}

    // Getters
    std::vector<glm::vec3> GetVertices() { return m_vertices; }
    std::vector<glm::vec3> GetNormals() { return m_normals; }
    std::vector<glm::vec2> GetTexCoords() { return m_texCoords; }
    std::vector<Face> GetFaces() { return m_faces; }
    std::vector<mtl> GetMtls() { return m_mtls; }
    

    // Debug
    void PrintVertices() {
        for (auto& v : m_vertices) {
            std::cout << "v " << v.x << " " << v.y << " " << v.z << std::endl;
        }
    }
    void PrintNormals() {
        for (auto& n : m_normals) {
            std::cout << "vn " << n.x << " " << n.y << " " << n.z << std::endl;
        }
    }
    void PrintTexCoords() {
        for (auto& t : m_texCoords) {
            std::cout << "vt " << t.x << " " << t.y << std::endl;
        }
    }
    void PrintFaces() {
        for (auto& f : m_faces) {
            std::cout << "f ";
            for (int i = 0; i < 3; i++) {
                std::cout << f.indices[i] << "/" << f.texCoords[i] << "/" << f.normals[i] << " ";
            }
            std::cout << std::endl;
        }
    }
    void PrintMtls() {
        for (auto& m : m_mtls) {
            std::cout << "newmtl " << m.name << std::endl;
            std::cout << "Ka " << m.ambient.x << " " << m.ambient.y << " " << m.ambient.z << std::endl;
            std::cout << "Kd " << m.diffuse.x << " " << m.diffuse.y << " " << m.diffuse.z << std::endl;
            std::cout << "Ks " << m.specular.x << " " << m.specular.y << " " << m.specular.z << std::endl;
            std::cout << "Ns " << m.shininess << std::endl;
            std::cout << "d " << m.alpha << std::endl;
            std::cout << "illum " << m.illum << std::endl;
            std::cout << "map_Kd " << m.diffuseTex << std::endl;
            std::cout << "map_Ks " << m.specularTex << std::endl;
        }
    }

    private:
    sAssimp() {}
    bool LoadBysAssimp(const std::string& filename);
    bool ParseVertex(std::stringstream& ss);
    bool ParseTexCoord(std::stringstream& ss);
    bool ParseNormal(std::stringstream& ss);
    bool ParseFace(std::stringstream& ss);
    bool ParseMtl(std::stringstream& ss, const std::string& objFilename);
    bool ParseMtlFile(const std::string& filename, const std::string& objFilename = "");
    mtl InitMtl();
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_texCoords;
    std::vector<Face> m_faces;
    std::vector<mtl> m_mtls;
};

#endif // SASSIMP_H