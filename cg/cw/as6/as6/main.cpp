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
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void interact_model();

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera  camera(glm::vec3(-1.0f, 1.3f, 1.2f), glm::vec3(0.0f, 1.0f, 0.0f), -57.0f, -22.0f);
GLfloat lastX  =  WIDTH  / 2.0;
GLfloat lastY  =  HEIGHT / 2.0;
bool    keys[1024];
bool firstMouse = true;

// Movement by keyboard params
const GLfloat translateSpeed = 3.0f;
const GLfloat rotateSpeed = 3.0f;
glm::vec3 position;
GLfloat x_angle = 0.f, y_angle = 0.f, z_angle = 0.f;

// display mode
GLint currentMode = 0;

// Light attributes
glm::vec3 lightPos(0.8f, 1.0f, -0.2f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
GLfloat lightAmbientStrength = 0.2;
GLfloat lightDiffuseStrength = 0.5;
GLfloat lightSpecularStrength = 1.0;

//Material attributes
glm::vec3 matColor(0.3f, 0.8f, 0.4f);
GLfloat matKa = 0.2;
GLfloat matKd = 0.3;
GLfloat matKs = 1.0;
GLfloat mat_n = 32.0;

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Light2", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // GLFW Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // OpenGL options
    glEnable(GL_DEPTH_TEST);
    
    
    // Build and compile our shader program
    Shader lightingShader("shaders/material.vert.glsl", "shaders/material.frag.glsl");
    Shader lampShader("shaders/lamp.vert.glsl", "shaders/lamp.frag.glsl");
    
    //load object
    obj3dmodel myModel("eight.uniform.obj");

    // Set up vertex data (and buffer(s)) and attribute pointers
    //mode 1 vertices
    GLfloat* vertices_m1 = new GLfloat[18 * myModel.nf]; // each face has three vertices, each vertice has 3-d pos coords + 3-d face normals
    for (int i = 0; i < myModel.nf; i++) {
        //fill in vertex positions 
        vertices_m1[18 * i] = myModel.vertices[myModel.faces[i].v1 - 1].x;
        vertices_m1[18 * i + 1] = myModel.vertices[myModel.faces[i].v1 - 1].y;
        vertices_m1[18 * i + 2] = myModel.vertices[myModel.faces[i].v1 - 1].z;

        vertices_m1[18 * i + 6] = myModel.vertices[myModel.faces[i].v2 - 1].x;
        vertices_m1[18 * i + 7] = myModel.vertices[myModel.faces[i].v2 - 1].y;
        vertices_m1[18 * i + 8] = myModel.vertices[myModel.faces[i].v2 - 1].z;

        vertices_m1[18 * i + 12] = myModel.vertices[myModel.faces[i].v3 - 1].x;
        vertices_m1[18 * i + 13] = myModel.vertices[myModel.faces[i].v3 - 1].y;
        vertices_m1[18 * i + 14] = myModel.vertices[myModel.faces[i].v3 - 1].z;

        //fill in vertex normals
        vertices_m1[18 * i + 3] = vertices_m1[18 * i + 9] = vertices_m1[18 * i + 15] =
            myModel.face_norms[i].x;
        vertices_m1[18 * i + 4] = vertices_m1[18 * i + 10] = vertices_m1[18 * i + 16] =
            myModel.face_norms[i].y;
        vertices_m1[18 * i + 5] = vertices_m1[18 * i + 11] = vertices_m1[18 * i + 17] =
            myModel.face_norms[i].z;
    }

    //mode 2 vertices
    GLfloat* vertices_m2 = new GLfloat[18 * myModel.nf]; // each face has three vertices, each vertice has 3-d pos coords + 3-d face normals
    for (int i = 0; i < myModel.nf; i++) {
        //fill in vertex positions 
        vertices_m2[18 * i] = myModel.vertices[myModel.faces[i].v1 - 1].x;
        vertices_m2[18 * i + 1] = myModel.vertices[myModel.faces[i].v1 - 1].y;
        vertices_m2[18 * i + 2] = myModel.vertices[myModel.faces[i].v1 - 1].z;

        vertices_m2[18 * i + 6] = myModel.vertices[myModel.faces[i].v2 - 1].x;
        vertices_m2[18 * i + 7] = myModel.vertices[myModel.faces[i].v2 - 1].y;
        vertices_m2[18 * i + 8] = myModel.vertices[myModel.faces[i].v2 - 1].z;

        vertices_m2[18 * i + 12] = myModel.vertices[myModel.faces[i].v3 - 1].x;
        vertices_m2[18 * i + 13] = myModel.vertices[myModel.faces[i].v3 - 1].y;
        vertices_m2[18 * i + 14] = myModel.vertices[myModel.faces[i].v3 - 1].z;

        //fill in vertex normals
        vertices_m2[18 * i + 3] = myModel.vertex_norms[myModel.faces[i].v1 - 1].x;
        vertices_m2[18 * i + 4] = myModel.vertex_norms[myModel.faces[i].v1 - 1].y;
        vertices_m2[18 * i + 5] = myModel.vertex_norms[myModel.faces[i].v1 - 1].z;

        vertices_m2[18 * i + 9] = myModel.vertex_norms[myModel.faces[i].v2 - 1].x;
        vertices_m2[18 * i + 10] = myModel.vertex_norms[myModel.faces[i].v2 - 1].y;
        vertices_m2[18 * i + 11] = myModel.vertex_norms[myModel.faces[i].v2 - 1].z;

        vertices_m2[18 * i + 15] = myModel.vertex_norms[myModel.faces[i].v3 - 1].x;
        vertices_m2[18 * i + 16] = myModel.vertex_norms[myModel.faces[i].v3 - 1].y;
        vertices_m2[18 * i + 17] = myModel.vertex_norms[myModel.faces[i].v3 - 1].z;
    }

    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    GLuint lightVBO, lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(lightVAO);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);


    //bind buffers for m1
    GLuint VBO_m1, VAO_m1;
    glGenVertexArrays(1, &VAO_m1);
    glGenBuffers(1, &VBO_m1);
    glBindVertexArray(VAO_m1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_m1);
    glBufferData(GL_ARRAY_BUFFER, 18 * myModel.nf * sizeof(GLfloat), vertices_m1, GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0); // Unbind VAO

    //bind buffers for m2
    GLuint VBO_m2, VAO_m2;
    glGenVertexArrays(1, &VAO_m2);
    glGenBuffers(1, &VBO_m2);
    glBindVertexArray(VAO_m2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_m2);
    glBufferData(GL_ARRAY_BUFFER, 18 * myModel.nf * sizeof(GLfloat), vertices_m2, GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
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
        interact_model();
        
        // Clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use cooresponding shader when setting uniforms/drawing objects
        lightingShader.Use();
        GLint lightPosLoc    = glGetUniformLocation(lightingShader.Program, "light.position");
        GLint viewPosLoc     = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);
        
        // light properties
        GLint lightAmbientLoc = glGetUniformLocation(lightingShader.Program, "light.ambient");
        GLint lightDiffuseLoc = glGetUniformLocation(lightingShader.Program, "light.diffuse");
        GLint lightSpecularLoc = glGetUniformLocation(lightingShader.Program, "light.specular");
        glm::vec3 diffuseColor = lightColor * glm::vec3(lightDiffuseStrength); // decrease the influence
        glm::vec3 ambientColor = lightColor * glm::vec3(lightAmbientStrength); // low influence
        glm::vec3 specularColor = lightColor * glm::vec3(lightSpecularStrength);
        glUniform3f(lightAmbientLoc, ambientColor.x, ambientColor.y, ambientColor.z);
        glUniform3f(lightDiffuseLoc, diffuseColor.x, diffuseColor.y, diffuseColor.z);
        glUniform3f(lightSpecularLoc, specularColor.x, specularColor.y, specularColor.z);
         
        
        // material properties
        GLint matAmbientLoc = glGetUniformLocation(lightingShader.Program, "material.ambient");
        GLint matDiffuseLoc = glGetUniformLocation(lightingShader.Program, "material.diffuse");
        GLint matSpecularLoc = glGetUniformLocation(lightingShader.Program, "material.specular");
        GLint matShineLoc = glGetUniformLocation(lightingShader.Program, "material.shininess");

        glm::vec3 matDiffCol = matColor * glm::vec3(matKd);
        glm::vec3 matAmbientCol = matColor * glm::vec3(matKa);
        glm::vec3 matSpecularCol = matColor * glm::vec3(matKs);
        
        glUniform3f(matAmbientLoc, matAmbientCol.x, matAmbientCol.y, matAmbientCol.z);
        glUniform3f(matDiffuseLoc, matDiffCol.x, matDiffCol.y, matDiffCol.z);
        glUniform3f(matSpecularLoc, matSpecularCol.x, matSpecularCol.y, matSpecularCol.z);
        glUniform1f(matShineLoc, mat_n);
        
        
        // Create camera transformations
        glm::mat4 view(1);
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc  = glGetUniformLocation(lightingShader.Program,  "view");
        GLint projLoc  = glGetUniformLocation(lightingShader.Program,  "projection");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // Draw the object
        if (currentMode == 0) {
            glBindVertexArray(VAO_m1);
        }
        else {
            glBindVertexArray(VAO_m2);
        }

        // Calculate the model matrix and pass it to shader before drawing
        glm::mat4 model(1);
        model = glm::translate(model, position);
        model = glm::rotate(model, x_angle, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, y_angle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, z_angle, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        glDrawArrays(GL_TRIANGLES, 0, myModel.nf*3);
        glBindVertexArray(0);
        
        // Also draw the lamp object, again binding the appropriate shader
        lampShader.Use();
        // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
        modelLoc = glGetUniformLocation(lampShader.Program, "model");
        viewLoc  = glGetUniformLocation(lampShader.Program, "view");
        projLoc  = glGetUniformLocation(lampShader.Program, "projection");
        GLint lightColLoc = glGetUniformLocation(lampShader.Program, "lightCol");
        // Set matrices
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(lightColLoc,lightColor.x, lightColor.y, lightColor.z);
        model = glm::mat4(1);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // Draw the light object (using light's vertex attributes)
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    
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
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }

    //change display mode
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        currentMode = 1 - currentMode;
    }
   
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void interact_model() {
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

    // light source translate control
    if (keys[GLFW_KEY_R]) //move forward
        lightPos.z += tspeed;
    if (keys[GLFW_KEY_T]) //move backward
        lightPos.z -= tspeed;
    if (keys[GLFW_KEY_F]) //move right
        lightPos.x += tspeed;
    if (keys[GLFW_KEY_G]) // move left
        lightPos.x -= tspeed;
    if (keys[GLFW_KEY_V]) //move up
        lightPos.y -= tspeed;
    if (keys[GLFW_KEY_B]) //move down
        lightPos.y += tspeed;

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

    // light source parameter control
    if (keys[GLFW_KEY_1])
        lightColor.x += 0.1;
    if (keys[GLFW_KEY_2])
        lightColor.y += 0.1;
    if (keys[GLFW_KEY_3])
        lightColor.z += 0.1;
    if (keys[GLFW_KEY_4])
        lightAmbientStrength += 0.1;
    if (keys[GLFW_KEY_5])
        lightDiffuseStrength += 0.1;
    if (keys[GLFW_KEY_6])
        lightSpecularStrength += 0.1;

    // material parameter control
    if (keys[GLFW_KEY_0])
        matColor.x += 0.1;
    if (keys[GLFW_KEY_9])
        matColor.y += 0.1;
    if (keys[GLFW_KEY_8])
        matColor.z += 0.1;
    if (keys[GLFW_KEY_UP])
        matKa += 0.1;
    if (keys[GLFW_KEY_DOWN])
        matKd += 0.1;
    if (keys[GLFW_KEY_SLASH])
        matKs += 0.1;
    if (keys[GLFW_KEY_LEFT])
        mat_n += 1.0;
    if (keys[GLFW_KEY_RIGHT])
        mat_n -= 1.0;

    if (lightColor.x > 1.0)
        lightColor.x = 0.0f;
    if (lightColor.y > 1.0)
        lightColor.y = 0.0f;
    if (lightColor.z > 1.0)
        lightColor.z = 0.0f;
    if (lightAmbientStrength > 1.0)
        lightAmbientStrength = 0.0f;
    if (lightDiffuseStrength > 1.0)
        lightDiffuseStrength = 0.0f;
    if (lightSpecularStrength > 1.0)
        lightSpecularStrength = 0.0f;

    if (matColor.x > 1.0)
        matColor.x = 0.0f;
    if (matColor.y > 1.0)
        matColor.y = 0.0f;
    if (matColor.z > 1.0)
        matColor.z = 0.0f;
    if (matKa > 1.0)
        matKa = 0.0f;
    if (matKd > 1.0)
        matKd = 0.0f;
    if (matKs > 1.0)
        matKs = 0.0f;
}