#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shaderinit.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <vector>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void init(void);
void render();


void transformations(Shader& ourShader, unsigned int textureID);

// variables
const unsigned int screen_width = 1200;
const unsigned int screen_height = 800;
const GLuint NumVertices = 36;
unsigned int texture;
GLuint VBO;
GLuint VAO;
GLuint EBO;
float startX = -1.0f;
float startY = 0.9f;
float rectWidth = 1.0f;
float rectHeight = 1.0f;
float padding = 0.1f;
// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//smiley texture flag
bool isSmiley = false;

//texture
void applyTexture(const char* texturePath, unsigned int& textureID);
unsigned int textures[4];

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window;
    window = glfwCreateWindow(screen_width, screen_height, "OpenGLCameraAuto", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);
    // build and compile our shader program
    std::vector<Shader> shaders = {
        Shader("shader.vs", "shader_yellow.fs"),
        Shader("shader.vs", "shader_green.fs"),
        Shader("shader.vs", "shader_red.fs"),
        Shader("shader.vs", "shader_blue.fs"),
        Shader("shader.vs", "shader.fs")//texture shader
    };

    init();

    //pplyTexture(texture);

    //cube 
    const int numRows = 5;
    const int numCols = 9;

    std::vector<glm::vec3> primPositions;

    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            float xPos = startX + col * (rectWidth + padding);
            float yPos = startY - row * (rectHeight + padding);
            float zPos = -10.0f;

            primPositions.push_back(glm::vec3(xPos, yPos, zPos));
        };
    };



    //code here
    // camera/view transformation
    glm::mat4 view = glm::mat4(1.0f); // view matrix initialization
    // setting the radius variable 
    float radius = 10.0f;


    // render loop
    // render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        render();
        glBindVertexArray(VAO);


        for (unsigned int i = 0; i < 45; i++) {
            unsigned int textureIndex = i % 4; // Cycle through textures 0, 1, 2, 3
            transformations(shaders[textureIndex], textures[textureIndex]);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, primPositions[i]);
            shaders[textureIndex].setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    glfwDestroyWindow(window);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

void transformations(Shader& ourShader, unsigned int textureID)
{
    ourShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    ourShader.setInt("texture1", 0);

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 2.5f, 2.0f), glm::vec3(0.7f, 1.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ourShader.setMat4("view", view);
}

void render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void init(void)
{

    // Texture paths for individual cube textures
    const char* texturePaths[4] = {
        "assets/red.png",
        "assets/yellow.png",
        "assets/blue.jpg",
        "assets/green.jpg"
    };

    // Apply textures to cube textures array
    for (int i = 0; i < 4; ++i) {
        applyTexture(texturePaths[i], textures[i]);
    }

    float vertices[] = {
        // Front face
        // Position             // Color             // Texture coordinates
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
        0.5f, 0.5f, -0.5f,      1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        0.5f, 0.5f, -0.5f,      1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f,     1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

        // Back face
        -0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
        0.5f, -0.5f, 0.5f,      0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
        0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
        0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f,      0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.0f,

        // Top face
        -0.5f, 0.5f, -0.5f,     0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
        0.5f, 0.5f, -0.5f,      0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
        0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
        0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f,      0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f,     0.0f, 0.0f, 1.0f,    0.0f, 1.0f,

        // Bottom face
        -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 0.0f,    1.0f, 1.0f,
        0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 0.0f,    1.0f, 0.0f,
        0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 0.0f,    1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 0.0f,    0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f,

        // Right face
        0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 1.0f,    0.0f, 1.0f,
        0.5f, 0.5f, -0.5f,      1.0f, 0.0f, 1.0f,    1.0f, 1.0f,
        0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 1.0f,    1.0f, 0.0f,
        0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 1.0f,    1.0f, 0.0f,
        0.5f, -0.5f, 0.5f,      1.0f, 0.0f, 1.0f,    0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 1.0f,    0.0f, 1.0f,

        // Left face
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f,     0.0f, 1.0f, 1.0f,    0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f,      0.0f, 1.0f, 1.0f,    0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f,      0.0f, 1.0f, 1.0f,    0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 1.0f,    1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f
    };


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

// glfw: user input
// glfw: user input
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 3 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        isSmiley = true;
    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        isSmiley = false;
    }
}


// glfw: viewport to window adjustment
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: callback called when the mouse moves
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

    float sensitivity = 0.1f; // a ratio that define how sensitive the mouse movement will be
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: callback called, when the mouse scroll wheel scrolls
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void applyTexture(const char* texturePath, unsigned int& textureID) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture: " << texturePath << std::endl;
    }
    stbi_image_free(data);
}