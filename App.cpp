#include <GL/glew.h>

#include "App.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "graphics/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>


App * App::app;

App::~App()
{
    ClearEntities();
}

int App::Execute()
{
    if (Init() == -1) {
        return -1;
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    while (m_running) {
        Update();
        Render();

        if (glfwWindowShouldClose(m_window) == GL_TRUE) {
            m_running = false;
        }
    }

    glfwDestroyWindow(m_window);
    glfwTerminate();

    return 0;
}

double App::GetDeltaTime() const
{
    return m_deltaTime;
}

double App::GetTime() const
{
    return m_time;
}

void App::Update()
{
    glfwPollEvents();

    /* Update deltatime */
    m_prevTime = m_time;
    m_time = glfwGetTime();
    m_deltaTime = m_time - m_prevTime;
    
    for (Entity *entity : m_entities) {
        entity->Update();
    }

    m_entities[0]->m_angle += GetDeltaTime() * glm::radians(25.0);
    m_entities[0]->m_position += static_cast<float>(GetDeltaTime()) * glm::vec3(0.3, 0.0, 0.0);
}

void App::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Background
    if (m_background) {
        m_background->Draw();
    }

    // projection
    glm::mat4 pv = glm::perspective(static_cast<float>(glm::radians(45.0)),
            static_cast<float>(m_screenWidth) / m_screenHeight,
            0.1f, 100.0f);

    // view
    pv = glm::translate(pv, -m_viewPos);
    pv = glm::rotate(pv, -m_viewAngle,
            glm::vec3(1.0, 0.0, 0.0)); 

    m_shaders[SHADER_LIGHTING].SetUniform("viewPos", m_viewPos);

    for (Entity *entity : m_entities) {
        entity->Draw(pv);
    }

    glfwSwapBuffers(m_window);
}

int App::Init()
{
    srand(time(nullptr));

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    m_screenWidth = 1200;
    m_screenHeight = 720;
    m_window = glfwCreateWindow(m_screenWidth, m_screenHeight, "Hello World", NULL, NULL);
    if (!m_window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(m_window);

    /* Init GLEW */
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error!" << std::endl;
    }

    const char *ver = (const char *) glGetString(GL_VERSION);
    std::cout << ver << std::endl;

    // Set the debug callback function
    // TODO: DISABLE THIS FOR RELEASE
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(App::debugCallback, nullptr);

    glfwSetKeyCallback(m_window, App::KeyCallback);

    m_time = m_prevTime = glfwGetTime();
    m_deltaTime = 0.0;

    app = this;

    InitMeshes();
    InitShaders();
    InitTextures();

    InitScene1();

    return 0;
}

void App::ClearEntities()
{
    for (Entity *entity : m_entities) {
        delete entity;
    }
    m_entities.clear();
    if (m_background) {
        delete m_background;
        m_background = nullptr;
    }
}
void App::InitScene1()
{
    m_viewPos = glm::vec3(0.0, 0.0, 5.0);
    m_viewAngle = glm::radians(-25.0);

    m_lightPos = {2.0, 1.0, 3.0};
    m_lightColor = {1.0, 1.0, 1.0};

    m_shaders[SHADER_LIGHTING].SetUniform("lightColor", m_lightColor);
    m_shaders[SHADER_LIGHTING].SetUniform("lightPos", m_lightPos);

    Entity *entity = new Entity(
            &m_meshes[MESH_CUBE],
            &m_shaders[SHADER_LIGHTING],
            nullptr);

    entity->m_shader->SetUniform("basicColor", glm::vec3(1.0f, 0.5f, 0.31f));
    m_entities.push_back(entity);

    entity->m_rotAxis = glm::normalize(glm::vec3(2.0, 3.0, 1.0));
    entity->m_angle = glm::radians(-120.0);
    entity->m_position = glm::vec3(-3.0, -0.5, 0.0);
}

void App::InitMeshes()
{
    std::vector<GLushort> emptyInds = {};
    std::vector<Vertex> vertices;
    std::vector<GLushort> inds;
    // MESH_TRIANGLE
    vertices = {
        {{-1.0, -sqrt(3.0)/3.0, 0.0}, {0.0, 0.0}},
        {{ 1.0, -sqrt(3.0)/3.0, 0.0}, {1.0, 0.0}},
        {{ 0.0, 2.0 * sqrt(3.0)/3.0, 0.0}, {0.5, sqrt(3.0)/4.0}}
    };
    m_meshes.emplace_back(vertices, emptyInds);

    // MESH_SQUARE
    vertices = {
        {{ 1.0,  1.0, 0.0}, {1.0, 1.0}},
        {{-1.0,  1.0, 0.0}, {0.0, 1.0}},
        {{-1.0, -1.0, 0.0}, {0.0, 0.0}},
        {{ 1.0, -1.0, 0.0}, {1.0, 0.0}}
    };
    inds = {
        0, 1, 2,
        2, 3, 0
    };
    m_meshes.emplace_back(vertices, inds);

    // MESH_CUBE
    auto p = ReadMesh("res/cube.mesh");
    m_meshes.emplace_back(p.first, p.second);
}

void App::InitShaders()
{
    m_shaders.emplace_back("graphics/shaders/basic.vert", "graphics/shaders/basic.frag");
    m_shaders.emplace_back("graphics/shaders/lighting.vert", "graphics/shaders/lighting.frag");
    m_shaders.emplace_back("graphics/shaders/background.vert", "graphics/shaders/textured.frag");
}

void App::InitTextures()
{
    m_textures.emplace_back("res/rainbow.jpg");
}

double App::GetRand(double l, double r)
{
    double d = rand() / (RAND_MAX + 1.0);
    return l + (r - l) * d;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void App::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    int input_ind = -1;
    switch (key) {
        case GLFW_KEY_1:
            input_ind = INPUT_1;
            break;
        case GLFW_KEY_2:
            input_ind = INPUT_2;
            break;
        case GLFW_KEY_3:
            input_ind = INPUT_3;
            break;
        case GLFW_KEY_W:
            input_ind = INPUT_W;
            break;
        case GLFW_KEY_S:
            input_ind = INPUT_S;
            break;
    }
    if (input_ind != -1) {
        App::app->m_input[input_ind] = (action != GLFW_RELEASE);
    }
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic ignored "-Wunused-parameter"
void App::debugCallback(GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar *message,
        const void *userParam)
{
    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        std::cerr << "OpenGL debug. Message: " << std::endl <<
            std::string(message, length) << std::endl;
    }
}
#pragma GCC diagnostic pop
