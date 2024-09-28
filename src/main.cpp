#include "common.h"
#include "shader.h"
#include "program.h"
#include "context.h"
#include "sAssimp.h"


void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    std::cout << "framebuffer size changed: " << width << "x" << height << std::endl;
    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    context->Reshape(width, height);
}

void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}


int main() {
    // glfw 라이브러리 초기화, 실패하면 에러 출력후 종료
    if (!glfwInit()) {
        const char* description = nullptr;
        glfwGetError(&description);
        std::cerr << "failed to initialize glfw: " << description << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw 윈도우 생성, 실패하면 에러 출력후 종료
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME,
      nullptr, nullptr);
    if (!window) {
        std::cerr << "failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    // glad를 활용한 OpenGL 함수 로딩
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "failed to initialize glad" << std::endl;
        glfwTerminate();
        return -1;
    }
    auto context = Context::Create();
    if (!context) {
        std::cerr << "failed to create context" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetWindowUserPointer(window, context.get());
    // callback 함수 등록
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, OnKeyEvent);
    // glfw 루프 실행, 윈도우 close 버튼을 누르면 정상 종료
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        context->ProcessInput(window);
        context->Render();

        glfwSwapBuffers(window);
    }
    context.reset();
    glfwTerminate();
    return 0;
}