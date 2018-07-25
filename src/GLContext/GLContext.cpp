//
// Created by vlado on 24-Jul-18.
//

#include "GLContext.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include "../util/util.h"

static int infoLogLength;

GLContext::GLContext(const GLWindow *context) : context(context), scene(nullptr) {
    glfwSetInputMode((*context).window.ref, GLFW_STICKY_KEYS, GL_TRUE);
}

GLContext::~GLContext() {
    for (GLuint program : shaderPrograms) {
        glDeleteProgram(program);
    }
};

unsigned GLContext::createProgram(const char *vertexFilePath,
                                  const char *geometryFilePath,
                                  const char *fragmentFilePath) {
    infoLogLength = 0;

    const GLuint shaders[3] = {
            createShader(Shaders::vertex, vertexFilePath),
            createShader(Shaders::geometry, geometryFilePath),
            createShader(Shaders::fragment, fragmentFilePath)
    };

    // Link program
    printf("Linking program\n");
    GLuint programID = glCreateProgram();

    for (GLuint shader: shaders) {
        if (shader != 0)
            glAttachShader(programID, shader);
    }

    glLinkProgram(programID);

    // Check program
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> ProgramErrorMessage((unsigned) infoLogLength + 1);
        glGetProgramInfoLog(programID, infoLogLength, nullptr, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    // Cleanup
    for (GLuint shader: shaders) {
        if (shader != 0) {
            glDetachShader(programID, shader);
            glDeleteShader(shader);
        }
    }

    shaderPrograms.push_back(programID);

    return programID;
}

GLuint GLContext::createShader(GLContext::Shaders type, const char *filePath) {
    if (filePath == nullptr) return (GLuint) nullptr;

    // Create shader
    GLuint shaderID = glCreateShader(type);

    // Read shader code from the file
    std::string shaderCode;
    std::ifstream shaderStream(filePath, std::ios::in);
    if (shaderStream.is_open()) {
        std::stringstream stringStream;
        stringStream << shaderStream.rdbuf();
        shaderCode = stringStream.str();
        shaderStream.close();
    } else {
        printf("Couldn't open shader file: %s.\n", filePath);
        getchar();
        return 0;
    }

    //  Compile shader
    printf("Compiling shader : %s\n", filePath);
    char const *sourcePointer = shaderCode.c_str();
    glShaderSource(shaderID, 1, &sourcePointer, nullptr);
    glCompileShader(shaderID);

    // Check for errors
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> shaderErrorMessage((unsigned) infoLogLength + 1);
        glGetShaderInfoLog(shaderID, infoLogLength, nullptr, &shaderErrorMessage[0]);
        printf("%s\n", &shaderErrorMessage[0]);
    }

    printf("Shader '%d' successfully compiled.\n", shaderID);
    return shaderID;
}

unsigned GLContext::loadTexture(const char *filePath) {

    util::FExtension ext = util::getFileExtension(filePath);

    GLuint texture;
    switch (ext) {
        case util::bmp:
            texture = util::loadBMP(filePath);
            textures.push_back(texture);
            return texture;
        case util::dds:
            texture = util::loadDDS(filePath);
            textures.push_back(texture);
            return texture;
        default:
            printf("Unsupported texture format %s", filePath);
            return 0;
    }
}

void GLContext::useProgram(unsigned int program) {
    this->program = program;
    glUseProgram(program);
}

const GLScene *GLContext::getScene() const {
    return scene;
}

void GLContext::attachScene(const GLScene *scene) {
    this->scene = scene;
    scene->program = program;
}

void GLContext::render() {
    scene->beforeRender();


    double lastTime = glfwGetTime();
    int frames = 0;

    while (!shouldBreak && glfwWindowShouldClose((*context).window.ref) == 0) {
        if (printFps) {
            double currentTime = glfwGetTime();
            frames++;
            if (currentTime - lastTime >= 1.0) {
                printf("%fms\n", 1000.f / frames);
                frames = 0;
                lastTime = currentTime;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        (*inputHandler).processInput();
        (*scene).render();
        glfwSwapBuffers((*context).window.ref);
        glfwPollEvents();
    }

    scene->afterRender();
}

void GLContext::breakLoop() {
    shouldBreak = true;
}

void GLContext::setInputHandler(const GLInputHandler *handler) {
    inputHandler = handler;
}

const GLInputHandler *GLContext::getInputHandler() const {
    return inputHandler;
}
