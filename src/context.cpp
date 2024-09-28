#include "context.h"
#include "image.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow* window) {
    const float moveSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_objectPos.y += moveSpeed;
    }
        
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_objectPos.y -= moveSpeed;
    }
        
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_objectPos.x += moveSpeed;
    }
        
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_objectPos.x -= moveSpeed;
    }
            
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        m_objectPos.z += moveSpeed;
    }
        
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        m_objectPos.z -= moveSpeed;
    }

    // 스페이스바를 눌러 회전 활성화/비활성화 토글
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !m_rotateToggled) {
        m_rotate = !m_rotate;  // 회전 상태 토글
        m_rotateToggled = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        m_rotateToggled = false;  // 키를 떼면 다시 토글 가능
    }

    // T 키를 눌러 텍스쳐 활성화/비활성화 토글
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !m_textureToggled) {
        m_textureEnabled = !m_textureEnabled;  // 텍스쳐 상태 토글
        m_textureToggled = true;  // 텍스쳐 토글
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
        m_textureToggled = false;  // 키를 떼면 다시 토글 가능
    }        
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}


bool Context::Init() {
    // OBJ 파일 로드
    // m_model = Model::Load("./model/backpack.obj");
    // m_model = Model::Load("./resources/42.obj");
    m_model = Model::sLoad("./resources/42.obj");
    // m_model = Model::sLoad("./model/backpack.obj");

    if (!m_model) {
        std::cerr << "Failed to load model" << std::endl;
        return false;
    }

    // 셰이더 생성
    m_program = Program::Create("./shader/shader.vs", "./shader/shader.fs");
    if (!m_program) {
        std::cerr << "Failed to create program" << std::endl;
        return false;
    }

    // 이미지 로드
    auto image = Image::Load("./image/awesomeface.png");
    if (!image) {
        std::cerr << "Failed to load image" << std::endl;
        return false;
    }
    m_texture = Texture::CreateFromImage(image.get());

    m_program->SetUniform("mainTexture", 2);

    // 배경색 설정
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    return true;
}

void Context::Render() {
    // 화면 클리어
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // 카메라 및 변환 행렬 설정
    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.1f, 100.0f);
    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    auto model = glm::mat4(1.0f);  // 모델 행렬 (초기 위치)
    model = glm::translate(model, m_objectPos);  // 모델 이동

    // 모델 회전
    static float angle = 0.0f;
    if (m_rotate) {
        angle += 0.5f;
    }
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    // 셰이더 사용 및 변환 행렬 전달
    m_program->Use();
    m_program->SetUniform("view", view);
    m_program->SetUniform("projection", projection);
    m_program->SetUniform("model", model);


    if (m_textureEnabled) {
        if (m_textureMix < 1.0f) {
            m_textureMix += 0.01f;
        }
        m_program->SetUniform("textureEnabled", true);
    }
    else {
        if (m_textureMix > 0.0f) {
            m_textureMix -= 0.01f;
        }
        m_program->SetUniform("textureEnabled", false);
    }
    m_program->SetUniform("textureMix", m_textureMix);

    // 모델 그리기
    m_model->Draw(m_program.get());
}
