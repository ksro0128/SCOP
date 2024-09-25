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
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
    MeshUPtr m_box;
    ModelUPtr m_model;
    TextureUPtr m_texture;
    TextureUPtr m_texture2;

    glm::vec3 m_objectPos { glm::vec3(0.0f, 0.0f, 0.0f) };
    bool m_rotate { false };
    bool m_rotateToggled { false };
    bool m_textureEnabled { false };
    bool m_textureToggled { false };
    float m_textureMix { 0.0f };
    
    // clear color
    glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

    // camera parameters
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f, 5.0f) };
    glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, -1.0f) };
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
    int m_width {WINDOW_WIDTH};
    int m_height {WINDOW_HEIGHT};
};

#endif // __CONTEXT_H__