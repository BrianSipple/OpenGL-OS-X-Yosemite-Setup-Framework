#define GLEW_STATIC
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include "shader.h"


/////// Globals ///////
GLuint program;
GLuint vbo;     // Vertex Buffer Object
GLuint vao;     // Vertex Array Object

GL::Shader mVertex, mGeometry, mFragment;

struct vertex
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

static const vertex vertices[] = {
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


// Create a texture from an image file
GLuint loadTextureImg(const GLchar* path) {

    GLuint texture;
    glGenTextures(1, &texture);

    int width, height;
    unsigned char* image;

    glBindTexture(GL_TEXTURE_2D, texture);
    image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}



/**
* Same as createProgramLinkedWithShaders*(), minus the linking,
* to allow for additional, pre-link processing after the shaders are
* attached,
*/
void createProgramWithShadersVF(
    GLuint& program,
    GLchar* vertexShaderPath,
    GLchar* fragmentShaderPath)
{
    program = glCreateProgram();

    mVertex = GL::Shader(GL_VERTEX_SHADER, vertexShaderPath);
    mVertex.attachTo(program);

    mFragment = GL::Shader(GL_FRAGMENT_SHADER, fragmentShaderPath);
    mFragment.attachTo(program);
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
    GLuint typeSize,
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
    GLuint typeSize,
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


int main() {

    // Initialize GLFW, and if it fails to initialize
    // for any reason, print it out to STDERR.

    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW.");
        exit(EXIT_FAILURE);
    }

    // Set the error callback that we wired up above
    glfwSetErrorCallback(errorCallback);

    // Set up OpenGL options
    // Use version 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // fullscreen

    // If the window fails to be created, print out the error,
    // clean up GLFW and exit the program.
    if (!window) {
        fprintf(stderr, "Failed to create OPENGL window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Make the OpenGL window context active
    glfwMakeContextCurrent(window);

    // Set the keyboard callback so that when we press ESC, it knows what to do.
    glfwSetKeyCallback(window, keyPressCallback);

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
    createVertexAttribPointerFromName(program, "position", GL_FLOAT, sizeof(GLfloat), GL_FALSE, 3, sizeof(vertex), offsetof(vertex, x));


    //glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    // ... or by its layout "location" number
    createVertexAttribPointerFromLayoutPos(1, GL_FLOAT, sizeof(GLfloat), GL_FALSE, 3, sizeof(vertex), offsetof(vertex, r));

    // // Get a pointer to the buffer's data store
    // void * ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    //
    // // Copy data into it
    // memcpy(ptr, vertices, sizeof(vertices));
    //
    // // Tell OPEN_GL that we're done with the pointer
    // glUnmapBuffer(GL_ARRAY_BUFFER);


    // Pass data to OPEN_GL for instertion into the buffer at offset zero.
    //glClearBufferSubData(GL_ARRAY_BUFFER, GL_RGBA32F, 0, sizeof(vertices), GL_RGBA, GL_FLOAT, vertices);





    glUseProgram(program);

    while(!glfwWindowShouldClose(window)) {

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a rectangle from the 2 triangles using 6 indices
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Swap the back buffer and front buffer after
        // we've finished drawing
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    tearDown();

    return 0;
}
