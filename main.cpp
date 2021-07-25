#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 lightRotation;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;
glm::vec3 lightColor1;
glm::vec3 lightPos;
glm::vec3 lightPos1;
glm::vec3 lightPos2;
glm::vec3 lightPos3;
glm::vec3 lightPos4;



// shader uniform locations
GLuint modelLoc;
GLuint viewLoc;
GLuint projectionLoc;
GLuint normalMatrixLoc;
GLuint lightDirLoc;
GLuint lightColorLoc;
GLuint lightColor1Loc;
GLuint lightPosLoc;
GLuint lightPosLoc1;
GLuint lightPosLoc2;
GLuint lightPosLoc3;
GLuint lightPosLoc4;


// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));


GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

// models
gps::Model3D screenQuad;
gps::Model3D ground;
gps::Model3D brad;
gps::Model3D christmas_tree;
gps::Model3D molid;
gps::Model3D cadou1;
gps::Model3D cadou2;
gps::Model3D cadou3;
gps::Model3D cadou4;
gps::Model3D cadou5;
gps::Model3D snowman1;
gps::Model3D home;
gps::Model3D cat;
gps::Model3D bustean;
gps::Model3D house;
gps::Model3D trunck;
gps::Model3D santa;
gps::Model3D felinar;
gps::Model3D sanie;
gps::Model3D fulg;

GLfloat angle;
GLfloat snowmanAngle;

// shaders
gps::Shader myBasicShader;
gps::Shader depthMapShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader skyboxShader;

gps::SkyBox mySkyBox;
std::vector<const GLchar*> faces;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

int fog;
int ninsoare;
float snowmanX = 0.0f;
float snowmanZ = 0.0f;
float santaSpeed = 0.2f;
int mode = 1;

float delta = 0.0f;
float movementSpeed = 10.0f;
void updateDelta(double elapsedSeconds)
{
    delta = delta + movementSpeed * elapsedSeconds;
}
double lastTimeStamp = glfwGetTime();

float xx[100];
float zz[100];
float yy[100];
float d = 0.0f;
int n = 100;
glm::vec3 snowflakeLength = glm::vec3(20.0, 20.0, 20.0);

void generateSnowflakes() {
    for (int i = 0; i < n; i++) {
        xx[i] = -snowflakeLength.x + ((float)(std::rand() % ((int)snowflakeLength.x * 2 * 100))) / 100.0f;
        yy[i] = ((float)(std::rand() % ((int)snowflakeLength.y * 100))) / 100.0f;
        zz[i] = -snowflakeLength.z + ((float)(std::rand() % (((int)snowflakeLength.z * 2) * 100))) / 100.0f;
    }
}


GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    //TODO

    myWindow.setWindowDimensions(WindowDimensions{ width,height });
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        showDepthMap = !showDepthMap;

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

bool firstMouse = true;
float lastX = 400, lastY = 300;
float yaw, pitch;
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
   
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);
}

void processMovement() {
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for object
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for object
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for object
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for object
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angle += 0.3f;
        if (angle > 360.0f)
            angle -= 360.0f;
        glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
        myBasicShader.useShaderProgram();
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle -= 0.3f;
        if (angle < 0.0f)
            angle += 360.0f;
        glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
        myBasicShader.useShaderProgram();
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
    }
    if (pressedKeys[GLFW_KEY_O]) {
        fog = 0;
    }

    if (pressedKeys[GLFW_KEY_I]) {
        fog = 1;
    }

    if (pressedKeys[GLFW_KEY_B]) {
        ninsoare = 0;
    }

    if (pressedKeys[GLFW_KEY_N]) {
        ninsoare = 1;
    }

    if (pressedKeys[GLFW_KEY_1]) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

    if (pressedKeys[GLFW_KEY_2]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (pressedKeys[GLFW_KEY_3]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (pressedKeys[GLFW_KEY_LEFT]) {
        snowmanAngle += 1.5f;
        if (snowmanAngle > 360.0f)
            snowmanAngle -= 360.0f;
    }


    if (pressedKeys[GLFW_KEY_RIGHT]) {
        snowmanAngle -= 1.5f;

        if (snowmanAngle < 0.0f)
            snowmanAngle += 360.0f;
    }

    if (pressedKeys[GLFW_KEY_UP]) {
        snowmanX += santaSpeed * sin(glm::radians(snowmanAngle));
        snowmanZ += santaSpeed * cos(glm::radians(snowmanAngle));
    }

}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    brad.LoadModel("models/brad/lowpoyltree.obj");
    christmas_tree.LoadModel("models/ch/12150_Christmas_Tree_V2_L2.obj");
    molid.LoadModel("models/molid/10459_White_Ash_Tree_v1_L3.obj");
    cadou1.LoadModel("models/cadou/g1.obj");
    cadou2.LoadModel("models/cadou/g2.obj");
    cadou3.LoadModel("models/cadou/g3.obj");
    cadou4.LoadModel("models/cadou/g4.obj");
    cadou5.LoadModel("models/cadou/g5.obj");
    sanie.LoadModel("models/sleigh/1157_Sleigh_v3_l2.obj");
    snowman1.LoadModel("models/snowman/SnowmanOBJ.obj");
    house.LoadModel("models/house/10783_TreeHouse_v7_LOD3.obj");
    bustean.LoadModel("models/bustean/Wood.obj");
    home.LoadModel("models/home/1home.obj");
    santa.LoadModel("models/santa/Santa.obj");
    cat.LoadModel("models/cat/12221_Cat_v1_l3.obj");
    felinar.LoadModel("models/streetlamp/streetlamp.obj");
    fulg.LoadModel("models/Snowflake/Snowflake.obj");
    trunck.LoadModel("models/trunchi/log.obj");
    ground.LoadModel("models/ground/ground.obj");
}

