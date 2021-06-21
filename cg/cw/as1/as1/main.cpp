#include <iostream>
#include <cmath>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "tut_headers/shader.h"
#include "tut_headers/camera.h"
#include "tut_headers/obj_loader.h"


// Function prototypes
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void move_model();
void update_polygon_display();

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;

bool keys[1024];

// Deltatime
GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame

// Polygon display
const GLfloat translateSpeed = 3.0f;
const GLfloat rotateSpeed = 3.0f;
glm::vec3 position;
GLfloat x_angle = 0.f, y_angle = 0.f, z_angle = 0.f;
GLfloat def_r = 0.99f, def_g = 0.49f, def_b = 0.19f;
GLint currentMode = 0;
GLint numMode = 4;

// The MAIN function, from here we start the application and run the game loop
int main() {
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment1", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    // Initial polygon display
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPointSize(5);

    // Build and compile our shader program
    Shader ourShader("main.vert.glsl", "main.frag.glsl");

    // Set up camera
    Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // Set up vertex data (and buffer(s)) and attribute pointers
    ObjLoader loader{};
    if (loader.load("eight.uniform.obj") != 0) {
        std::cout << "Failed to open obj file" << std::endl;
        return -1;
    }

    GLfloat *vertices = loader.getData();
    GLint nf = loader.getNf();

    position = glm::vec3(0.0f, 0.0f, 0.0f);
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 18 * nf * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if any events have been activiated (key pressed) and call corresponding response functions
        glfwPollEvents();
        move_model();
        update_polygon_display();

        // Render
        // Clear the colorbuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader
        ourShader.Use();

        // Camera/View transformation
        glm::mat4 view(1);
        view = camera.GetViewMatrix();
        // Projection
        glm::mat4 projection(1);
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat) WIDTH / (GLfloat) HEIGHT, 0.1f, 100.0f);
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
        GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
        GLint colorfulLoc = glGetUniformLocation(ourShader.Program, "colorful");
        GLint defColorLoc = glGetUniformLocation(ourShader.Program, "defaultColor");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(VAO);

        // Calculate the model matrix and pass it to shader before drawing
        glm::mat4 model(1);
        model = glm::translate(model, position);
        model = glm::rotate(model, x_angle, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, y_angle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, z_angle, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Calculate color and if colorful
        glUniform1i(colorfulLoc, currentMode >= 2);
        glUniform3f(defColorLoc, def_r, def_g, def_b);

        glDrawArrays(GL_TRIANGLES, 0, 3 * nf);

        if (currentMode == numMode - 1) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1.f, -1.f);
            glUniform1i(colorfulLoc, false);
            glDrawArrays(GL_TRIANGLES, 0, 3 * nf);
        }

        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, width, height);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        currentMode = (currentMode + 1) % numMode;
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        def_r += 0.1f;
        def_g += 0.1f;
        def_b += 0.1f;
        def_r -= floor(def_r);
        def_g -= floor(def_g);
        def_b -= floor(def_b);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void move_model() {
    // Camera controls
    GLfloat tspeed = translateSpeed * deltaTime;
    GLfloat rspeed = rotateSpeed * deltaTime;
    if (keys[GLFW_KEY_W])
        position.z += tspeed;
    if (keys[GLFW_KEY_S])
        position.z -= tspeed;
    if (keys[GLFW_KEY_A])
        position.x += tspeed;
    if (keys[GLFW_KEY_D])
        position.x -= tspeed;
    if (keys[GLFW_KEY_J])
        position.y -= tspeed;
    if (keys[GLFW_KEY_K])
        position.y += tspeed;
    if (keys[GLFW_KEY_U])
        x_angle += rspeed;
    if (keys[GLFW_KEY_M])
        x_angle -= rspeed;
    if (keys[GLFW_KEY_I])
        y_angle += rspeed;
    if (keys[GLFW_KEY_COMMA])
        y_angle -= rspeed;
    if (keys[GLFW_KEY_O])
        z_angle += rspeed;
    if (keys[GLFW_KEY_PERIOD])
        z_angle -= rspeed;
}

void update_polygon_display() {
    switch (currentMode) {
        case 0:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case 1:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        case 2:
        case 3:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        default:;
    }
}