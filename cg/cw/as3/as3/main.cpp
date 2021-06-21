// Std. Includes
#include <string>
#include <map>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// GL includes
#include "tut_headers/shader.h"
#include "tut_headers/camera.h"
#include "Sphere.h"

// Other Libs
#include <SOIL/src/SOIL.h>

// Properties
GLuint WIDTH = 800, HEIGHT = 600;
GLuint textVAO, textVBO;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void Do_Movement();
GLuint loadTexture(const char* path, GLboolean alpha = false);
void RenderText(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};
std::map<GLchar, Character> Characters;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//Movement
float PlanetSpeed = .2f;
GLfloat SceneRotateY = 0.0f;
GLfloat SceneRotateX = 0.0f;
GLfloat SceneRotateZ = 0.0f;

//Planet names
glm::vec3 PlanetsPositions[10];
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
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Planets", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    
    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Setup some OpenGL options
	glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Setup and compile our shaders
    Shader shader("shaders/main.vert.glsl", "shaders/main.frag.glsl");
	Shader textShader("shaders/text.vert.glsl", "shaders/text.frag.glsl");
    
    // Set the object data (buffers, vertex attributes)
    // LOAD TEXTURES 
    GLuint texture_earth = loadTexture("Texture/earth.jpg");
    GLuint texture_sun = loadTexture("Texture/sun.jpg");
    GLuint texture_mercury = loadTexture("Texture/mercury.jpg");
    GLuint texture_venus = loadTexture("Texture/venus.jpg");
    GLuint texture_mars = loadTexture("Texture/mars.jpg");
    GLuint texture_jupiter = loadTexture("Texture/jupiter.jpg");
    GLuint texture_saturn = loadTexture("Texture/saturn.jpg");
    GLuint texture_uranus = loadTexture("Texture/uranus.jpg");
    GLuint texture_neptune = loadTexture("Texture/neptune.jpg");
	GLuint texture_moon = loadTexture("Texture/moon.jpg");
    

    // SPHERE GENERATION (sizes determined by radius)
	float factor = 500.0f; //arbitrarily determined so that all planets visible in window
    float shrink = 0.5; // arbitrarily determined to scale planet names to visibility
    Sphere Sun(100.0f/factor, 36 * 5, 18 * 5);
    Sphere Mercury(10.0f / factor, 36, 18);
    Sphere Venus(12.0f / factor, 36, 18);
    Sphere Earth(11.8f / factor, 36, 18);
    Sphere Mars(8.0f / factor, 36, 18);
    Sphere Jupiter(40.0f / factor, 36, 18);
    Sphere Saturn(37.0f / factor, 36, 18);
    Sphere Uranus(30.0f / factor, 36, 18);
    Sphere Neptune(30.0f / factor, 36, 19);
    Sphere Moon(5.5f / factor, 36, 18);
    /* SPHERE GENERATION */

	/*TEXT GENERATION*/
    glm::mat4 projectionText = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT));
    textShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionText));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "font/arial.ttf", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            GLuint(face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    /*TEXT GENERATION*/
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check and call events
        glfwPollEvents();
        Do_Movement();
        
        // Clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw objects
        shader.Use();
        glm::mat4 model(1);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_sun);

		glm::vec3 point = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 tmpPoint = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec4 centrepos;

		/* SUN */
		glm::mat4 model_sun;
		model_sun = glm::rotate(model_sun, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_sun = glm::rotate(model_sun, (GLfloat)glfwGetTime() * glm::radians(23.5f) * 0.25f, glm::vec3(0.0f, 0.0f, 1.f));
		model_sun = glm::translate(model_sun, point);
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_sun));
		Sun.Draw();
		/* SUN */
        centrepos = projection * view * model_sun * glm::vec4(tmpPoint, 1.0f);
        PlanetsPositions[0] = glm::vec3(centrepos.x, centrepos.y, centrepos.z) / centrepos.w;

		/* MERCURY */
		glm::mat4 model_mercury;
		double xx = sin(glfwGetTime() * PlanetSpeed) * 100.0f*2.0f/factor;
		double zz = cos(glfwGetTime() * PlanetSpeed) * 100.0f*2.0f/factor;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_mercury);
		model_mercury = glm::translate(model_mercury, point);
		model_mercury = glm::rotate(model_mercury, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_mercury = glm::rotate(model_mercury, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
        model_mercury = glm::rotate(model_mercury, glm::radians(SceneRotateZ), glm::vec3(0.0f, 1.0f, 1.0f));
		model_mercury = glm::translate(model_mercury, glm::vec3(xx, 0.0f, zz));
		model_mercury = glm::rotate(model_mercury, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_mercury = glm::rotate(model_mercury, (GLfloat)glfwGetTime() * glm::radians(-90.0f) * 0.05f, glm::vec3(0.0f, 0.0f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_mercury));
		Mercury.Draw();
		/* MERCURY */
        centrepos = projection * view * model_mercury * glm::vec4(tmpPoint, 1.0f);
        PlanetsPositions[1] = glm::vec3(centrepos.x, centrepos.y, centrepos.z) / centrepos.w;

		/* VENUS */
		glm::mat4 model_venus;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.75f) * 100.0f * 3.0f /factor;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.75f) * 100.0f * 3.0f / factor;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_venus);
		model_venus = glm::translate(model_venus, point);
		model_venus = glm::rotate(model_venus, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_venus = glm::rotate(model_venus, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
        model_venus = glm::rotate(model_venus, glm::radians(SceneRotateZ), glm::vec3(0.0f, 1.0f, 1.0f));
		model_venus = glm::translate(model_venus, glm::vec3(xx, 0.0f, zz));
		model_venus = glm::rotate(model_venus, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_venus = glm::rotate(model_venus, glm::radians(-132.5f), glm::vec3(0.0f, 1.0f, 0.f));
		model_venus = glm::rotate(model_venus, (GLfloat)glfwGetTime() * glm::radians(-132.5f) * 0.012f, glm::vec3(0.0f, 0.0f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_venus));
		Venus.Draw();
		/* VENUS */
        centrepos = projection * view * model_venus * glm::vec4(tmpPoint, 1.0f);
        PlanetsPositions[2] = glm::vec3(centrepos.x, centrepos.y, centrepos.z) / centrepos.w;

		/* EARTH */
		glm::mat4 model_earth;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.55f) * 100.0f * 4.0f / factor;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.55f) * 100.0f * 4.0f / factor;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_earth);
		model_earth = glm::translate(model_earth, point);
		model_earth = glm::rotate(model_earth, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_earth = glm::rotate(model_earth, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
        model_earth = glm::rotate(model_earth, glm::radians(SceneRotateZ), glm::vec3(0.0f, 1.0f, 1.0f));
		model_earth = glm::translate(model_earth, glm::vec3(xx, 0.0f, zz));
		glm::vec3 EarthPoint = glm::vec3(xx, 0.0f, zz);
		model_earth = glm::rotate(model_earth, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_earth = glm::rotate(model_earth, glm::radians(-33.25f), glm::vec3(0.0f, 1.0f, 0.f));
		model_earth = glm::rotate(model_earth, (GLfloat)glfwGetTime() * glm::radians(-33.25f) * 2.0f, glm::vec3(0.0f, 0.0f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_earth));
		Earth.Draw();
		/* EARTH */
        centrepos = projection * view * model_earth * glm::vec4(tmpPoint, 1.0f);
        PlanetsPositions[3] = glm::vec3(centrepos.x, centrepos.y, centrepos.z) / centrepos.w;

		/* MOON */
		glm::mat4 model_moon;
		xx = sin(glfwGetTime() * PlanetSpeed * 67.55f) * 100.0f * 0.5f  / factor;
		zz = cos(glfwGetTime() * PlanetSpeed * 67.55f) * 100.0f * 0.5f  / factor;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_moon);
		model_moon = glm::rotate(model_moon, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_moon = glm::rotate(model_moon, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
        model_moon = glm::rotate(model_moon, glm::radians(SceneRotateZ), glm::vec3(0.0f, 1.0f, 1.0f));
		model_moon = glm::translate(model_moon, EarthPoint);
		model_moon = glm::translate(model_moon, glm::vec3(xx, 0.0f, zz));
		model_moon = glm::rotate(model_moon, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_moon = glm::rotate(model_moon, glm::radians(-32.4f), glm::vec3(0.0f, 1.0f, 0.f));
		model_moon = glm::rotate(model_moon, (GLfloat)glfwGetTime() * glm::radians(-32.4f) * 3.1f, glm::vec3(0.0f, 0.0f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_moon));
		Moon.Draw();
		/* MOON */
        centrepos = projection * view * model_moon * glm::vec4(tmpPoint, 1.0f);
        PlanetsPositions[4] = glm::vec3(centrepos.x, centrepos.y, centrepos.z) / centrepos.w;


		/* MARS */
		glm::mat4 model_mars;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.35f) * 100.0f * 5.0f / factor;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.35f) * 100.0f * 5.0f / factor;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_mars);
		model_mars = glm::translate(model_mars, point);
		model_mars = glm::rotate(model_mars, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_mars = glm::rotate(model_mars, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
        model_mars = glm::rotate(model_mars, glm::radians(SceneRotateZ), glm::vec3(0.0f, 1.0f, 1.0f));
		model_mars = glm::translate(model_mars, glm::vec3(xx, 0.0f, zz));
		model_mars = glm::rotate(model_mars, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_mars = glm::rotate(model_mars, glm::radians(-32.4f), glm::vec3(0.0f, 1.0f, 0.f));
		model_mars = glm::rotate(model_mars, (GLfloat)glfwGetTime() * glm::radians(-32.4f) * 2.1f, glm::vec3(0.0f, 0.0f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_mars));
		Mars.Draw();
		/* MARS */
        centrepos = projection * view * model_mars * glm::vec4(tmpPoint, 1.0f);
        PlanetsPositions[5] = glm::vec3(centrepos.x, centrepos.y, centrepos.z) / centrepos.w;

		/* JUPITER */
		glm::mat4 model_jupiter;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.2f) * 100.0f * 6.0f / factor;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.2f) * 100.0f * 6.0f / factor;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_jupiter);
		model_jupiter = glm::translate(model_jupiter, point);
		model_jupiter = glm::rotate(model_jupiter, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_jupiter = glm::rotate(model_jupiter, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
        model_jupiter = glm::rotate(model_jupiter, glm::radians(SceneRotateZ), glm::vec3(0.0f, 1.0f, 1.0f));
		model_jupiter = glm::translate(model_jupiter, glm::vec3(xx, 0.0f, zz));
		model_jupiter = glm::rotate(model_jupiter, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_jupiter = glm::rotate(model_jupiter, glm::radians(-23.5f), glm::vec3(0.0f, 1.0f, 0.f));
		model_jupiter = glm::rotate(model_jupiter, (GLfloat)glfwGetTime() * glm::radians(-23.5f) * 4.5f, glm::vec3(0.0f, 0.0f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_jupiter));
		Jupiter.Draw();
		/* JUPITER */
        centrepos = projection * view * model_jupiter * glm::vec4(tmpPoint, 1.0f);
        PlanetsPositions[6] = glm::vec3(centrepos.x, centrepos.y, centrepos.z) / centrepos.w;

		/* SATURN */
		glm::mat4 model_saturn;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.15f) * 100.0f * 7.0f / factor;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.15f) * 100.0f * 7.0f / factor;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_saturn);
		model_saturn = glm::translate(model_saturn, point);
		model_saturn = glm::rotate(model_saturn, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_saturn = glm::rotate(model_saturn, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
        model_saturn = glm::rotate(model_saturn, glm::radians(SceneRotateZ), glm::vec3(0.0f, 1.0f, 1.0f));
		model_saturn = glm::translate(model_saturn, glm::vec3(xx, 0.0f, zz));
		model_saturn = glm::rotate(model_saturn, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_saturn = glm::rotate(model_saturn, glm::radians(-34.7f), glm::vec3(0.0f, 1.0f, 0.f));
		model_saturn = glm::rotate(model_saturn, (GLfloat)glfwGetTime() * glm::radians(-34.7f) * 4.48f, glm::vec3(0.0f, 0.0f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_saturn));
		Saturn.Draw();
		/* SATURN */
        centrepos = projection * view * model_saturn * glm::vec4(tmpPoint, 1.0f);
        PlanetsPositions[7] = glm::vec3(centrepos.x, centrepos.y, centrepos.z) / centrepos.w;

		/* URANUS */
		glm::mat4 model_uranus;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.1f) * 100.0f * 8.0f / factor;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.1f) * 100.0f * 8.0f / factor;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_uranus);
		model_uranus = glm::translate(model_uranus, point);
		model_uranus = glm::rotate(model_uranus, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_uranus = glm::rotate(model_uranus, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
        model_uranus = glm::rotate(model_uranus, glm::radians(SceneRotateZ), glm::vec3(0.0f, 1.0f, 1.0f));
		model_uranus = glm::translate(model_uranus, glm::vec3(xx, 0.0f, zz));
		model_uranus = glm::rotate(model_uranus, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_uranus = glm::rotate(model_uranus, glm::radians(-99.0f), glm::vec3(0.0f, 1.0f, 0.f));
		model_uranus = glm::rotate(model_uranus, (GLfloat)glfwGetTime() * glm::radians(-99.0f) * 4.5f, glm::vec3(0.0f, 0.0f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_uranus));
		Uranus.Draw();
		/* URANUS */
        centrepos = projection * view * model_uranus * glm::vec4(tmpPoint, 1.0f);
        PlanetsPositions[8] = glm::vec3(centrepos.x, centrepos.y, centrepos.z) / centrepos.w;

		/* NEPTUNE */
		glm::mat4 model_neptune;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.08f) * 100.0f * 9.0f / factor;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.08f) * 100.0f * 9.0f / factor;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_neptune);
		model_neptune = glm::translate(model_neptune, point);
		model_neptune = glm::rotate(model_neptune, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_neptune = glm::rotate(model_neptune, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
        model_neptune = glm::rotate(model_neptune, glm::radians(SceneRotateZ), glm::vec3(0.0f, 1.0f, 1.0f));
		model_neptune = glm::translate(model_neptune, glm::vec3(xx, 0.0f, zz));
		model_neptune = glm::rotate(model_neptune, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_neptune = glm::rotate(model_neptune, glm::radians(-30.2f), glm::vec3(0.0f, 1.0f, 0.f));
		model_neptune = glm::rotate(model_neptune, (GLfloat)glfwGetTime() * glm::radians(-30.2f) * 4.0f, glm::vec3(0.0f, 0.0f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_neptune));
		Neptune.Draw();
		/* NEPTUNE */
        centrepos = projection * view * model_neptune * glm::vec4(tmpPoint, 1.0f);
        PlanetsPositions[9] = glm::vec3(centrepos.x, centrepos.y, centrepos.z) / centrepos.w;

        //Render planet names
        RenderText(textShader, "SUN", (PlanetsPositions[0].x + 1) / 2 * WIDTH, (PlanetsPositions[0].y + 1) / 2 *HEIGHT, 1.0f * shrink, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(textShader, "MERCURY", (PlanetsPositions[1].x + 1) / 2 * WIDTH, (PlanetsPositions[1].y + 1) / 2 * HEIGHT, 0.7f * shrink, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(textShader, "VENUS", (PlanetsPositions[2].x + 1) / 2 * WIDTH, (PlanetsPositions[2].y + 1) / 2 * HEIGHT, 0.7f * shrink, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(textShader, "EARTH", (PlanetsPositions[3].x + 1) / 2 * WIDTH, (PlanetsPositions[3].y + 1) / 2 * HEIGHT, 0.7f * shrink, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(textShader, "MOON", (PlanetsPositions[4].x + 1) / 2 * WIDTH - 30, (PlanetsPositions[4].y + 1) / 2 * HEIGHT, 0.4f * shrink, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(textShader, "MARS", (PlanetsPositions[5].x + 1) / 2 * WIDTH, (PlanetsPositions[5].y + 1) / 2 * HEIGHT, 0.7f * shrink, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(textShader, "JUPITER", (PlanetsPositions[6].x + 1) / 2 * WIDTH, (PlanetsPositions[6].y + 1) / 2 * HEIGHT, 0.7f * shrink, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(textShader, "SATURN", (PlanetsPositions[7].x + 1) / 2 * WIDTH, (PlanetsPositions[7].y + 1) / 2 * HEIGHT, 0.7f * shrink, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(textShader, "URANUS", (PlanetsPositions[8].x + 1) / 2 * WIDTH, (PlanetsPositions[8].y + 1) / 2 * HEIGHT, 0.7f * shrink, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(textShader, "NEPTUNE", (PlanetsPositions[9].x + 1) / 2 * WIDTH, (PlanetsPositions[9].y + 1) / 2 * HEIGHT, 0.7f * shrink, glm::vec3(0.5, 0.8f, 0.2f));

        RenderText(textShader, "WSAD - forward backward left right", 25, 50, 0.3f, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(textShader, "UP DOWN - increase/decrease speed", 25, 35, 0.3f, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(textShader, "UIJKNM - rotate plane in x, y, z plane", 25, 20, 0.3f, glm::vec3(0.5, 0.8f, 0.2f));

        glBindVertexArray(0);
        
        
        // Swap the buffers
        glfwSwapBuffers(window);
    }
    
    // Properly de-allocate all resources once they've outlived their purpose
    /*glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);*/
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// This function loads a texture from file. Note: texture loading functions like these are usually
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio).
// For learning purposes we'll just define it as a utility function.
GLuint loadTexture(const char* path, GLboolean alpha)
{
    //Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );    // Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}

void RenderText(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state
    shader.Use();
    glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 1.0 },
            { xpos,     ypos,       0.0, 0.0 },
            { xpos + w, ypos,       1.0, 0.0 },

            { xpos,     ypos + h,   0.0, 1.0 },
            { xpos + w, ypos,       1.0, 0.0 },
            { xpos + w, ypos + h,   1.0, 1.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
    camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
    camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
    camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
    camera.ProcessKeyboard(RIGHT, deltaTime);

    if (keys[GLFW_KEY_J])
        SceneRotateX += 0.1;
    if (keys[GLFW_KEY_K])
		SceneRotateX -= 0.1;
	if (keys[GLFW_KEY_U])
		SceneRotateY += 0.1;
	if (keys[GLFW_KEY_I])
		SceneRotateY -= 0.1;
    if (keys[GLFW_KEY_N])
        SceneRotateZ += 0.1;
    if (keys[GLFW_KEY_M])
        SceneRotateZ -= 0.1;
	if (keys[GLFW_KEY_UP])
		PlanetSpeed += 0.05;
	if (keys[GLFW_KEY_DOWN])
		PlanetSpeed -= 0.05;
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