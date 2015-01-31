#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include "Util.h"

namespace GL {
    class Shader {
    public:
        Shader();
        Shader(GLenum shaderType, char* shaderLocation);
        GLuint getHandle(); // Returns the ID referring to this shader in the GL.
        GLint status(); // Returns the status of compiling the shader.
        void attachTo(GLuint programId); // Attaches the shader to a GL program.
        void detachFrom(GLuint programId); // Detaches the shader from a GL program.

        // Creates a program linked with a vertex shader
        static void createProgramLinkedWithShadersV(
            GLuint& program,            // pointer to the program
            Shader& vertexShaderConst,   // pointer to unCompiled shader object
            GLchar* vertexShaderPath    // path to the shader source
        );

        // Creates a program linked with vertex, geometry and fragment shaders
        static void createProgramLinkedWithShadersVGF(
            GLuint& program,            // pointer to the program
            Shader& vertexShaderConst,   // pointer to unCompiled shader object
            GLchar* vertexShaderPath,    // path to the shader source
            Shader& geometryShaderConst,   // pointer to unCompiled shader object
            GLchar* geometryShaderPath,    // path to the shader source
            Shader& fragmentShaderConst,   // pointer to unCompiled shader object
            GLchar* fragmentShaderPath    // path to the shader source
        );

        // Creates a program linked with vertex and fragment shaders
        static void createProgramLinkedWithShadersVF(
            GLuint& program,            // pointer to the program
            Shader& vertexShaderConst,   // pointer to unCompiled shader object
            GLchar* vertexShaderPath,    // path to the shader source
            Shader& fragmentShaderConst,   // pointer to unCompiled shader object
            GLchar* fragmentShaderPath    // path to the shader source
        );



        /**
        * Same as createProgramLinkedWithShaders*(), minus the linking,
        * to allow for additional, pre-link processing after the shaders are
        * attached,
        */
        static void createProgramWithShadersVF(
            GLuint& program,            // pointer to the program
            Shader& vertexShaderConst,   // pointer to unCompiled shader object
            GLchar* vertexShaderPath,    // path to the shader source
            Shader& fragmentShaderConst,   // pointer to unCompiled shader object
            GLchar* fragmentShaderPath    // path to the shader source
        );

        

    private:
        GLuint _handle;
        void logError(char* location);
    };
}

#endif
