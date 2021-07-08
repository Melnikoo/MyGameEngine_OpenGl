#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<GLM/glm.hpp>
#include <GLM/vec2.hpp>
#include <GLM/vec3.hpp>
#include <GLM/vec4.hpp>
#include <GLM/mat4x4.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>


#include <iostream>
#include "shader.h"
#include "stb_image.h"
#include <cmath>
#include "Camera.h"
#include "Model.h"

#include "mesh.h"
#include"Sound.h"
#include <irrklang/irrKlang.h>
#include <imgui.h>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "physics.h"
#include "UI.h"
#include "NetworkClient.h"
#include "enet/enet.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int arg, char** argv)
{

    UI ui;
    ui.UImain(arg, argv);
    NetworkClient netClient;
    const char* glsl_version = "#version 130";
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Telega", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    Physics physics(window);
    physics.PhysMain();
    Sound music;
    music.playMusic();

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    netClient.clientMain(window);


    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    Shader ourShader("./shaders/modelLoad.vs", "./shaders/modelLoad.fs");

    Model ourModel("./models/tank/tank.obj", glm::vec3(0.0f, 0.0f, -3.0f));
    Model floorModel("./models/brick/brick.obj", glm::vec3(0.0f, 0.0f, 0.0f));
    Model robotModel("./models/robot/robot.obj", glm::vec3(5.0f, 0.0f, 5.0f));
    Model ourModel2("./models/tank2/tank2.obj", glm::vec3(0.0f, 0.0f, 3.0f));
    Model skyboxModel("./models/skybox/skybox.obj", glm::vec3(0.0f, 0.0f, 0.0f));

    // render loop
    while (!glfwWindowShouldClose(window))
    {


       
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        // render
        physics.Update();
        glClearColor(0.5f, 0.5f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        glm::vec3 nothing = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 movement = glm::vec3(1.0f, 1.0f, 1.0f);
        movement.x = physics.cubePos.x;
        movement.y = physics.cubePos.y;
        movement.z = physics.cubePos.z;
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        ourModel.Update(deltaTime, model, ourShader, movement);
        floorModel.Update(deltaTime, model, ourShader, nothing);
        robotModel.Update(deltaTime, model, ourShader, nothing);
        ourModel2.Update(deltaTime, model, ourShader, nothing);
        skyboxModel.Update(deltaTime, model, ourShader, nothing);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        netClient.Update();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);


}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}