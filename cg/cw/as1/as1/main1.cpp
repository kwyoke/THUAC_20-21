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
#include "tut_headers/obj3dmodel.h"


// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void move_model();
void update_polygon_display();

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];

// Movement by keyboard params
const GLfloat translateSpeed = 3.0f;
const GLfloat rotateSpeed = 3.0f;
glm::vec3 position;
GLfloat x_angle = 0.f, y_angle = 0.f, z_angle = 0.f;
GLfloat def_r = 0.49f, def_g = 0.99f, def_b = 0.19f;

// Color change and polygon display params
GLint currentMode = 0;
GLint numMode = 4;

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Camera3", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
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

    // Initialise polygon display
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPointSize(5);
    
    // Build and compile our shader program
    Shader ourShader("main1.vert.glsl", "main1.frag.glsl");
    
    obj3dmodel myModel("eight.uniform.obj");
    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat* vertices = new GLfloat[18 * myModel.nf]; // each face has three vertices, each vertice has 3-d pos coords and 3-d col coords
    for (int i = 0; i < myModel.nf; i++) {
        //fill in vertex positions 
        vertices[18 * i] = myModel.vertices[myModel.faces[i].v1 - 1].x;
        vertices[18 * i + 1] = myModel.vertices[myModel.faces[i].v1 - 1].y;
        vertices[18 * i + 2] = myModel.vertices[myModel.faces[i].v1 - 1].z;

        vertices[18 * i + 6] = myModel.vertices[myModel.faces[i].v2 - 1].x;
        vertices[18 * i + 7] = myModel.vertices[myModel.faces[i].v2 - 1].y;
        vertices[18 * i + 8] = myModel.vertices[myModel.faces[i].v2 - 1].z;

        vertices[18 * i + 12] = myModel.vertices[myModel.faces[i].v3 - 1].x;
        vertices[18 * i + 13] = myModel.vertices[myModel.faces[i].v3 - 1].y;
        vertices[18 * i + 14] = myModel.vertices[myModel.faces[i].v3 - 1].z;

        //fill in intermediate vertex colours (centre coordinate of triangle face)
        vertices[18 * i + 3] = vertices[18 * i + 9] = vertices[18 * i + 15] =
            (vertices[18 * i] + vertices[18 * i + 6] + vertices[18 * i + 12]) / 3;
        vertices[18 * i + 4] = vertices[18 * i + 10] = vertices[18 * i + 16] =
            (vertices[18 * i + 1] + vertices[18 * i + 7] + vertices[18 * i + 13]) / 3;
        vertices[18 * i + 5] = vertices[18 * i + 11] = vertices[18 * i + 17] = 
            (vertices[18 * i + 2] + vertices[18 * i + 8] + vertices[18 * i + 14]) / 3;
    }

   
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 18 * myModel.nf * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Colour attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0); // Unbind VAO
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        move_model();
        update_polygon_display();
        
        // Render
        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Activate shader
        ourShader.Use();
        
        // Camera/View transformation
        glm::mat4 view(1);
        view = camera.GetViewMatrix();
        // Projection
        glm::mat4 projection(1);
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
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

        glDrawArrays(GL_TRIANGLES, 0, 3 * myModel.nf);

        //for face and edge mode
        if (currentMode == numMode - 1) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1.f, -1.f);
            glUniform1i(colorfulLoc, false);
            glDrawArrays(GL_TRIANGLES, 0, 3 * myModel.nf);
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, width, height);
}


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    //press 'v' to change polygon display mode
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        currentMode = (currentMode + 1) % numMode;
    }

    // press 'c' to change colour
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

    // translate control
    if (keys[GLFW_KEY_W]) //move forward
        position.z += tspeed;
    if (keys[GLFW_KEY_S]) //move backward
        position.z -= tspeed;
    if (keys[GLFW_KEY_A]) //move right
        position.x += tspeed;
    if (keys[GLFW_KEY_D]) // move left
        position.x -= tspeed;
    if (keys[GLFW_KEY_Z]) //move up
        position.y -= tspeed;
    if (keys[GLFW_KEY_X]) //move down
        position.y += tspeed;

    // rotate control
    if (keys[GLFW_KEY_U])
        z_angle += rspeed;
    if (keys[GLFW_KEY_H])
        z_angle -= rspeed;
    if (keys[GLFW_KEY_J]) 
        x_angle += rspeed;
    if (keys[GLFW_KEY_K]) 
        x_angle -= rspeed;
    if (keys[GLFW_KEY_N]) 
        y_angle += rspeed;
    if (keys[GLFW_KEY_M])
        y_angle -= rspeed;
}

void update_polygon_display() {
    switch (currentMode) {
    case 0: //wireframe mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case 1: //vertex mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case 2: //face mode (default polygon display mode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case 3: //face and edge mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    default:;
    }
}