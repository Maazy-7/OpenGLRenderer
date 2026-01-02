#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <random>


#include "util.h"
#include "Shader.h"
#include "Camera.h"
#include "OpenGLObjects/Texture2D.h"
#include "OpenGLObjects/Cubemap.h"
#include "OpenGLObjects/Framebuffer.h"
#include "Mesh.h"
#include "Model.h"
#include "Physics/Collision/GJK.h"
#include "Physics/Collision/CubeCollider.h"
#include "Physics/Components/GameObject.h"


#include "Core/Application.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float dt);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const float ASPECT = SCR_WIDTH / SCR_HEIGHT;

float dt = 0;
float t1 = 0;
float t2 = 0;

std::vector<GameObject*> objects;

//debug stuff
glm::vec3 cubePos = glm::vec3(0.f);
void renderQuad();
void renderQuad(glm::vec2 size);
bool hdr;

float lerpf(float a, float b, float f) { return a + f * (b - a); }

int main()
{


    Application app;
    app.run();
    return 0;



    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGLRenderer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    Camera camera(window, glm::vec3(0.f, 0.f, 3.f), (float)SCR_WIDTH / (float)SCR_HEIGHT);

    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    //this calls the scroll_callback function in the camera class
    glfwSetWindowUserPointer(window,&camera);
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
        camera->scroll_callback(window, xoffset, yoffset);
        });

    glfwSetInputMode(window,GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    stbi_set_flip_vertically_on_load(true);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    LOG(glGetString(GL_RENDERER));
    // build and compile our shader program
    // ------------------------------------
    Shader shader("Shaders/shader.vert","Shaders/shader.frag");
    Shader lightingShader("Shaders/lighting.vert","Shaders/lighting.frag");
    Shader singleColorShader("Shaders/shader.vert", "Shaders/singleColor.frag");
    Shader kernelShader("Shaders/kernelShader.vert", "Shaders/kernelShader.frag");
    Shader depthShader("Shaders/depthShader.vert", "Shaders/depthShader.frag");
    Shader quadShader("Shaders/quad.vert", "Shaders/quad.frag");
    Shader pointLightDepthShader("Shaders/pointShadowDepthShader.vert", "Shaders/pointShadowDepthShader.geom", "Shaders/pointShadowDepthShader.frag");
    Shader bloomShader("Shaders/quad.vert", "Shaders/blur.frag");
    Shader deferredGeometryPass("Shaders/gBufferShader.vert", "Shaders/gBufferShader.frag");
    Shader deferredLightingPass("Shaders/quad.vert","Shaders/gBufferLighting.frag");
    Shader ssaoShader("Shaders/quad.vert", "Shaders/ssao.frag");
    Shader ssaoBlurShader("Shaders/quad.vert", "Shaders/ssaoBlur.frag");


    //uniform buffer object

    //getting the uniform block index (a shader can have multiple uniform buffer objects, and index points to a specific one)
    unsigned int matrixUniformBlockIndex = glGetUniformBlockIndex(lightingShader.m_ID, "Matrices");
    //setting the uniform's binding point, the matrix block will have a binding point of 0
    glUniformBlockBinding(lightingShader.m_ID, matrixUniformBlockIndex, 0);
    
    //setting deferred shader's uniform block
    glUniformBlockBinding(deferredGeometryPass.m_ID, matrixUniformBlockIndex, 0);//same uniform block index as previous shader
    //setting ssao shader's uniform block
    glUniformBlockBinding(ssaoShader.m_ID, matrixUniformBlockIndex, 0);
    
    //creating the uniform buffer
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //linking a certain range of the buffer to the bounding point 0 (in this case its the entire buffer)
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));



    //shadow resolution
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    //framebuffer and texture for shadows
    Cubemap depthMap(GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT);
    Framebuffer depthFBO(depthMap);

    //hdr framebuffer, hdr texture and bloom texture
    //Texture2D colorBuffer1(GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, GL_RGBA);
    //Texture2D colorBuffer2(GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, GL_RGBA);

    //Framebuffer hdrFBO({ colorBuffer1, colorBuffer2 }, {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1}, 
     //   GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, SCR_WIDTH, SCR_HEIGHT);


    //ping pong framebuffers and textures
    //Texture2D pingPongTextures[2] = { Texture2D(GL_RGBA16F, 960, 540, GL_RGBA), Texture2D(GL_RGBA16F, 960, 540, GL_RGBA) };
    //Framebuffer pingPongFBOs[2] = { Framebuffer(pingPongTextures[0], GL_COLOR_ATTACHMENT0), Framebuffer(pingPongTextures[1], GL_COLOR_ATTACHMENT0)};


    //gBuffer for deferred rendering
    
    //position color buffer (R,G,B) -> (x,y,z)
    Texture2D gPosition(GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, GL_RGBA);
    gPosition.setTexFilterParametersi(GL_NEAREST, GL_NEAREST);
    //normal color buffer (R,G,B) -> (x,y,z)
    Texture2D gNormal(GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, GL_RGBA);
    gNormal.setTexFilterParametersi(GL_NEAREST, GL_NEAREST);
    //albedo + specular colour buffer, RGB -> albedo, A -> specular value
    Texture2D gAlbedo(GL_RGBA, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, 0, GL_UNSIGNED_BYTE);

    //gBuffer framebuffer
    Framebuffer gBuffer({ &gPosition, &gNormal, &gAlbedo }, { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 }, 
        GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, SCR_WIDTH, SCR_HEIGHT);

    //ssao texture and framebuffer
    Texture2D ssaoColorBuffer(GL_RED, SCR_WIDTH, SCR_HEIGHT);
    Framebuffer ssaoFramebuffer(ssaoColorBuffer, GL_COLOR_ATTACHMENT0);

    //ssao blur texture and framebuffer
    Texture2D ssaoBlurColorBuffer(GL_RED, SCR_WIDTH, SCR_HEIGHT);
    ssaoBlurColorBuffer.setTexFilterParametersi(GL_NEAREST, GL_NEAREST);
    Framebuffer ssaoBlurFramebuffer(ssaoBlurColorBuffer, GL_COLOR_ATTACHMENT0);

    //generating sample kernel
    std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    ssaoKernel.reserve(64);

    for (int i = 0; i < 64; i++) 
    {
        glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = (float)i / 64.0f;

        scale = lerpf(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
        //Log("kernel vectors");
        //PRINTVEC3(sample);
    }

    //genertating noise texture
    std::vector<glm::vec3> ssaoNoise;
    ssaoNoise.reserve(16);
    for (int i = 0; i < 16; i++) 
    {
        glm::vec3 noise(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, 0.0f);//rotating on xy plane (around z axis) in tangent space
        ssaoNoise.push_back(noise);
        //Log("noise texture");
        //PRINTVEC3(noise);
    }
    Texture2D noiseTex(GL_RGBA32F, 4, 4, GL_RGBA, ssaoNoise.data());
    noiseTex.setTexWrapParametersi(GL_REPEAT, GL_REPEAT);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_MULTISAMPLE);
    
    glm::mat4 model = glm::mat4(1.f);


    glm::vec3 lightPos = glm::vec3(0.f, 1.5f, 0.2f);
    glm::mat4 model2 = glm::translate(glm::mat4(1.f), lightPos) * glm::scale(glm::mat4(1.f), glm::vec3(0.25f));

    

    unsigned int brickNormalMap = loadTexture("Assets/brickwallNormalMap.jpg");
    unsigned int crateDiffuse = loadTexture("Assets/container2.jpg");
    unsigned int crateSpec = loadTexture("Assets/containerFrame.jpg");
    //unsigned int backPackDiffuse = loadTexture("backpack/diffuse.jpg");
    //unsigned int backPackSpecular = loadTexture("backpack/specular.jpg");

    Model ourModel("Assets/brickThingy.obj");
    Model guy("Assets/guy.obj");

    ourModel.addTexture(depthMap.getID(), cubeDepthMap, 0);//shadow map texture
    ourModel.addTexture(brickNormalMap, normalMap, 0);
    ourModel.addTexture(crateDiffuse, diffuse, 0);
    ourModel.addTexture(crateSpec, specular, 0);

    GameObject g1(glm::vec3(-2.f,-0.3f,-4.f), &guy, &camera);
    GameObject g2(glm::vec3(2.f,0.2f,0.f), &ourModel, &camera);

    GameObject wall1(glm::vec3(7.5f, 1.8f, 0.f), &ourModel, &camera);
    wall1.setScale(glm::vec3(0.2f, 3.5f, 7.5f));
    wall1.isStatic = true;

    GameObject wall2(glm::vec3(-7.5f, 1.8f, 0.f), &ourModel, &camera);
    wall2.setScale(glm::vec3(0.2f, 3.5f, 7.5f));
    wall2.isStatic = true;

    GameObject wall3(glm::vec3(0.0f, 1.8f, 7.5f), &ourModel, &camera);
    wall3.setScale(glm::vec3(7.5f, 3.5f, 0.2f));
    wall3.isStatic = true;

    GameObject wall4(glm::vec3(7.5f, 1.8f, 0.f), &ourModel, &camera);
    wall4.setScale(glm::vec3(0.2f, 3.5f, 7.5f));
    wall4.isStatic = true;

    g1.rotate(glm::vec3(0.f, 0.f, -3.14f/2.f));
    g1.scale(glm::vec3(0.4f));


    GameObject floor(glm::vec3(0.f,-1.f,0.f),&ourModel, &camera);
    floor.setScale(glm::vec3(7.5f,0.2f,7.5f));
    floor.isStatic = true;

    objects.push_back(&g1);
    objects.push_back(&g2);
    objects.push_back(&floor);
    //objects.push_back(&wall1);
    //objects.push_back(&wall2);
    //objects.push_back(&wall3);
    //objects.push_back(&wall4);

    CollisionManifold collisionInfo;
    //cubePos = g1.getPosition();

    //g1.setAngularVelocity(glm::vec3(0.f,1.f,0.f));
    

    //hdr shader texture set
    quadShader.use();
    quadShader.setInt("hdrBuffer", 0);
    quadShader.setInt("blurBuffer", 1);

    deferredLightingPass.use();
    deferredLightingPass.setInt("gPosition", 0);
    deferredLightingPass.setInt("gNormal", 1);
    deferredLightingPass.setInt("gAlbedoSpec", 2);
    deferredLightingPass.setInt("cubeDepthMap", 3);
    deferredLightingPass.setInt("ssao", 4);

    ssaoShader.use();
    ssaoShader.setVec2("screenDimensions", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
    ssaoShader.setVec3("samples", ssaoKernel.size(), ssaoKernel);
    ssaoShader.setInt("gPosition", 0);
    ssaoShader.setInt("gNormal", 1);
    ssaoShader.setInt("texNoise", 2);

    ssaoBlurShader.use();
    ssaoBlurShader.setInt("ssaoInput", 0);

    std::array<glm::vec3, 4> lightPositions =
    {
        glm::vec3(1.f, 2.f, 1.f),
        glm::vec3(1.f, 2.f,-1.f),
        glm::vec3(-1.f, 2.f,-1.f),
        glm::vec3(-1.f, 2.f, 1.f),
    };

    std::array<glm::vec3, 4> lightColors =
    {
        glm::vec3(1.f, 1.f, 1.f)*4.f,
        glm::vec3(1.f, 0.f, 0.f) * 4.f,
        glm::vec3(0.f, 1.f, 0.f) * 4.f,
        glm::vec3(0.f, 0.f, 1.f) * 4.f,
    };


    // render loop
    // -----------
    
    //DEBUG
    float curTime = 0.f;
    
    while (!glfwWindowShouldClose(window))
    {
        t2 = (float)glfwGetTime();
        dt = t2 - t1;
        t1 = t2;

        // input
        // -----
        processInput(window, dt);
        
        //DEBUG FPS DISPLAY
        curTime += dt;
        float n = 1.f;
        if (curTime >= n) //every n seconds
        {
            LOG(1.0/dt);
            curTime = 0.f;
        }

        // physics
        // -------
        /*g1.movePosition(cubePos);
        cubePos = glm::vec3(0.f);

        //

        //updating object positions
        for (GameObject* obj : objects) 
        {
            obj->update(dt);
        }

        //checking and solving collisions, O(N^2)      for now...
        for (GameObject* obj1 : objects) 
        {
            for (GameObject* obj2 : objects) 
            {
                if (obj1 == obj2) { continue; }
                auto [simplex, collision] = GJK(&obj1->cCollider(), &obj2->cCollider());

                if (collision)
                {
                    collisionInfo = EPA(simplex, &obj1->cCollider(), &obj2->cCollider());
                    glm::vec3 move = collisionInfo.m_normal * collisionInfo.m_depth / 2.f;
                    if (!obj1->isStatic) { obj1->movePosition(-move); }
                    if (!obj2->isStatic) {obj2->movePosition(move);}
                }
            }
        }*/


        //lightPositions[0].x = sin(glfwGetTime()) * 3.0f;
        //lightPositions[0].z = cos(glfwGetTime()) * 3.0f;
        
        //lightingShader.setVec3("lightPos", lightPos);
        
        // render
        // ------
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.update(dt);

        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), camera.getView());
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), camera.getProjection());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        float nearPlane = 0.1f, farPlane = 25.0f;

        //shadow map
        //render scene from light's point of view into the depth texture - creates the depth map
        /*glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        lightProjection = glm::ortho(-20.f, 20.f, -20.f, 20.f, nearPlane, farPlane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
        lightSpaceMatrix = lightProjection * lightView;
        */

        //point shadow
        glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), (float)(SHADOW_WIDTH / SHADOW_HEIGHT), nearPlane, farPlane);//aka light projection but for point shadows, since point light projection should be a perspective one
        std::array<glm::mat4, 6> shadowTransforms = { glm::mat4(1.f) };
        //shadow's matrix for each face of the cubemap, essentially the same as the lightSpaceMatrix
        shadowTransforms[0] = shadowProjection * glm::lookAt(lightPositions[0], lightPositions[0] + glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3( 0.0f,-1.0f, 0.0f));
        shadowTransforms[1] = shadowProjection * glm::lookAt(lightPositions[0], lightPositions[0] + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3( 0.0f,-1.0f, 0.0f));
        shadowTransforms[2] = shadowProjection * glm::lookAt(lightPositions[0], lightPositions[0] + glm::vec3( 0.0f, 1.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f));
        shadowTransforms[3] = shadowProjection * glm::lookAt(lightPositions[0], lightPositions[0] + glm::vec3( 0.0f,-1.0f, 0.0f), glm::vec3( 0.0f, 0.0f,-1.0f));
        shadowTransforms[4] = shadowProjection * glm::lookAt(lightPositions[0], lightPositions[0] + glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec3( 0.0f,-1.0f, 0.0f));
        shadowTransforms[5] = shadowProjection * glm::lookAt(lightPositions[0], lightPositions[0] + glm::vec3( 0.0f, 0.0f,-1.0f), glm::vec3( 0.0f,-1.0f, 0.0f));


        //shadow map
        /*
        //render scene from the light's point of view
        depthShader.use();
        depthShader.setMatrix4f("lightSpaceMatrix", lightSpaceMatrix);
        glViewport(0,0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFbo);
        glClear(GL_DEPTH_BUFFER_BIT);
        g1.renderDepth(depthShader);
        g2.renderDepth(depthShader);
        floor.renderDepth(depthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

        //point shadow
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        pointLightDepthShader.use();
        //setting the shadow matrices array uniform for the geometry shader
        for (size_t i = 0; i < 6; i++) { pointLightDepthShader.setMatrix4f("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]); }
        pointLightDepthShader.setFloat("farPlane", farPlane);
        pointLightDepthShader.setVec3("lightPos", lightPositions[0]);
        depthFBO.bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        for (GameObject* obj : objects) 
        {
            obj->renderDepth(pointLightDepthShader);
        }
        depthFBO.unbind();

        //reset viewport
        glViewport(0,0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //forward renderer
        /*
           //drawing normal lit scene into hdr framebuffer
           hdrFBO.bind();
           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
           //drawing the light
           singleColorShader.use();
           singleColorShader.setMatrix4f("model", model2);
           singleColorShader.setMatrix4f("view", *camera.getView());
           singleColorShader.setMatrix4f("projection", *camera.getProjection());
           singleColorShader.setVec3("color", glm::vec3(1.f,1.f,1.f));
           ourModel.draw(singleColorShader);
           
           //drawing rest of the scene
           
           //setting common uniforms
           lightingShader.use();
           //lightingShader.setInt("cubDepthMap", 1);
           lightingShader.setVec3("viewPos", camera.getPos());
           //lightingShader.setMatrix4f("lightSpaceMatrix", lightSpaceMatrix); for shadow maps
           lightingShader.setVec3("lightPos", lightPos);
           lightingShader.setFloat("farPlane", farPlane);
           
           
           for (GameObject* obj : objects)
           {
               obj->render(lightingShader);
           }
           
           hdrFBO.unbind();
           
           if (hdr)
           {
               bool horizontal = true;
               unsigned int amount = 10;
               bloomShader.use();
               pingPongFBOs[horizontal].bind();
               bloomShader.setInt("horizontal", horizontal);
               colorBuffer2.bind();
               renderQuad();
               horizontal = !horizontal;
           
               for (unsigned int i = 0; i < amount - 1; i++)
               {
                   pingPongFBOs[horizontal].bind();
                   bloomShader.setInt("horizontal", horizontal);
                   pingPongTextures[!horizontal].bind();
                   renderQuad();
                   horizontal = !horizontal;
               }
               glBindFramebuffer(GL_FRAMEBUFFER, 0);
           }
           
           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
           //render scene as texture using hdr shader
           quadShader.use();
           glActiveTexture(GL_TEXTURE0);
           colorBuffer1.bind();
           glActiveTexture(GL_TEXTURE1);
           //pingPongTextures[horizontal].bind();
           
           quadShader.setFloat("exposure", 1.0);
           quadShader.setInt("hdr", hdr);
           //renderQuad();

        */


        //deferred renderer

        //deferred geometry pass, rendering geometry information, positions, normals, diffuse, specular
        gBuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (GameObject* obj : objects) 
        {
            obj->render(deferredGeometryPass);
        }
        gBuffer.unbind();

        //ssao pass
        ssaoFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT);
        ssaoShader.use();
        glActiveTexture(GL_TEXTURE0);
        gPosition.bind();
        glActiveTexture(GL_TEXTURE1);
        gNormal.bind();
        glActiveTexture(GL_TEXTURE2);
        noiseTex.bind();
        renderQuad();
        ssaoFramebuffer.unbind();

        //ssao blur pass
        ssaoBlurFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT);
        ssaoBlurShader.use();
        glActiveTexture(GL_TEXTURE0);
        ssaoColorBuffer.bind();
        renderQuad();
        ssaoBlurFramebuffer.unbind();

        //deferred lighting pass
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        deferredLightingPass.use();
        glActiveTexture(GL_TEXTURE0);
        gPosition.bind();
        glActiveTexture(GL_TEXTURE1);
        gNormal.bind();
        glActiveTexture(GL_TEXTURE2);
        gAlbedo.bind();
        glActiveTexture(GL_TEXTURE3);
        depthMap.bind();
        glActiveTexture(GL_TEXTURE4);
        ssaoColorBuffer.bind();

        deferredLightingPass.setMatrix4f("viewMat", *camera.getView());
        deferredLightingPass.setInt("ssaoOn", hdr);
        //light uniforms
        for (int i = 0; i < 1; i++) 
        {
            deferredLightingPass.setVec3("lights[" + std::to_string(i) + "].position", lightPositions[i]);
            deferredLightingPass.setVec3("lights[" + std::to_string(i) + "].color", lightColors[i]);
            deferredLightingPass.setFloat("lights[" + std::to_string(i) + "].linear", 0.09f);
            deferredLightingPass.setFloat("lights[" + std::to_string(i) + "].quadratic", 0.32f);
        }
        deferredLightingPass.setVec3("viewPos", camera.getPos());

        renderQuad();
        
        //writing the depth information from the gbuffer's renderbuffer to the default framebuffer so that the lights can be drawn based off the scene's gbuffer depth
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.getID());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(0,0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        //drawing the lights
        singleColorShader.use();
        singleColorShader.setMatrix4f("view", *camera.getView());
        singleColorShader.setMatrix4f("projection", *camera.getProjection());
        for (int i = 0; i < 1; i++) 
        {
            singleColorShader.setMatrix4f("model", glm::translate(glm::mat4(1.f), lightPositions[i]) * glm::scale(glm::mat4(1.f), glm::vec3(0.2f)));
            singleColorShader.setVec3("color", lightColors[i]);
            ourModel.draw(singleColorShader);
        }

        glDisable(GL_DEPTH_TEST);
        quadShader.use();
        glActiveTexture(GL_TEXTURE0);
        ssaoColorBuffer.bind();
        renderQuad(glm::vec2());
        glEnable(GL_DEPTH_TEST);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1,&EBO);
    

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0; 
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, float dt)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //moving the cube, debug stuff for now
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { cubePos += glm::vec3(0.f, 0.f, 1.f)*dt*3.f; }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { cubePos += glm::vec3(0.f, 0.f, -1.f)*dt*3.f; }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { cubePos += glm::vec3(1.f, 0.f, 0.f)*dt*3.f; }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { cubePos += glm::vec3(-1.f, 0.f, 0.f)*dt*3.f; }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { hdr = true; }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) { hdr = false; }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

