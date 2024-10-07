#define _CRTDBG_MAP_ALLOC
#include "common.h"
#include "shader.h"
#include "program.h"
#include "context.h"
#include <cstdlib>
#include <crtdbg.h>


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

int main(int argc, char* argv[]) {

    // 메모리 릭 감지 활성화
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    {
        if (argc > 3) {
            std::cerr << "usage: " << argv[0] << " <obj file> <texture file>" << std::endl;
            return -1;
        }


        // 릭 확인
        // int* leak = (int*)malloc(sizeof(int) * 100);
        // leak[5] = 10;
        // leak = nullptr;  // 메모리 해제하지 않음

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

        std::string objFileName = "./resources/42.obj";
        std::string textureFileName = "";

        if (argc > 1) {
            objFileName = argv[1];
        }
        if (argc > 2) {
            textureFileName = argv[2];
        }

        auto context = Context::Create(objFileName, textureFileName);
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
    }

    //릭 감지 결과 출력
    _CrtDumpMemoryLeaks();
    return 0;
}