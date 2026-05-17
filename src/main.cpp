#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <string>



#include "Shader.hpp"



#include"Model.hpp"
#include "Grid.hpp"
#include "Camera.hpp"
#include "Solver.hpp"
#include "VectorRenderer.hpp"
#include"StreamLineRenderer.hpp"
#include"BoxWire.hpp"   
#include"SolidBox.hpp"



#include <iostream>




Camera camera({ 0.0f, -3.0f, -6.0f });
float lastX = 400.0f, lastY = 300.0f;
bool  firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
static bool g_CameraDrag = false;
bool rotating = false;




void framebuffer_size_callback(GLFWwindow*, int, int);
void cursor_pos_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main() {
   
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // opsiyonel MSAA:
    // glfwWindowHint(GLFW_SAMPLES, 4);
    
   


    
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "ProjectCar", nullptr, nullptr);
    if (!window) { std::cerr << "GLFW window failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n"; glfwTerminate(); return -1;
    }


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

   
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    
    
    // Grid3D
    Grid3D grid3d(32, 16, 16, 0.1f); // 3.2 x 1.6 x 1.6 kutu


  
    FlowField field(grid3d);
    int i0 = 10, i1 = 22;
    int j0 = 4, j1 = 12;
    int k0 = 4, k1 = 12;
  
 
   Solver solver(field, 0.6f, 0.05f);//tau,uIn

   // resources 
   Shader lineShader("shaders/line.vs", "shaders/line.fs");


   //MODEL
   SolidBox solid;
   Model m("Models/Mercedes/Mercedes.obj", false);
  
   Shader modelShader("shaders/Mercedes.vs", "shaders/Mercedes.fs");
  
   
   glm::vec3 bmin(i0 * grid3d.h,
       j0 * grid3d.h,
       k0 * grid3d.h);

   glm::vec3 bmax(i1 * grid3d.h,
       j1 * grid3d.h,
       k1 * grid3d.h);

   solid.setAABB(bmin, bmax);
    VectorRenderer vr;
   
    Shader slShader("shaders/streamline.vs", "shaders/streamline.fs");
    StreamLineRenderer sl;

 //   sl.seedFromInlet(field, /*sy=*/16, /*sz=*/12);
    
    
  

    //// domain bilgisi
    glm::vec3 half = 0.5f * glm::vec3(grid3d.nx * grid3d.h,
        grid3d.ny * grid3d.h,
        grid3d.nz * grid3d.h);

    // render matrisi
    glm::mat4 M_render = glm::mat4(1.0f);
    M_render = glm::translate(M_render, -half);                 
    M_render = glm::translate(M_render, glm::vec3(2.0f, 0.5f, 1.4f));
    M_render = glm::rotate(M_render, glm::radians(90.0f), glm::vec3(0, 1, 0));
    M_render = glm::scale(M_render, glm::vec3(0.2f));

    // flow için matris
   
    
     
  glm::mat4 M_flow = glm::translate(glm::mat4(1.0f), +half) * M_render;

   
  glm::vec3 worldMin, worldMax;
  glm::ivec3 cellMin, cellMax;
  m.ModelAABB(field, M_flow, worldMin, worldMax, cellMin, cellMax);

    
  m.obstacleFromModel( field, M_flow);
   
    
    
    
    
    
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
       

        solver.step(1.0f);
      

        vr.update(field, /*step=*/1.0, 1.0f);
       

        int w, h; glfwGetFramebufferSize(window, &w, &h);
        float aspect = (h > 0) ? (float)w / (float)h : 1.0f;
        glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), aspect, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::vec3 half = 0.5f * glm::vec3(grid3d.nx * grid3d.h,
            grid3d.ny * grid3d.h,
            grid3d.nz * grid3d.h);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), -half);
  

        glm::mat4 mvp = proj * view * model;
    // camera.FocusOnDomain(grid3d, 9.0f);
        processInput(window);
        glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
         lineShader.use();
     vr.draw(lineShader.ID, mvp);
     glm::mat4 vp = proj * view * model; 
  
        sl.build(field,
            /*dt=*/1.0f,
            /*maxSteps=*/100,
            /*vmaxColor=*/0.05f,   
            /*stopSpeed=*/1e-6f);
        slShader.use();
       
       sl.draw(slShader.ID, mvp);

        
       // MODEL
        ///////////////////////
       
        modelShader.use();
        modelShader.setMat4("uProjection", proj);
        modelShader.setMat4("uView", view);
        modelShader.setMat4("uModel", M_render);

        modelShader.setVec3("uColor", glm::vec3(1.0f));
        modelShader.setVec3("uLightDir", glm::normalize(glm::vec3(1, 1, 0)));

        m.Draw(modelShader);
        
        /////////////////
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

 

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rotating = true;
            firstMouse = true;                
            
        }
        else if (action == GLFW_RELEASE) {
            rotating = false;
            
        }
    }
}


void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

void cursor_pos_callback(GLFWwindow*window, double xpos, double ypos) {
    
    if (!rotating) return;

    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
        return;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; // ekran Y’si ters
    lastX = (float)xpos;
    lastY = (float)ypos;
  
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow*, double /*xoffset*/, double yoffset) {
    camera.ProcessMouseScroll((float)yoffset);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
}




