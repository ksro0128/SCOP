#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"

CLASS_PTR(Context)
class Context {
public:
    static ContextUPtr Create(const std::string& objFileName, const std::string& textureFileName);
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);

private:
    Context() {}
    bool Init(const std::string& objFileName, const std::string& textureFileName);
    ProgramUPtr m_program;
    MeshUPtr m_box;
    ModelUPtr m_model;
    TextureUPtr m_texture;
    TextureUPtr m_texture2;

    sglm::vec3 m_objectPos { sglm::vec3(0.0f, 0.0f, 0.0f) };

    bool m_rotateX { false };
    bool m_rotateY { false };
    bool m_rotateZ { false };

    bool m_rotateXToggled { false };
    bool m_rotateYToggled { false };
    bool m_rotateZToggled { false };

    bool m_textureEnabled { false };
    bool m_textureToggled { false };
    float m_textureMix { 0.0f };
    
    // camera parameters
    sglm::vec3 m_cameraPos { sglm::vec3(0.0f, 0.0f, 5.0f) };
    sglm::vec3 m_cameraFront { sglm::vec3(0.0f, 0.0f, -1.0f) };
    sglm::vec3 m_cameraUp { sglm::vec3(0.0f, 1.0f, 0.0f) };
    int m_width {WINDOW_WIDTH};
    int m_height {WINDOW_HEIGHT};
};

#endif // __CONTEXT_H__