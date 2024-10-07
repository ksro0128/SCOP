#include "context.h"
#include "image.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow* window) {
    const float moveSpeed = 0.2f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_objectPos.z += moveSpeed;
    }
        
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_objectPos.z -= moveSpeed;
    }
        
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_objectPos.x -= moveSpeed;
    }
        
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_objectPos.x += moveSpeed;
    }
            
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        m_objectPos.y += moveSpeed;
    }
        
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        m_objectPos.y -= moveSpeed;
    }

    // T 키를 눌러 텍스쳐 활성화/비활성화 토글
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !m_textureToggled) {
        m_textureEnabled = !m_textureEnabled;  // 텍스쳐 상태 토글
        m_textureToggled = true;  // 텍스쳐 토글
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
        m_textureToggled = false;  // 키를 떼면 다시 토글 가능
    }

    // 1, 2, 3 키를 눌러 X, Y, Z축 회전 활성화/비활성화 토글
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !m_rotateXToggled) {
        m_rotateX = !m_rotateX;  // X축 회전 상태 토글
        m_rotateXToggled = true;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
        m_rotateXToggled = false;  // 키를 떼면 다시 토글 가능
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !m_rotateYToggled) {
        m_rotateY = !m_rotateY;  // Y축 회전 상태 토글
        m_rotateYToggled = true;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
        m_rotateYToggled = false;  // 키를 떼면 다시 토글 가능
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !m_rotateZToggled) {
        m_rotateZ = !m_rotateZ;  // Z축 회전 상태 토글
        m_rotateZToggled = true;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
        m_rotateZToggled = false;  // 키를 떼면 다시 토글 가능
    }
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}


bool Context::Init() {
    // OBJ 파일 로드
    m_model = Model::sLoad("./resources/42.obj");

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

    // 텍스쳐 이미지 로드
    // if (textureFilename != "") {
        auto image = Image::LoadBmp("./resources/sample.bmp");
        if (!image) {
            std::cerr << "Failed to load image" << std::endl;
            return false;
        }
        m_texture = Texture::CreateFromImage(image.get());

        m_program->SetUniform("mainTexture", 0);
    // }


    // 배경색 설정
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    return true;
}

void Context::Render() {
    // 화면 클리어
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // 카메라 및 변환 행렬 설정
    auto projection = sglm::perspective(sglm::radians(45.0f), (float)m_width / (float)m_height, 0.1f, 1000.0f);
    auto view = sglm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    auto model = sglm::mat4(1.0f);  // 모델 행렬 (초기 위치)
    model = sglm::translate(model, m_objectPos);  // 모델 이동

    // 모델 회전
    static float angleX = 0.0f;
    static float angleY = 0.0f;
    static float angleZ = 0.0f;

    if (m_rotateX) {
        angleX += 0.5f;
        if (angleX >= 360.0f) {
            angleX = 0.0f;
        }
    }
    if (m_rotateY) {
        angleY += 0.5f;
        if (angleY >= 360.0f) {
            angleY = 0.0f;
        }
    }
    if (m_rotateZ) {
        angleZ += 0.5f;
        if (angleZ >= 360.0f) {
            angleZ = 0.0f;
        }
    }
    model = sglm::rotate(model, sglm::radians(angleX), sglm::vec3(1.0f, 0.0f, 0.0f));
    model = sglm::rotate(model, sglm::radians(angleY), sglm::vec3(0.0f, 1.0f, 0.0f));
    model = sglm::rotate(model, sglm::radians(angleZ), sglm::vec3(0.0f, 0.0f, 1.0f));


    // 셰이더 사용 및 변환 행렬 전달
    m_program->Use();
    m_program->SetUniform("view", view);
    m_program->SetUniform("projection", projection);
    m_program->SetUniform("model", model);
    if (m_textureEnabled) {
        if (m_textureMix < 1.0f) {
            m_textureMix += 0.01f;
        }
    }
    else {
        if (m_textureMix > 0.0f) {
            m_textureMix -= 0.01f;
        }
    }
    m_program->SetUniform("textureMix", m_textureMix);

    // 모델 그리기
    m_model->Draw(m_program.get());
}
