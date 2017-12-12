
#include <iostream>

// C++ library for creating windows with OpenGL contexts and receiving 
// input and events http://www.glfw.org/ 
#include <glfw/glfw3.h> 

// C++ binding for the OpenGL API. 
// https://github.com/cginternals/glbinding
#include <glbinding/Binding.h>

#include "screenalignedtriangle.h"


// From http://en.cppreference.com/w/cpp/language/namespace:
// "Unnamed namespace definition. Its members have potential scope 
// from their point of declaration to the end of the translation
// unit, and have internal linkage."
namespace
{

auto example = ScreenAlignedTriangle();

const auto canvasWidth = 640; // in pixel
const auto canvasHeight = 1080 - 170; // in pixel

// "The size callback ... which is called when the window is resized."
// http://www.glfw.org/docs/latest/group__window.html#gaa40cd24840daa8c62f36cafc847c72b6
void resizeCallback(GLFWwindow * /*window*/, int width, int height)
{
    example.resize(width, height);
}

// "The key callback ... which is called when a key is pressed, repeated or released."
// http://www.glfw.org/docs/latest/group__input.html#ga7e496507126f35ea72f01b2e6ef6d155
void keyCallback(GLFWwindow * /*window*/, int key, int /*scancode*/, int action, int mods)
{
    if (action != GLFW_RELEASE)
        return;

    switch (key)
    {
    case GLFW_KEY_F5:
        example.loadShaders();
        break;

    case GLFW_KEY_R:
        example.reset();
        break;
        
    case GLFW_KEY_M:
        example.switchDrawMode();
        break;

    case GLFW_KEY_P:
        example.benchmarkAll();
        break;

    case GLFW_KEY_1:
        example.switchDrawMode(ScreenAlignedTriangle::Mode::Quad);
        break;

    case GLFW_KEY_2:
        example.switchDrawMode(ScreenAlignedTriangle::Mode::Triangle);
        break;

    case GLFW_KEY_3:
        example.switchDrawMode(ScreenAlignedTriangle::Mode::Quad_Fill_Rectangle_Extension);
        break;

    case GLFW_KEY_4:
        example.switchDrawMode(ScreenAlignedTriangle::Mode::Two_Triangles_Two_DrawCalls);
        break;

    case GLFW_KEY_5:
        example.switchDrawMode(ScreenAlignedTriangle::Mode::AVC_One_DrawCall);
        break;

    case GLFW_KEY_T:
        mods == GLFW_MOD_SHIFT ? example.incrementReplaySpeed() : example.decrementReplaySpeed();
        break;
    }
}


// "In case a GLFW function fails, an error is reported to the GLFW 
// error callback. You can receive these reports with an error
// callback." http://www.glfw.org/docs/latest/quick.html#quick_capture_error
void errorCallback(int errnum, const char * errmsg)
{
    std::cerr << errnum << ": " << errmsg << std::endl;
}


}


int main(int /*argc*/, char ** /*argv*/)
{
    if (!glfwInit())
    {
        return 1;
    }

    glfwSetErrorCallback(errorCallback);

    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(canvasWidth, canvasHeight, "", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 2;
    }

    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwSetKeyCallback(window, keyCallback);

    std::cout << "Screen Aligned Quad vs. Triangle(s)" << std::endl << std::endl;

    std::cout << "Key Binding: " << std::endl << std::endl
        << "  [F5] reload shaders" << std::endl
        << "  [r]  reset record and record anew" << std::endl << std::endl
        << "  [m]  switch over to next draw mode" << std::endl
        << "  [1]  ... draw mode (1) - " << ScreenAlignedTriangle::s_modeDescriptions[0] << std::endl
        << "  [2]  ... draw mode (2) - " << ScreenAlignedTriangle::s_modeDescriptions[1] << std::endl
        << "  [3]  ... draw mode (3) - " << ScreenAlignedTriangle::s_modeDescriptions[2] << std::endl
        << "  [4]  ... draw mode (4) - " << ScreenAlignedTriangle::s_modeDescriptions[3] << std::endl
        << "  [5]  ... draw mode (5) - " << ScreenAlignedTriangle::s_modeDescriptions[4] << std::endl << std::endl
        << "  [P]  measure performance (may take a few minutes)" << std::endl << std::endl
        << "  [T]  increment replay speed (by magnitude)" << std::endl
        << "  [t]  decrement replay speed (by magnitude)" << std::endl
        << std::endl;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glbinding::Binding::initialize(false);

    example.resize(canvasWidth, canvasHeight);
    example.initialize();

    while (!glfwWindowShouldClose(window)) // main loop
    {
        glfwPollEvents();

        example.render();

        glfwSwapBuffers(window);
    }

    glfwMakeContextCurrent(nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
