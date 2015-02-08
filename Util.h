// Namespace for our utility function that will read a file and
// return the contents of that file as a C string.
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>

namespace Util {
    class Files {
    public:

        static char* fileToBuffer(char *file);

        static void loadKtx(GLchar* filePath);
    };
}
