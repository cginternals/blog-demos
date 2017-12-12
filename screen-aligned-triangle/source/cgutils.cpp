
#include "cgutils.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols

using namespace gl;

namespace cgutils
{


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
