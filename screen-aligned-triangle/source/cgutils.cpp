
#include "cgutils.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/vec4.hpp>
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#pragma warning(pop)

#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols

using namespace gl;

namespace cgutils
{

// This function is reads the contents of the back left buffer and 
// stores it into a PPM formated image file http://netpbm.sourceforge.net/doc/ppm.html
void captureAsPPM(const char * filePath, const unsigned int width, const unsigned int height)
{
    // create buffer for GL_RGB in GL_UNSIGNED_BYTE
    auto buffer = std::vector<GLubyte>(width * height * 3); 

    // bind the default frame buffer and read from its back left buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glReadBuffer(GL_BACK_LEFT);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());

    // write the buffer into a PPM formated image file
    auto ppm = std::ofstream();
    ppm.open(filePath, std::ios::binary);     

    if (ppm.fail())
    {
        std::cerr << "Cannot open output file '" << filePath << "' (review mode)." << std::endl;
        ppm.close();
        return;
    }

    ppm << "P6\n" << width << " " << height << "\n255\n"; // PPM header
    
    // mirror the image horizontally
    const auto scanLineSize = width * 3;
    for(auto y = static_cast<int>(height) - 1; y >= 0; --y)
        ppm.write(reinterpret_cast<const char *>(buffer.data()) + scanLineSize * y, scanLineSize);

    ppm.close();
}


// Read raw binary file into a char vector (probably the fastest way).

std::vector<char> rawFromFile(const char * filePath)
{
    auto stream = std::ifstream(filePath, std::ios::in | std::ios::binary | std::ios::ate);

    if (!stream)
    {
        std::cerr << "Reading from file '" << filePath << "' failed." << std::endl;
        return std::vector<char>();
    }

    stream.seekg(0, std::ios::end);

    const auto size = stream.tellg();
    auto raw = std::vector<char>(size);

    stream.seekg(0, std::ios::beg);
    stream.read(raw.data(), size);

    return raw;
}

std::string textFromFile(const char * filePath)
{
    const auto text = rawFromFile(filePath);
    return std::string(text.begin(), text.end());
}

bool checkForCompilationError(GLuint shader, const std::string & identifier)
{
    auto success = static_cast<GLint>(GL_FALSE);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success != static_cast<GLint>(GL_FALSE))
        return true;

    auto length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    std::vector<char> log(length);

    glGetShaderInfoLog(shader, length, &length, log.data());

    std::cerr
        << "Compiler error in " << identifier << ":" << std::endl
        << std::string(log.data(), length) << std::endl;

    return false;
}

bool checkForLinkerError(GLuint program, const std::string & identifier)
{
    auto success = static_cast<GLint>(GL_FALSE);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success != static_cast<GLint>(GL_FALSE))
        return true;

    auto length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

    std::vector<char> log(length);

    glGetProgramInfoLog(program, length, &length, log.data());

    std::cerr
        << "Linker error in " << identifier << ":" << std::endl
        << std::string(log.data(), length) << std::endl;

    return false;

}

bool createShader(gl::GLenum type, const std::string & name, const std::string & source, gl::GLuint & id)
{
    id = gl::glCreateShader(type);

    const auto source_ptr = source.c_str();
    if (source_ptr)
        glShaderSource(id, 1, &source_ptr, 0);
    else
        return false;

    glCompileShader(id);

    return checkForCompilationError(id, name);
}

bool createProgram(const std::string & name, gl::GLuint vertexShader, gl::GLuint fragmentShader, gl::GLuint & id)
{
    id = glCreateProgram();

    gl::glAttachShader(id, vertexShader);
    gl::glAttachShader(id, fragmentShader);

    gl::glLinkProgram(id);

    return checkForLinkerError(id, name);
}

float mse(const std::vector<glm::vec4> & I, const std::vector<glm::vec4> & K, const float scale)
{
    assert(I.size() == K.size());

    // based on https://en.wikipedia.org/wiki/Peak_signal-to-noise_ratio

    auto sum = 0.f;
    for (auto i = 0; i < I.size(); ++i)
        sum += glm::dot(glm::pow((I[i] - K[i]) * scale, glm::vec4(2.f)), glm::vec4(1.f));

    sum /= I.size() * 4.f;

    return sum;
}

float psnr(const std::vector<glm::vec4> & I, const std::vector<glm::vec4> & K, const float scale)
{
    return 20.f * log10(scale) - 10.f * log10(mse(I, K, scale));
}

std::string humanFileSize(const size_t bytes, const int decimals)
{
    static const auto sz = "BKMGTP";
    const auto f = bytes > 0 ? static_cast<int>(floor((log10(bytes)) / 3)) : 0;

    std::stringstream ss;
    ss << std::fixed << std::setprecision(decimals)
        << (bytes > 0 ? static_cast<double>(bytes) / pow(1024.0, f) : 0)
        << sz[f] << (f > 0 ? "iB" : "");

    return ss.str();
}

std::string humanTimeDuration(const std::uint64_t nanoseconds, const int decimals)
{
    static const auto sz = "nums";
    const auto f = nanoseconds > 0 ? static_cast<int>(floor((log10(nanoseconds)) / 3)) : 0;

    std::stringstream ss;
    ss << std::fixed << std::setprecision(decimals)
        << (nanoseconds > 0 ? static_cast<double>(nanoseconds) / pow(1000.0, f) : 0)
        << sz[f] << (f > 0 ? "s" : "");

    return ss.str();
}

} // namespace cgutils
