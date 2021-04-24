#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

enum ShaderType { vertex, fragment };

GLuint LoadSingleShader(const char* shaderFilePath, ShaderType type)
{
    // Create a shader id.
    GLuint shaderID = 0;
    if (type == vertex)
        shaderID = glCreateShader(GL_VERTEX_SHADER);
    else if (type == fragment)
        shaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Try to read shader codes from the shader file.
    std::string shaderCode;
    std::ifstream shaderStream(shaderFilePath, std::ios::in);
    if (shaderStream.is_open())
    {
        std::string Line = "";
        while (getline(shaderStream, Line))
            shaderCode += "\n" + Line;
        shaderStream.close();
    }
    else
    {
        std::cerr << "Impossible to open " << shaderFilePath << ". "
            << "Check to make sure the file exists and you passed in the "
            << "right filepath!"
            << std::endl;
        return 0;
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Shader.
    std::cerr << "Compiling shader: " << shaderFilePath << std::endl;
    char const* sourcePointer = shaderCode.c_str();
    glShaderSource(shaderID, 1, &sourcePointer, NULL);
    glCompileShader(shaderID);

    // Check Shader.
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> shaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(shaderID, InfoLogLength, NULL, shaderErrorMessage.data());
        std::string msg(shaderErrorMessage.begin(), shaderErrorMessage.end());
        std::cerr << msg << std::endl;
        return 0;
    }
    else
    {
        if (type == vertex)
            printf("Successfully compiled vertex shader!\n");
        else if (type == fragment)
            printf("Successfully compiled fragment shader!\n");
    }

    return shaderID;
}

GLuint LoadShaders(const char* vertexFilePath, const char* fragmentFilePath)
{
    // Create the vertex shader and fragment shader.
    GLuint vertexShaderID = LoadSingleShader(vertexFilePath, vertex);
    GLuint fragmentShaderID = LoadSingleShader(fragmentFilePath, fragment);

    // Check both shaders.
    if (vertexShaderID == 0 || fragmentShaderID == 0) return 0;

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Link the program.
    printf("Linking program\n");
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    // Check the program.
    glGetProgramiv(programID, GL_LINK_STATUS, &Result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(programID, InfoLogLength, NULL, ProgramErrorMessage.data());
        std::string msg(ProgramErrorMessage.begin(), ProgramErrorMessage.end());
        std::cerr << msg << std::endl;
        glDeleteProgram(programID);
        return 0;
    }
    else
    {
        printf("Successfully linked program!\n");
    }

    // Detach and delete the shaders as they are no longer needed.
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Playground", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    // Initialize GLEW library
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW Error!" << std::endl;

    // Print GL Version
    std::cout << glGetString(GL_VERSION) << std::endl;

    {
        float positions[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f,
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0,
        };

        
        VertexArray vao;
        VertexBuffer vbo(positions, 4 * 2 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        vao.AddBuffer(vbo, layout);

        // index buffer
        IndexBuffer ibo(indices, 6);

        unsigned int shader = LoadShaders("res/shaders/vert.shader", "res/shaders/frag.shader");
        GLCall(glUseProgram(shader));

        GLCall(int location = glGetUniformLocation(shader, "u_Color"));
        ASSERT(location != -1);
        GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

        // Unbind everything
        GLCall(glBindVertexArray(0));
        GLCall(glUseProgram(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        float r = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

            vao.Bind();
            ibo.Bind();

            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}