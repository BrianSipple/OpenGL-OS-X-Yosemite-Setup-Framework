#define GLEW_STATIC
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include "shader.h"


/////// Globals ///////
GLFWwindow* mWindow;
GLuint program;
GLuint vbo;     // Vertex Buffer Object
GLuint vao;     // Vertex Array Object

GL::Shader mVertex, mGeometry, mFragment;

struct vertexPosColor
{
    // Position
    float x;
    float y;
    float z;

    // color
    float r;
    float g;
    float b;
};

static const vertexPosColor vertices[] = {
    -0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,  // Red vertex, top-left
    0.5f,  0.5f, 0.5f,   0.0f, 1.0f, 0.0f,  // Green vertex, top-right
    0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,  // Blue verex, bottom-right
    -0.5f, -0.5f, 0.5f,   1.0f, 1.0f, 1.0f,  // White vertex, bottom-left
};



static const GLfloat positions[] = {
    -1.0f, -1.0f, 0.5f,
    1.0f, -1.0f, 0.5f,
    1.0f,  1.0f, 0.5f,
    -1.0f,  1.0f, 0.5f
};

static const GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f
};

// The error callback will just print any errors that GLFW hits.
void errorCallback(int error, const char* description) {
    fputs(description, stderr);
}

// The key callback should handle any input,
// To start, we'll just use the ESC key
void keyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    } else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        return;
    }
}


/**
* Helper function around creating a vertex attribute pointer for
* vertex floats.
*
* @param numItems: the number of values represented by the atrribute
* @param vertexElemLength: the length of the vertex element
* @param offset: the offset within the element
*                from which the pointer should begin
*/
void createVertexAttribPointerFromName(
    GLuint program,
    char* attribName,
    GLenum type,
    GLboolean normalized,
    GLuint numItems,
    GLuint vertexElemLength,
    GLuint offset)
{

    GLint attribIndex = glGetAttribLocation(program, attribName);

    if (attribIndex != -1) {
        glVertexAttribPointer(
            attribIndex,
            numItems,
            type,
            normalized,
            vertexElemLength,
            (void*)(offset)
        );

        glEnableVertexAttribArray(attribIndex);
    }
}

void createVertexAttribPointerFromLayoutPos(
    GLint layoutPosition,
    GLenum type,
    GLboolean normalized,
    GLuint numItems,
    GLuint vertexElemLength,
    GLuint offset)
{
    glVertexAttribPointer(
        layoutPosition,
        numItems,
        type,
        normalized,
        vertexElemLength,
        (void*)(offset)
    );

    glEnableVertexAttribArray(layoutPosition);
}


void tearDown() {

    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glfwTerminate();
}

void initWindow()
{
    // Set up OpenGL options
    // Use version 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    mWindow = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // fullscreen

    // If the window fails to be created, print out the error,
    // clean up GLFW and exit the program.
    if (!mWindow) {
        fprintf(stderr, "Failed to create OPENGL window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
}


int main() {

    // Initialize GLFW, and if it fails to initialize
    // for any reason, print it out to STDERR.

    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW.");
        exit(EXIT_FAILURE);
    }

    initWindow();

    // Make the OpenGL window context active
    glfwMakeContextCurrent(mWindow);

    // Set the callbacks that we wired up above
    glfwSetKeyCallback(mWindow, keyPressCallback);
    glfwSetErrorCallback(errorCallback);

    printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

    // Force GLEW to use a modern OpenGL method for checking
    // if a function is available.
    glewExperimental = GL_TRUE;

    // Make sure all extensions will be exposed in GLEW and initialize GLEW.
    glewInit();


    GL::Shader::createProgramLinkedWithShadersVF(program, mVertex, "vertex.shader", mFragment, "fragment.shader");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object
    glGenBuffers(1, &vbo);

    // Bind our buffer object to the GL_ARRAY_BUFFER binding.
    // Subsequent use of glVertexAttribPointer will then reference this buffer.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    // We can specify a Vertex Attribute pointer by is name...
    createVertexAttribPointerFromName(program, "position", GL_FLOAT, GL_FALSE, 3, sizeof(vertexPosColor), offsetof(vertexPosColor, x));

    // ... or by its layout "location" number
    createVertexAttribPointerFromLayoutPos(1, GL_FLOAT, GL_FALSE, 3, sizeof(vertexPosColor), offsetof(vertexPosColor, r));

    glUseProgram(program);

    while(!glfwWindowShouldClose(mWindow)) {

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a rectangle from the 2 triangles using 6 indices
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Swap the back buffer and front buffer after
        // we've finished drawing
        glfwSwapBuffers(mWindow);

        glfwPollEvents();
    }

    tearDown();

    return 0;
}
