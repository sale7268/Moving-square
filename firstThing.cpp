#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective



using namespace std;

GLFWwindow* initialization()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
    {
        cerr << "GLFW Init failed.";
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        cerr << "Window failed to open.";
        exit(-1);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //Init glew
    glewExperimental = GL_TRUE;
    glewInit();

    return window;
}


GLuint compile_shader() {
    // Define shader sourcecode
    const char* vertex_shader_src =
        "#version 330 core\n"
        "uniform vec2 offset;"
        "layout (location = 0) in vec3 pos;\n"
        "void main() {\n"
        "   gl_Position = vec4(pos.x + offset.x, pos.y + offset.y, pos.z, 1.0);\n"
        "}\n";
    const char* fragment_shader_src =
        "#version 330 core\n"
        "uniform vec4 triangleColor;"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "   FragColor = triangleColor;\n"
        "}\n";

    // Define some vars
    const int MAX_ERROR_LEN = 512;
    GLint success;
    GLchar infoLog[MAX_ERROR_LEN];

    // Compile the vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);

    // Check for errors
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, MAX_ERROR_LEN, NULL, infoLog);
        std::cout << "vertex shader compilation failed:\n" << infoLog << std::endl;
        abort();
    }

    // Compile the fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);

    // Check for errors
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, MAX_ERROR_LEN, NULL, infoLog);
        std::cout << "fragment shader compilation failed:\n" << infoLog << std::endl;
        abort();
    }

    // Link the shaders
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Check for errors
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, MAX_ERROR_LEN, NULL, infoLog);
        std::cout << "shader linker failed:\n" << infoLog << std::endl;
        abort();
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Enable the shader here since we only have one
    glUseProgram(shader_program);

    return shader_program;
}

void load_geometry(GLuint* vao, GLuint* vbo, GLsizei* vertex_count) {
    // Send the vertex data to the GPU
    {
        // Generate the data on the CPU
        GLfloat vertices[] = {
            -0.5f,  0.5f, 0.0f, // Vertex 1 (X, Y)
             0.5f, -0.5f, 0.0f, // Vertex 2 (X, Y)
            -0.5f, -0.5f, 0.0f,  // Vertex 3 (X, Y)
             0.5f, 0.5f, 0.0f, //vertex 4
             0.5f,  -0.5f, 0.0f, // Vertex 1 (X, Y)
            -0.5f, 0.5f, 0.0f, // Vertex 2 (X, Y)
             0.5f, 0.5f, 0.0f,  // Vertex 3 (X, Y)
            -0.5f, -0.5f, 0.0f, //vertex 4
        };
        *vertex_count = sizeof(vertices) / sizeof(vertices[0]);

        // Use OpenGL to store it on the GPU
        {
            // Create a Vertex Buffer Object on the GPU
            glGenBuffers(1, vbo);
            // Tell OpenGL that we want to set this as the current buffer
            glBindBuffer(GL_ARRAY_BUFFER, *vbo);
            // Copy all our data to the current buffer!
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        }
    }

    // Tell the GPU how to interpret our existing vertex data
    {
        // Create a Vertex Array Object to hold the settings
        glGenVertexArrays(1, vao);

        // Tell OpenGL that we want to set this as the current vertex array
        glBindVertexArray(*vao);

        // Tell OpenGL the settings for the current 0th vertex array!
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

        // Enable the 0th vertex attrib array!
        glEnableVertexAttribArray(0);
    }
}


void render_scene(GLFWwindow* window, GLsizei vertex_count, GLuint shader_program) {
    // Set the clear color
    glClearColor(0.7f, 0.0f, 0.5f, 1.0f);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    static float red = 0.0f;
    static float dir = 1.0f;
    if (dir > 0.0) {
        red += 0.001f;
        if (red > 1.0) {
            dir = -1.0;
        }
    }
    else {
        red -= 0.001f;
        if (red < 0.0) {
            dir = 1.0;
        }
    }
     
    GLint color = glGetUniformLocation(shader_program, "triangleColor");
    GLint color_location = glGetUniformLocation(shader_program, "offset");
    

    glUniform2f(color_location, red, 0.0);
    glUniform4f(color, red, 0.0, 0.0, 1.0);
    // Draw the current vao/vbo, with the current shader
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
   

    glUniform2f(color_location, 0.0, red);
    glUniform4f(color, 0.0, red, red, 1.0);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
        
    // Display the results on screen
    glfwSwapBuffers(window);
}


int main(void)
{

    GLFWwindow* window = initialization();
    GLuint vao; 
    GLuint vbo; 
    GLsizei vertex_count;
    GLuint shader_program;
 

    load_geometry(&vao, &vbo, &vertex_count);
    shader_program = compile_shader();
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        render_scene(window, vertex_count, shader_program);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();
    return 0;
}
