#include "GLFW.hpp"

void GLFW::init()
{

    if (!glfwInit()) return;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // opsiyonel MSAA:
    // glfwWindowHint(GLFW_SAMPLES, 4);


}

void GLFW::create(int width, int height)
{

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "ProjectCar", nullptr, nullptr);
    if (!window) { std::cerr << "GLFW window failed\n"; glfwTerminate(); 
    
    return; 
    
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n"; glfwTerminate(); 
        return ;
    }

    // callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);


}