unsigned int loadTexture(const char* path)
{
    unsigned int texture = 0;
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    return texture;
}

//debug function stuff
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

unsigned int quadVAO2 = 0;
unsigned int quadVBO2;
void renderQuad(glm::vec2 size)
{
    if (quadVAO2 == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  -0.5f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
             -0.5f,  -0.5f, 0.0f, 1.0f, 1.0f,
             -0.5f, -1.0f, 0.0f,  1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO2);
        glGenBuffers(1, &quadVBO2);
        glBindVertexArray(quadVAO2);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO2);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

//framebuffer lesson stuff
/* //screen quad stuff, screen quad which will be a singular texture that spans the whole screen
    float quadVertices[] = 
    { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    //quad VAO,VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2* sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //creating framebuffer
    unsigned int frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    //create a color attatchment for the framebuffer, this attatchment will be a texture
    unsigned int textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
    
    //create renderbuffer
    unsigned int renderBuffer;
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    //check for any erros (incomplete framebuffer)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Log("FRAME BUFFER IS NOT COMPLETE");
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    
    
    ----------
    In render loop
    //glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);//current scene's information will be written into the framebuffer as a texture
    //glEnable(GL_DEPTH_TEST);
    
    ***Draw normal scene
    

    //then drawing texture whose data is from the framebuffer
    /binding back to default framebuffer
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glDisable(GL_DEPTH_TEST);//disabling depth test so the quad isn't discarded from the depth test

    //clearing relevant buffers
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);

    //kernelShader.use();
    //glBindVertexArray(VAO);
    //glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    //glBindVertexArray(0);
    */

//framebuffer shadow stuff (without classes)
/*depth framebuffer
  unsigned int depthFbo;
  glGenFramebuffers(1, &depthFbo);
  
  //shadow map 2D texture
  /*
  //depth map texture, 
  unsigned int depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 1.0f,1.0f,1.0f,1.0f };//white border color so depth map returns max depth when oustide range of map...
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  //attach depth map texture to framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, depthFbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
    
  
      //point shadow cubemap texture
  unsigned int depthCubeMap;
  glGenTextures(1, &depthCubeMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
  for (unsigned int i = 0; i < 6; i++)
  {
      //making each face of the cubemap into a depth map
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glBindFramebuffer(GL_FRAMEBUFFER, depthFbo);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  
  
  glBindBuffer(GL_FRAMEBUFFER, 0); */