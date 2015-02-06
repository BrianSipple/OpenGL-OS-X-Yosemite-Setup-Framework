#include "shader.h"

namespace GL {
    // Public

    Shader::Shader(){}
    /**
     * Constructor that creates a shader, sources data into it, and compiles
     * it into code that can be executed by the graphics card
     */
    Shader::Shader(GLenum type, char* location) {
        _handle = glCreateShader(type);

        GLchar* src = Util::fileToBuffer(location);
        glShaderSource(_handle, 1, (const GLchar**)&src, NULL);

        printf("Compiling Shader: %s ID: %i\n", location, _handle);
        glCompileShader(_handle);

        if (status() == GL_FALSE) {
            logError(location);
        }
    }

    GLuint Shader::getHandle() {
        return _handle;
    }

    /**
     * Checks to see if a shader has compiled correctly.
     */
    GLint Shader::status() {
        GLint status;
        glGetShaderiv(_handle, GL_COMPILE_STATUS, &status);

        return status;
    }

    void Shader::attachTo(GLuint programId) {
        printf("Attaching shader: %i to program: %i\n", _handle, programId);
        glAttachShader(programId, _handle);
    }


    void Shader::detachFrom(GLuint programId) {
        printf("Detaching shader: %i from program: %i\n", _handle, programId);
        glDetachShader(programId, _handle);
        printf("Deleting shader: %i\n", _handle);
        glDeleteShader(_handle);
    }


    /**
     * Creates a program linked with a Vertex shader
     */
    void Shader::createProgramLinkedWithShadersV(
        GLuint& program,
        Shader& vertexShaderConst,
        GLchar* vertexShaderPath
    )
    {
        printf("Creating program linked with Vertex Shader");

        program = glCreateProgram();

        vertexShaderConst = Shader(GL_VERTEX_SHADER, vertexShaderPath);
        vertexShaderConst.attachTo(program);

        glLinkProgram(program);

        vertexShaderConst.detachFrom(program);
    }


    /**
     * Creates a program linked with a Fragment shader
     */
    void Shader::createProgramLinkedWithShadersF(
        GLuint& program,
        Shader& fragmentShaderConst,
        GLchar* fragmentShaderPath
    )
    {
        printf("Creating program linked with Vertex Shader");

        program = glCreateProgram();

        fragmentShaderConst = Shader(GL_FRAGMENT_SHADER, fragmentShaderPath);
        fragmentShaderConst.attachTo(program);

        glLinkProgram(program);

        fragmentShaderConst.detachFrom(program);
    }




    /**
     * Creates a program linked with a vertex and fragment shader
     */
    void Shader::createProgramLinkedWithShadersVF(
        GLuint& program,
        Shader& vertexShaderConst,
        GLchar* vertexShaderPath,
        Shader& fragmentShaderConst,
        GLchar* fragmentShaderPath
    )
    {
        printf("Creating program linked with Vertex and Fragment Shaders");

        program = glCreateProgram();

        vertexShaderConst = Shader(GL_VERTEX_SHADER, vertexShaderPath);
        fragmentShaderConst = Shader(GL_FRAGMENT_SHADER, fragmentShaderPath);

        vertexShaderConst.attachTo(program);
        fragmentShaderConst.attachTo(program);

        glLinkProgram(program);

        // Shaders are safely linked -- delete them
        vertexShaderConst.detachFrom(program);
        fragmentShaderConst.detachFrom(program);
    }


    /**
    * Creates a program linked with a vertex, geometry, and fragment shader
    */
    void Shader::createProgramLinkedWithShadersVGF(
        GLuint& program,
        Shader& vertexShaderConst,
        GLchar* vertexShaderPath,
        Shader& geometryShaderConst,
        GLchar* geometryShaderPath,
        Shader& fragmentShaderConst,
        GLchar* fragmentShaderPath
    )
    {
        printf("Creating program linked with Vertex and Fragment Shaders");

        program = glCreateProgram();

        vertexShaderConst = Shader(GL_VERTEX_SHADER, vertexShaderPath);
        geometryShaderConst = Shader(GL_GEOMETRY_SHADER, geometryShaderPath);
        fragmentShaderConst = Shader(GL_FRAGMENT_SHADER, fragmentShaderPath);

        vertexShaderConst.attachTo(program);
        geometryShaderConst.attachTo(program);
        fragmentShaderConst.attachTo(program);

        glLinkProgram(program);

        // Shaders are safely linked -- delete them
        vertexShaderConst.detachFrom(program);
        geometryShaderConst.detachFrom(program);
        fragmentShaderConst.detachFrom(program);
    }

    /**
    * Same as createProgramLinkedWithShaders*(), minus the linking,
    * to allow for additional, pre-link processing after the shaders are
    * attached,
    */
    void Shader::createProgramWithShadersVF(
        GLuint& program,
        Shader& vertexShaderConst,
        GLchar* vertexShaderPath,
        Shader& fragmentShaderConst,
        GLchar* fragmentShaderPath
    )
    {
        program = glCreateProgram();

        vertexShaderConst = Shader(GL_VERTEX_SHADER, vertexShaderPath);
        vertexShaderConst.attachTo(program);

        fragmentShaderConst = Shader(GL_FRAGMENT_SHADER, fragmentShaderPath);
        fragmentShaderConst.attachTo(program);
    }


    // END Public

    // Private
    void Shader::logError(char* location) {
        GLint infoLogLength;
        glGetShaderiv(_handle, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(_handle, infoLogLength, NULL, strInfoLog);

        fprintf(stderr, "Shader Compile failure in %s:\n%s\n", location, strInfoLog);
        delete[] strInfoLog;
    }
    // END Private
}
