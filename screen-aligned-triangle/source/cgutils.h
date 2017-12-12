#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include <glbinding/gl/types.h>

namespace cgutils
{

// This function is reads the contents of the back left buffer and 
// stores it into a PPM formated image file http://netpbm.sourceforge.net/doc/ppm.html
void captureAsPPM(const char * filePath, unsigned int width, unsigned int height);

// Read raw binary file into a char vector (probably the fastest way).
std::vector<char> rawFromFile(const char * filePath);

std::string textFromFile(const char * filePath);

bool checkForCompilationError(gl::GLuint shader, const std::string & identifier);
bool checkForLinkerError(gl::GLuint program, const std::string & identifier);

std::string humanTimeDuration(const std::uint64_t nanoseconds, const int decimals = 2);

} // namespace cgutils
