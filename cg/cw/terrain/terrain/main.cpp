// Std. Includes
#include <string>
#include <map>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "tut_headers/shader.h"
#include "tut_headers/camera.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL/src/SOIL.h>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

// Properties
GLuint WIDTH = 800, HEIGHT = 600;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
GLuint loadTexture(const char* path);
GLuint loadWaterTexture(const char* path);
void setupSkyboxBuffers(GLuint& VAO, GLuint& VBO, GLfloat* vertexBuffer, int buffSize);
void setupWaterBuffer(GLuint& VAO, GLuint& VBO, GLfloat* vertexBuffer, int buffSize);
void setupTerrainBuffers(GLuint& VAO, GLuint& VBO, GLfloat* vertexBuffer, int buffSize);
void setForbiddenRegion();
void updateWaterWaves();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastPressed[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat scale = 30.f;

// wave params
GLfloat xshift = 0.0;
GLfloat waterSpeed = 0.01 * scale;


// for setting forbidden region
GLfloat heightmap[512][512];

// for 3d wave effect
int wave3d = 1;
const int waterPix = 256;
float damping = 0.1;
GLfloat waterHeightmap[waterPix][waterPix];
GLfloat* waterMeshPos = new GLfloat[waterPix * waterPix * 3]; // (x, y, z), y is water height
int numWaterVertices = (waterPix - 1) * (waterPix - 1) * 6 * 5; // numPatches, each patch 2 triangles, each vertex 5 coords
GLfloat* waterMesh = new GLfloat[numWaterVertices];
GLuint water3dVAO, water3dVBO;
GLfloat freq = 0.8;
GLfloat phase = 1;
GLfloat amp = 0.0005 * scale;


// The MAIN function, from here we start our application and run our Game loop
int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Terrain Engine", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setup and compile our shaders
    Shader skyboxShader("shaders/sky.vert.glsl", "shaders/sky.frag.glsl");
    Shader shaderWave("shaders/wave.vert.glsl", "shaders/wave.frag.glsl");
    Shader terrainShader("shaders/terrain.vert.glsl", "shaders/terrain.frag.glsl");

    // -------------- Set the object data (buffers, vertex attributes) ------------------

    // ---------- skybox ----------- 
    //skybox0
    GLfloat skyFrontVertices[30] = { 
        // Positions          // Texture Coords
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, //front
        1.0f, -1.0f,  1.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
    };

    //skybox1
    GLfloat skyRightVertices[30] = {
        // Positions          // Texture Coords
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f, //right
        1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,
    };

    //skybox2
    GLfloat skyBackVertices[30] = {
        // Positions          // Texture Coords
        -1.0f, -1.0f, -1.0f,  1.0f,  0.0f, //back
        1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  1.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,  1.0f,  0.0f
    };

    //skybox3
    GLfloat skyLeftVertices[30] = {
        // Positions          // Texture Coords
         -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, //left
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  1.0f,  1.0f
    };

    //skybox4
    GLfloat skyTopVertices[30] = {
        // Positions          // Texture Coords
         -1.0f,  1.0f, -1.0f,  0.0f,  1.0f, //top
        1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f
    };
    
    //------------------Setup skybox buffers-----------------
    GLuint skyFrontVAO, skyFrontVBO;
    setupSkyboxBuffers(skyFrontVAO, skyFrontVBO, skyFrontVertices, 30);

    GLuint skyRightVAO, skyRightVBO;
    setupSkyboxBuffers(skyRightVAO, skyRightVBO, skyRightVertices, 30);

    GLuint skyBackVAO, skyBackVBO;
    setupSkyboxBuffers(skyBackVAO, skyBackVBO, skyBackVertices, 30);

    GLuint skyLeftVAO, skyLeftVBO;
    setupSkyboxBuffers(skyLeftVAO, skyLeftVBO, skyLeftVertices, 30);

    GLuint skyTopVAO, skyTopVBO;
    setupSkyboxBuffers(skyTopVAO, skyTopVBO, skyTopVertices, 30);


    // ----------------------- terrain ----------------------------- 
    int width, height, channels;
    unsigned char* heightImage = SOIL_load_image("textures/heightmap.bmp", &width, &height, &channels, SOIL_LOAD_L);
    GLfloat* heightInfo = new GLfloat[width * height * 5]; // (x, y, z, terrain1, terrain2), y is height
    GLfloat x, y, z, t1, t2;
    int i = 0;
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            y = heightImage[r * width + c]; //raw height value

            //normalise height
            y = y / 127.5;
            y = (y < 0.41) ? 0.41 : y; // not sure how value was chosen

            //pos and texture coords
            x = r * 1.0 / height;
            y = y / scale * 3.0;
            z = c * 1.0 / width;
            t1 = r * 1.0 / height;
            t2 = c * 1.0 / width;

            heightmap[r][c] = y;
           
            heightInfo[i++] = x;
            heightInfo[i++] = y;
            heightInfo[i++] = z;
            heightInfo[i++] = t1;
            heightInfo[i++] = t2;
        }
    }

    SOIL_free_image_data(heightImage);

    //link adjacent vertices together into triangles 
    // also add detail texcoords
    int numIslandVertices = (width - 1) * (height - 1) * 6 * 7; //patch count * 6 (2 triangles per terrain patch) * 7 (x,y,z,t1,t2,d1,d2)
    GLfloat* islandVertices = new GLfloat[numIslandVertices]; 
    int j = 0;
    for (int r = 0; r < height - 1; r++) {
        for (int c = 0; c < width - 1; c++) {

            int start = r * width + c;

            // triangle 1, vertex 1
            islandVertices[j++] = heightInfo[start * 5];
            islandVertices[j++] = heightInfo[start * 5 + 1];
            islandVertices[j++] = heightInfo[start * 5 + 2];
            islandVertices[j++] = heightInfo[start * 5 + 3];
            islandVertices[j++] = heightInfo[start * 5 + 4];
            islandVertices[j++] = 0.0f;
            islandVertices[j++] = 0.0f;

            // triangle 1, vertex 2
            islandVertices[j++] = heightInfo[(start + width) * 5];
            islandVertices[j++] = heightInfo[(start + width) * 5 + 1];
            islandVertices[j++] = heightInfo[(start + width) * 5 + 2];
            islandVertices[j++] = heightInfo[(start + width) * 5 + 3];
            islandVertices[j++] = heightInfo[(start + width) * 5 + 4];
            islandVertices[j++] = 0.0f;
            islandVertices[j++] = 1.0f;

            // triangle 1, vertex 3
            islandVertices[j++] = heightInfo[(start + width + 1) * 5];
            islandVertices[j++] = heightInfo[(start + width + 1) * 5 + 1];
            islandVertices[j++] = heightInfo[(start + width + 1) * 5 + 2];
            islandVertices[j++] = heightInfo[(start + width + 1) * 5 + 3];
            islandVertices[j++] = heightInfo[(start + width + 1) * 5 + 4];
            islandVertices[j++] = 1.0f;
            islandVertices[j++] = 1.0f;


            // triangle 2, vertex 1
            islandVertices[j++] = heightInfo[(start + width + 1) * 5];
            islandVertices[j++] = heightInfo[(start + width + 1) * 5 + 1];
            islandVertices[j++] = heightInfo[(start + width + 1) * 5 + 2];
            islandVertices[j++] = heightInfo[(start + width + 1) * 5 + 3];
            islandVertices[j++] = heightInfo[(start + width + 1) * 5 + 4];
            islandVertices[j++] = 1.0f;
            islandVertices[j++] = 1.0f;

            // triangle 2, vertex 2
            islandVertices[j++] = heightInfo[(start + 1) * 5];
            islandVertices[j++] = heightInfo[(start + 1) * 5 + 1];
            islandVertices[j++] = heightInfo[(start + 1) * 5 + 2];
            islandVertices[j++] = heightInfo[(start + 1) * 5 + 3];
            islandVertices[j++] = heightInfo[(start + 1) * 5 + 4];
            islandVertices[j++] = 1.0f;
            islandVertices[j++] = 0.0f;

            //triangle 2, vertex 3
            islandVertices[j++] = heightInfo[start * 5];
            islandVertices[j++] = heightInfo[start * 5 + 1];
            islandVertices[j++] = heightInfo[start * 5 + 2];
            islandVertices[j++] = heightInfo[start * 5 + 3];
            islandVertices[j++] = heightInfo[start * 5 + 4];
            islandVertices[j++] = 0.0f;
            islandVertices[j++] = 0.0f;
        }
    }


    // --------- set up terrain buffers --------------
    GLuint islandVAO, islandVBO;
    setupTerrainBuffers(islandVAO, islandVBO, islandVertices, numIslandVertices);


    // ------------------- water 3D mesh ---------------------
    i = 0;
    for (int r = 0; r < waterPix; r++) {
        for (int c = 0; c < waterPix; c++) {

            x = r * 1.0 / waterPix * 20.0 - 10.0;
            z = c * 1.0 / waterPix * 20.0 - 10.0;
            y = 0;

            waterMeshPos[i++] = x;
            waterMeshPos[i++] = y;
            waterMeshPos[i++] = z;

            waterHeightmap[r][c] = y;
        }
    }

    
    j = 0;
    for (int r = 0; r < waterPix - 1; r++) {
        for (int c = 0; c < waterPix - 1; c++) {

            int start = r * waterPix + c;

            // triangle 1, vertex 1
            waterMesh[j++] = waterMeshPos[start * 3];
            waterMesh[j++] = waterMeshPos[start * 3 + 1];
            waterMesh[j++] = waterMeshPos[start * 3 + 2];
            waterMesh[j++] = 0.0f;
            waterMesh[j++] = 0.0f;

            // triangle 1, vertex 2
            waterMesh[j++] = waterMeshPos[(start + waterPix) * 3];
            waterMesh[j++] = waterMeshPos[(start + waterPix) * 3 + 1];
            waterMesh[j++] = waterMeshPos[(start + waterPix) * 3 + 2];
            waterMesh[j++] = 0.0f;
            waterMesh[j++] = 1.0f;

            // triangle 1, vertex 3
            waterMesh[j++] = waterMeshPos[(start + waterPix + 1) * 3];
            waterMesh[j++] = waterMeshPos[(start + waterPix + 1) * 3 + 1];
            waterMesh[j++] = waterMeshPos[(start + waterPix + 1) * 3 + 2];
            waterMesh[j++] = 1.0f;
            waterMesh[j++] = 1.0f;


            // triangle 2, vertex 1
            waterMesh[j++] = waterMeshPos[(start + waterPix + 1) * 3];
            waterMesh[j++] = waterMeshPos[(start + waterPix + 1) * 3 + 1];
            waterMesh[j++] = waterMeshPos[(start + waterPix + 1) * 3 + 2];
            waterMesh[j++] = 1.0f;
            waterMesh[j++] = 1.0f;

            // triangle 2, vertex 2
            waterMesh[j++] = waterMeshPos[(start + 1) * 3];
            waterMesh[j++] = waterMeshPos[(start + 1) * 3 + 1];
            waterMesh[j++] = waterMeshPos[(start + 1) * 3 + 2];
            waterMesh[j++] = 1.0f;
            waterMesh[j++] = 0.0f;

            //triangle 2, vertex 3
            waterMesh[j++] = waterMeshPos[start * 3];
            waterMesh[j++] = waterMeshPos[start * 3 + 1];
            waterMesh[j++] = waterMeshPos[start * 3 + 2];
            waterMesh[j++] = 0.0f;
            waterMesh[j++] = 0.0f;
        }
    }

    //------------------Setup water3d buffer-----------------
    setupWaterBuffer(water3dVAO, water3dVBO, waterMesh, numWaterVertices);


    // ---------------- Load textures -----------------------
    GLuint skyFrontTexture = loadTexture("textures/SkyBox0.bmp");
    GLuint skyRightTexture = loadTexture("textures/SkyBox1.bmp");
    GLuint skyBackTexture = loadTexture("textures/SkyBox2.bmp");
    GLuint skyLeftTexture = loadTexture("textures/SkyBox3.bmp");
    GLuint skyTopTexture = loadTexture("textures/SkyBox4.bmp");
    GLuint skySeaTexture = loadWaterTexture("textures/SkyBox5.bmp");
    GLuint islandTexture = loadTexture("textures/terrain-texture3.bmp");
    GLuint detailTexture = loadTexture("textures/detail.bmp");
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        setForbiddenRegion();
        Do_Movement();
        updateWaterWaves();
        
        // Clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        // ---------------- Draw objects -----------------
        glm::mat4 projection(1);
        glm::mat4 model(1);
        glm::mat4 view(1);
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);


        // ------ Skybox --------
        skyboxShader.Use();
        model = glm::translate(model, glm::vec3(0.0f, 0.9f * scale, 0.0f)); //skybox slightly above zero
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // skybox remains static despite camera mvt
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(glGetUniformLocation(skyboxShader.Program, "scale"), scale);
        glUniform1i(glGetUniformLocation(skyboxShader.Program, "rev"), 0);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        //glDepthMask(GL_FALSE);
        //Back
        glBindVertexArray(skyBackVAO);
        glBindTexture(GL_TEXTURE_2D, skyBackTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //Front
        glBindVertexArray(skyFrontVAO);
        glBindTexture(GL_TEXTURE_2D, skyFrontTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //Left
        glBindVertexArray(skyLeftVAO);
        glBindTexture(GL_TEXTURE_2D, skyLeftTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //Right
        glBindVertexArray(skyRightVAO);
        glBindTexture(GL_TEXTURE_2D, skyRightTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //Top
        glBindVertexArray(skyTopVAO);
        glBindTexture(GL_TEXTURE_2D, skyTopTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // -------- draw reversed skybox ----------
        glUniform1i(glGetUniformLocation(skyboxShader.Program, "rev"), 1);
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0.0f, -1.1f * scale, 0.0f)); // reverse skybox slightly below zero
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        //Skybox
        //Back
        glBindVertexArray(skyBackVAO);
        glBindTexture(GL_TEXTURE_2D, skyBackTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //Front
        glBindVertexArray(skyFrontVAO);
        glBindTexture(GL_TEXTURE_2D, skyFrontTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //Left
        glBindVertexArray(skyLeftVAO);
        glBindTexture(GL_TEXTURE_2D, skyLeftTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //Right
        glBindVertexArray(skyRightVAO);
        glBindTexture(GL_TEXTURE_2D, skyRightTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //Top
        glBindVertexArray(skyTopVAO);
        glBindTexture(GL_TEXTURE_2D, skyTopTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        //glDepthMask(GL_TRUE);


        // ----------- Draw terrain --------------
        terrainShader.Use();
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0.0f, -0.05f * scale, 0.0f)); // shift terrain slightly below water
        view = camera.GetViewMatrix(); // view terrain from camera location
        glUniformMatrix4fv(glGetUniformLocation(terrainShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(terrainShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(glGetUniformLocation(terrainShader.Program, "scale"), scale);
        glUniformMatrix4fv(glGetUniformLocation(terrainShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(terrainShader.Program, "rev"), 0);

        glBindVertexArray(islandVAO);

        glUniform1i(glGetUniformLocation(terrainShader.Program, "texture0"), 0);
        glUniform1i(glGetUniformLocation(terrainShader.Program, "texture1"), 1);
        
        glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, islandTexture);

        glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
        glBindTexture(GL_TEXTURE_2D, detailTexture);

        glDrawArrays(GL_TRIANGLES, 0, numIslandVertices);

        glDisable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE1);
        glDisable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glDisable(GL_TEXTURE_2D);

        // ---------- draw reversed terrain -------------
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0.0f, 0.04f * scale, 0.0f)); // shift terrain slightly above water
        glUniformMatrix4fv(glGetUniformLocation(terrainShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(terrainShader.Program, "rev"), 1);

        glUniform1i(glGetUniformLocation(terrainShader.Program, "texture0"), 0);
        glUniform1i(glGetUniformLocation(terrainShader.Program, "texture1"), 1);

        glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, islandTexture);

        glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
        glBindTexture(GL_TEXTURE_2D, detailTexture);

        glBindVertexArray(islandVAO);
        glDrawArrays(GL_TRIANGLES, 0, numIslandVertices);

        glDisable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE1);
        glDisable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glDisable(GL_TEXTURE_2D);



        // -------------- water waves -------------------
        shaderWave.Use();
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0.0f, 0.002f * scale, 0.0f)); // water in between normal and reversed skyboxes
        glUniformMatrix4fv(glGetUniformLocation(shaderWave.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderWave.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderWave.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        xshift += waterSpeed*deltaTime;
        glUniform1f(glGetUniformLocation(shaderWave.Program, "xshift"), xshift);
        glUniform1f(glGetUniformLocation(shaderWave.Program, "scale"), scale);
        //Sea
        glBindVertexArray(water3dVAO);
        glBindTexture(GL_TEXTURE_2D, skySeaTexture);
        
        glDrawArrays(GL_TRIANGLES, 0, numWaterVertices);

        glBindVertexArray(0);
        
        // Swap the buffers
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}


// Moves/alters the camera positions based on user input
void Do_Movement()
{   
    // water 3d waves
    if (keys[GLFW_KEY_C]) {
        wave3d = 1 - wave3d;
    }
    
    // Camera controls
    if (keys[GLFW_KEY_W]) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        lastPressed[0] = lastFrame;
    }
    if (keys[GLFW_KEY_S]) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        lastPressed[1] = lastFrame;
    }
    if (keys[GLFW_KEY_A]) {
        camera.ProcessKeyboard(LEFT, deltaTime);
        lastPressed[2] = lastFrame;
    }
    if (keys[GLFW_KEY_D]) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        lastPressed[3] = lastFrame;
    }
    if (keys[GLFW_KEY_Z]) {
        camera.ProcessKeyboard(UP, deltaTime);
        lastPressed[4] = lastFrame;
    }
    if (keys[GLFW_KEY_X]) {
        camera.ProcessKeyboard(DOWN, deltaTime);
        lastPressed[5] = lastFrame;
    }

    if (lastFrame - lastPressed[0] < 100 * deltaTime)
        camera.ProcessKeyboard(FORWARD, 0.1 * deltaTime / ((lastFrame - lastPressed[0] + 0.1)));

    if (lastFrame - lastPressed[1] < 100 * deltaTime)
        camera.ProcessKeyboard(BACKWARD, 0.1 * deltaTime / ((lastFrame - lastPressed[1] + 0.1)));

    if (lastFrame - lastPressed[2] < 100 * deltaTime)
        camera.ProcessKeyboard(LEFT, 0.1 * deltaTime / ((lastFrame - lastPressed[2] + 0.1)));

    if (lastFrame - lastPressed[3] < 100 * deltaTime)
        camera.ProcessKeyboard(RIGHT, 0.1 * deltaTime / ((lastFrame - lastPressed[3] + 0.1)));

    if (lastFrame - lastPressed[4] < 100 * deltaTime)
        camera.ProcessKeyboard(UP, 0.1 * deltaTime / ((lastFrame - lastPressed[4] + 0.1)));

    if (lastFrame - lastPressed[5] < 100 * deltaTime)
        camera.ProcessKeyboard(DOWN, 0.1 * deltaTime / ((lastFrame - lastPressed[5] + 0.1)));
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
    GLfloat yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

GLuint loadTexture(const char* path)
{
    //Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);

    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);


    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);    // Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    SOIL_free_image_data(image);
    return textureID;
}