void initShaders() {
    myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
    myBasicShader.useShaderProgram();
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
    depthMapShader.useShaderProgram();
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
    faces.push_back("skybox/left.jpg");
    faces.push_back("skybox/right.jpg");
    faces.push_back("skybox/top.jpg");
    faces.push_back("skybox/bottom.jpg");
    faces.push_back("skybox/back.jpg");
    faces.push_back("skybox/front.jpg");
    mySkyBox.Load(faces);
}

void initUniforms() {
    myBasicShader.useShaderProgram();

    // create model matrix for object
    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for object
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(30.0f, 20.0f, 0.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    //second light source 
    lightColor1 = glm::vec3(1.0f, 0.0f, 0.0f); //red color
    lightColor1Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor1");
    glUniform3fv(lightColor1Loc, 1, glm::value_ptr(lightColor1));

    //position of the second light - under the main tree 
    lightPos = glm::vec3(-3.0f, 0.0f, -13.0f);
    lightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

    //position for streetlight  (on fog only)
    lightPos1 = glm::vec3(9.0f, -1.0f, 7.0f);
    lightPosLoc1 = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos1");
    glUniform3fv(lightPosLoc1, 1, glm::value_ptr(lightPos1));

    lightPos2 = glm::vec3(-20.0f, -1.0f, 20.0f);
    lightPosLoc2 = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos2");
    glUniform3fv(lightPosLoc2, 1, glm::value_ptr(lightPos2));

    lightPos3 = glm::vec3(19.0f, -1.0f, -24.0f);
    lightPosLoc3 = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos3");
    glUniform3fv(lightPosLoc3, 1, glm::value_ptr(lightPos3));

    lightPos4 = glm::vec3(19.0f, -1.0f, -1.0f);
    lightPosLoc4 = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos4");
    glUniform3fv(lightPosLoc4, 1, glm::value_ptr(lightPos4));

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void renderObjects(gps::Shader shader, bool depthPass) {


    // select active shader program
    shader.useShaderProgram();

    if (ninsoare == 1)
    {
        d = 0.07;

        for (int i = 0; i < n; i++) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(xx[i], yy[i], zz[i]));
            yy[i] -= d;
            if (yy[i] <= -1.0f) {
                yy[i] = snowflakeLength.y;
                xx[i] = -snowflakeLength.x + ((float)(std::rand() % (((int)snowflakeLength.x * 2) * 10000))) / 100.0f;
                zz[i] = -snowflakeLength.z + ((float)(std::rand() % (((int)snowflakeLength.z * 2) * 10000))) / 100.0f;
            }
            model = glm::scale(model, glm::vec3(0.0007));
            glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            fulg.Draw(shader);
        }
    }

    //draw the lame christmass tree
    int i1 = 4;
    for (float j = -25.0; j <= 27.0; j = j + 2) {

        if (i1 <= 20)
        {
            for (float i = -28.0; i <= 28.0; i = i + i1)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(i, -1.0f, j));
                model = glm::scale(model, glm::vec3(0.4f));
                glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                if (!depthPass) {
                    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
                    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
                }
                brad.Draw(shader);
                i1 = i1 + 1;
            }
        }
        else
        {
            i1 = 3;
        }
    }

    //draw pathces of molid 

    for (float j = 0.0; j <= 4.0; j= j + 3)
    {
        for (float i = 0.0; i <= 15.0; i = i + 2)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(7.0f + i, -1.0f, 12.0f + j));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.004f));
            glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            if (!depthPass) {
                normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
                glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
            }
            molid.Draw(shader);
        }
    }

    for (float j = 0.0; j <= 15.0; j = j + 3)
    {
        for (float i = 0.0; i <= 7.0; i = i + 2)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-25.0f + i, -1.0f, -8.0f - j));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.004f));
            glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            if (!depthPass) {
                normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
                glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
            }
            molid.Draw(shader);
        }
    }

    //draw the main tree

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -1.0f, -13.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.05f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    christmas_tree.Draw(shader);

    //main houses
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(19.0f, -1.0f, -12.0f));
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.05f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    home.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(19.0f, -1.0f, -5.0f));
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.05f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    home.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(19.0f, -1.0f, -20.0f));
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.05f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    home.Draw(shader);


    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-25.0f, -1.0f, 20.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.05f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    home.Draw(shader);

    //felinare 

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-20.0f, -1.0f, 20.0f));
    model = glm::scale(model, glm::vec3(0.8f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    felinar.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(19.0f, -1.0f, -1.0f));
    model = glm::scale(model, glm::vec3(0.8f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    felinar.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(16.0f, -1.0f, -24.0f));
    model = glm::scale(model, glm::vec3(0.8f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    felinar.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(14.0f, -1.0f, 7.0f));
    model = glm::scale(model, glm::vec3(0.8f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    felinar.Draw(shader);

    //draw the cat 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(13.0f, -1.0f, -7.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.01f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    cat.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10.0f, -1.0f, -17.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.01f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    cat.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, -1.0f, 25.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.01f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    cat.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-22.0f, -1.0f, 12.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.01f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    cat.Draw(shader);

    //draw bustean pt foc de tabara
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(20.0f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.1f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    bustean.Draw(shader);


    //draw gifts
    //g1
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -1.0f, -10.0f));
    model = glm::scale(model, glm::vec3(5.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    cadou1.Draw(shader);
    //g2
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, -1.0f, -9.0f));
    model = glm::scale(model, glm::vec3(2.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    cadou2.Draw(shader);
    //g3
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.7f, -1.0f, -10.0f));
    model = glm::scale(model, glm::vec3(3.5f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    cadou3.Draw(shader);
    //g4
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, -1.0f, -9.0f));
    model = glm::scale(model, glm::vec3(4.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    cadou4.Draw(shader);
    //g5
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.5f, -1.0f, -9.0f));
    model = glm::scale(model, glm::vec3(3.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    cadou5.Draw(shader);

    //draw snowmen
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -1.0f, -7.0f));
    model = glm::scale(model, glm::vec3(0.01f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    snowman1.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-10.0f, -1.0f, 7.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.01f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    snowman1.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-12.0f, -1.0f, 4.0f));
    //model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.01f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    snowman1.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-9.0f, -1.0f, 4.0f));
   // model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.01f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    snowman1.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(7.5f, 1.4f, 7.5f));
    // model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.007f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    snowman1.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-22.0f, -1.0f, -3.0f));
    // model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.01f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    snowman1.Draw(shader);

    


    double currentTimeStamp = glfwGetTime();
    updateDelta(currentTimeStamp - lastTimeStamp);
    lastTimeStamp = currentTimeStamp;
    int var = 0;

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-15.0f, -1.0f, -9.0f));
    model = glm::scale(model, glm::vec3(0.01f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
    if ( var < 60)
    {
       model = glm::translate(model, glm::vec3(delta, 0.0f, 0.0f));
       var++;
   }
    else
    {
        var = 0;
        delta = 0;
    }
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    snowman1.Draw(shader);

    

    //draw animated snowman 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(8.0f, -1.0f, -5.0f));
    model = glm::scale(model, glm::vec3(0.01f));
    model = glm::translate(model, glm::vec3(snowmanX, 1.3f, snowmanZ));
    model = glm::rotate(model, glm::radians(snowmanAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    snowman1.Draw(shader);


    //draw santa 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(6.0f, -1.0f, 12.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.005f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    santa.Draw(shader);


    //draw the house 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(6.0f, -1.0f, 7.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.04f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    house.Draw(shader);

    //sustinere casa
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(6.0f, -1.1f, 9.5f));
    // model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(2.7f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    trunck.Draw(shader);

    //sanie mosu 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(16.0f, 0.0f, 9.0f));
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.05f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    sanie.Draw(shader);


    // draw the ground 
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(3.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    ground.Draw(shader);
}

void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    glGenFramebuffers(1, &shadowMapFBO);

    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix
    glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 80.0f;
    glm::mat4 lightProjection = glm::ortho(-60.0f, 60.0f, -60.0f, 60.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}

void renderScene() {
    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderObjects(depthMapShader, true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render depth map on screen - toggled with the M key

    if (showDepthMap) {

        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
        glClear(GL_COLOR_BUFFER_BIT);
        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else {

        // final scene rendering pass (with shadows)

        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "fogSwitch"), fog);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));


        renderObjects(myBasicShader, false);

        //draw a white cube around the light
    }
    mySkyBox.Draw(skyboxShader, view, projection);

}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    initFBO();

    setWindowCallbacks();

    glCheckError();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
