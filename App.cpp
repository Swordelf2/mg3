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
        RenderToDepthMap();
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

    if (m_input[INPUT_1] && m_curScene != 1) {
        m_curScene = 1;
    }
    if (m_input[INPUT_2] && m_curScene != 2) {
        m_curScene = 2;
    }

    /* Update deltatime */
    m_prevTime = m_time;
    m_time = glfwGetTime();
    m_deltaTime = m_time - m_prevTime;
    
    for (Entity *entity : m_entities) {
        entity->Update();
    }

    m_cube->m_angle += 1.8 * GetDeltaTime() * glm::radians(25.0);

    m_square->m_position.y = 3.0 + std::sin(m_time * 3.0);

    m_triangle->m_position.x = 0.5 + std::sin(m_time * 5.0);
    m_triangle->m_basicColor = glm::vec3(
            static_cast<float>(0.5 + 0.5 * std::sin(m_time * 2.0)),
            0.0,
            static_cast<float>(0.5 + 0.5 * std::cos(m_time * 2.0)));
}

void App::RenderToDepthMap()
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    float near_plane = 1.0f, far_plane = 15.0f;
    glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

    /*
    glm::mat4 lightProj = glm::perspective(static_cast<float>(glm::radians(45.0)),
            static_cast<float>(SHADOW_WIDTH) / SHADOW_HEIGHT,
            0.1f, 100.0f);
            */

    glm::mat4 lightView = glm::lookAt(m_lightPos,
            glm::vec3(0.0, 0.0, 0.0),
            glm::vec3(0.0, 1.0, 0.0));


    m_lightPV = lightProj * lightView;

    for (Entity* entity : m_entities) {
        Shader *shaderStore = entity->m_shader;
        entity->m_shader = &m_shaders[SHADER_LIGHT];
        entity->Draw(m_lightPV);
        entity->m_shader = shaderStore;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void App::Render()
{
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_curScene == 1) {
        // projection
        glm::mat4 pv = glm::perspective(static_cast<float>(glm::radians(45.0)),
                static_cast<float>(m_screenWidth) / m_screenHeight,
                0.1f, 100.0f);

        // view
        pv = glm::translate(pv, -m_viewPos);
        pv = glm::rotate(pv, -m_viewAngle,
                glm::vec3(1.0, 0.0, 0.0)); 

        m_shaders[SHADER_LIGHTING].SetUniform("viewPos", m_viewPos);
        m_shaders[SHADER_LIGHTING].SetUniform("lightSpaceTransform", m_lightPV);
        m_shaders[SHADER_LIGHTING].SetUniform("shadowMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_depthMap);


        for (Entity *entity : m_entities) {
            entity->Draw(pv);
        }
    } else if (m_curScene == 2) {
        m_shaders[SHADER_QUAD].Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_depthMap);
        m_meshes[MESH_SQUARE].Draw();
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

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glGenTextures(1, &m_depthMap);
    glBindTexture(GL_TEXTURE_2D, m_depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
}

void App::InitScene1()
{
    // TODO change here
    m_curScene = 2;

    m_viewPos = glm::vec3(0.0, 3.0, 9.0);
    m_viewAngle = glm::radians(-60.0);

    m_lightPos = {-3.5, 8.0, 3.5};
    m_lightColor = {1.0, 1.0, 1.0};

    m_shaders[SHADER_LIGHTING].SetUniform("lightColor", m_lightColor);
    m_shaders[SHADER_LIGHTING].SetUniform("lightPos", m_lightPos);

    /*
    Entity *lightEntity = new Entity(
            &m_meshes[MESH_CUBE],
            &m_shaders[SHADER_BASIC],
            nullptr);
    lightEntity->m_position = m_lightPos;
    lightEntity->m_scale *= 0.2;
    m_shaders[SHADER_BASIC].SetUniform("basicColor", glm::vec3(1.0, 1.0, 1.0));
    m_entities.push_back(lightEntity);
    */

    // main cube
    m_cube = new Entity(
            &m_meshes[MESH_CUBE],
            &m_shaders[SHADER_LIGHTING],
            &m_textures[TEXTURE_GOLD]);
    m_cube->m_rotAxis = glm::normalize(glm::vec3(2.0, 3.0, 1.0));
    m_cube->m_angle = glm::radians(-120.0);
    m_cube->m_position = glm::vec3(-1.0, 4.6, -0.5);
    m_cube->m_scale *= 1.0;
    m_entities.push_back(m_cube);

    // square
    m_square = new Entity(
            &m_meshes[MESH_SQUARE],
            &m_shaders[SHADER_LIGHTING],
            nullptr);
    m_square->m_position = glm::vec3(1.5, 2.0, -3.5);
    m_square->m_scale *= 0.75;
    m_square->m_basicColor = {0.5, 0.74, 0.22};
    m_square->m_rotAxis = {1.0, 0.0, 0.0};
    m_square->m_angle = glm::radians(-30.0);
    m_entities.push_back(m_square);


    // triangle
    m_triangle = new Entity(
            &m_meshes[MESH_TRIANGLE],
            &m_shaders[SHADER_LIGHTING],
            nullptr);

    m_triangle->m_rotAxis = {1.0, 0.0, 0.0};
    m_triangle->m_angle = glm::radians(-30.0);
    m_triangle->m_position = glm::vec3(0.0, 2.8, -2.0);
    m_entities.push_back(m_triangle);

    // plane
    m_plane = new Entity(
            &m_meshes[MESH_SQUARE],
            &m_shaders[SHADER_LIGHTING],
            nullptr);

    m_plane->m_rotAxis = {1.0, 0.0, 0.0};
    m_plane->m_angle = glm::radians(-90.0);
    m_plane->m_position.y = -1.0;
    m_plane->m_scale *= 20.0;
    m_plane->m_basicColor = {0.7, 0.7, 0.7};
    m_entities.push_back(m_plane);

    // debugQuad
}

void App::InitMeshes()
{
    std::vector<GLushort> emptyInds = {};
    std::vector<VertexN> vertices;
    std::vector<GLushort> inds;
    // MESH_TRIANGLE
    vertices = {
        {{-1.0, -sqrt(3.0)/3.0, 0.0}, {0.0, 0.0}, {0.0, 0.0, 1.0}},
        {{ 1.0, -sqrt(3.0)/3.0, 0.0}, {1.0, 0.0}, {0.0, 0.0, 1.0}},
        {{ 0.0, 2.0 * sqrt(3.0)/3.0, 0.0}, {0.5, sqrt(3.0)/4.0}, {0.0, 0.0, 1.0}}
    };
    m_meshes.emplace_back(vertices, emptyInds);

    // MESH_SQUARE
    vertices = {
        {{ 1.0,  1.0, 0.0}, {1.0, 1.0}, {0.0, 0.0, 1.0}},
        {{-1.0,  1.0, 0.0}, {0.0, 1.0}, {0.0, 0.0, 1.0}},
        {{-1.0, -1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0, 1.0}},
        {{ 1.0, -1.0, 0.0}, {1.0, 0.0}, {0.0, 0.0, 1.0}}
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
    m_shaders[SHADER_LIGHTING].SetUniform("texture0", 0);
    m_shaders.emplace_back("graphics/shaders/light.vert", "graphics/shaders/lighting.frag");
    m_shaders.emplace_back("graphics/shaders/quad.vert", "graphics/shaders/quad.frag");
    m_shaders[SHADER_QUAD].SetUniform("texture0", 0);
}

void App::InitTextures()
{
    m_textures.emplace_back("res/gold.jpg");
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