GLuint loadWaterTexture(const char* path)
{
    //Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);

    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    glGenerateMipmap(GL_TEXTURE_2D);


    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    SOIL_free_image_data(image);
    return textureID;
}

void setupSkyboxBuffers(GLuint& VAO, GLuint& VBO, GLfloat* vertexBuffer, int buffSize)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*buffSize, &vertexBuffer[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);
}

void setupWaterBuffer(GLuint& VAO, GLuint& VBO, GLfloat* vertexBuffer, int buffSize)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * buffSize, &vertexBuffer[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);
}

void setupTerrainBuffers(GLuint& VAO, GLuint& VBO, GLfloat* vertexBuffer, int buffSize)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * buffSize, &vertexBuffer[0], GL_STATIC_DRAW);
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
    // TexCoord attribute terrain
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    // TexCoord attribute details
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glBindVertexArray(0); // Unbind VAO
}

void setForbiddenRegion()
{
    GLfloat cam_y = camera.Position.y;
    if (cam_y < 1.0) {
        //keeps camera above sea level
        camera.Position.y = 1.0;
    }
    else {
        //prevents user from getting inside the terrain
        if (camera.Position.x > 0.0 - 0.1 && camera.Position.z > 0.0 - 0.1 && (camera.Position.x < scale + 3) && (camera.Position.z < scale + 3))
        {
            int x_ind = std::round(camera.Position.x / scale * 256);
            int z_ind = std::round(camera.Position.z / scale * 256);
            GLfloat ter_height = heightmap[x_ind][z_ind] * scale;
            if (cam_y - ter_height - 0.2 < 0.0) {
                camera.Position.y = ter_height + 0.2;
            }
        }
    }
}

void updateWaterWaves()
{
    float x, z;
    for (int r = 0; r < waterPix; r++) {
        for (int c = 0; c < waterPix; c++) {
            if (wave3d == 1) {
                x = 4 * r;
                z = 4 * c;
                waterHeightmap[r][c] = amp * 0.5 * (sin(x * freq + lastFrame * phase) + cos(z * freq + lastFrame * phase));
            }
            else {
                waterHeightmap[r][c] = 0;
            }
        }
    }

    // update the y coords of water vertex buffer
    int r, c;
    for (int i = 1; i < numWaterVertices; i += 5) {
        r = std::round((waterMesh[i - 1] + 10.0) / 20.0 * waterPix);
        c = std::round((waterMesh[i + 1] + 10.0) / 20.0 * waterPix);
        waterMesh[i] = waterHeightmap[r][c];
    }

    glBindVertexArray(water3dVAO);
    glBindBuffer(GL_ARRAY_BUFFER, water3dVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*numWaterVertices, &waterMesh[0]);
    glBindVertexArray(0);
}