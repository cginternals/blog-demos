
#include "screenalignedtriangle.h"

#include <cmath>
#include <cstring>
#include <iostream>

#include <glbinding/gl32ext/gl.h>
#include <glbinding-aux/Meta.h>
#include <glbinding-aux/ContextInfo.h>

#include "cgutils.h"


using namespace gl32core;


const std::array<std::string, 5> ScreenAlignedTriangle::s_modeDescriptions = std::array<std::string, 5>{
    "quad (triangle strip)                    ",
    "triangle                                 ",
    "fill rectangle extension                 ",
    "quad (two triangles, two draw calls)     ",
    "triangle (attributed vertex cloud, AVC)  " };


ScreenAlignedTriangle::ScreenAlignedTriangle()
: m_recorded(false)
, m_vaoMode(Mode::Quad)
, m_timeDurationMagnitude(2u)
, m_NV_extension_supported(true)
{
}

ScreenAlignedTriangle::~ScreenAlignedTriangle()
{
    glDeleteBuffers(1, &m_VBO_screenAlignedTriangle);
    glDeleteBuffers(1, &m_VBO_screenAlignedQuad);

    glDeleteVertexArrays(1, &m_VAO_screenAlignedTriangle);
    glDeleteVertexArrays(1, &m_VAO_screenAlignedQuad);
    glDeleteVertexArrays(1, &m_VAO_empty);

    glDeleteProgram(m_program_record);
    glDeleteProgram(m_program_replay);
    glDeleteProgram(m_program_record_AVC);
    glDeleteProgram(m_program_benchmark);
    glDeleteProgram(m_program_benchmark_AVC);

    glDeleteShader(m_vertexShader_record);
    glDeleteShader(m_vertexShader_replay);
    glDeleteShader(m_vertexShader_record_AVC);

    glDeleteShader(m_geometryShader_record_AVC);

    glDeleteShader(m_fragmentShader_record);
    glDeleteShader(m_fragmentShader_replay);
    glDeleteShader(m_fragmentShader_minimal);
    
    glDeleteFramebuffers(1, &m_fbo);

    glDeleteTextures(1, &m_texture);

    glDeleteBuffers(1, &m_acbuffer);
    glDeleteQueries(1, &m_query);
}

void ScreenAlignedTriangle::initialize()
{
    glClearColor(0.12f, 0.14f, 0.18f, 1.0f);

    glGenBuffers(1, &m_VBO_screenAlignedTriangle);
    glGenBuffers(1, &m_VBO_screenAlignedQuad);

    static const float verticesScreenAlignedTriangle[] = { -1.f, -1.f, -1.f, 3.f, 3.f, -1.f };
    static const float verticesScrAQ[] = { -1.f, -1.f, -1.f, 1.f, 1.f, -1.f, 1.f, 1.f };

    glGenVertexArrays(1, &m_VAO_screenAlignedTriangle);
    glGenVertexArrays(1, &m_VAO_screenAlignedQuad);
    glGenVertexArrays(1, &m_VAO_empty);

    glBindVertexArray(m_VAO_screenAlignedTriangle);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_screenAlignedTriangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sizeof(verticesScreenAlignedTriangle), verticesScreenAlignedTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
    glBindVertexArray(0);

    glBindVertexArray(m_VAO_screenAlignedQuad);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_screenAlignedQuad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sizeof(verticesScrAQ), verticesScrAQ, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
    glBindVertexArray(0);

    glBindVertexArray(m_VAO_empty);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create resources for record program

    m_program_record = glCreateProgram();
    m_vertexShader_record = glCreateShader(GL_VERTEX_SHADER);
    m_fragmentShader_record = glCreateShader(GL_FRAGMENT_SHADER);

    glAttachShader(m_program_record, m_vertexShader_record);
    glAttachShader(m_program_record, m_fragmentShader_record);

    glBindFragDataLocation(m_program_record, 0, "out_color");


    //create resources for replay program

    m_program_replay = glCreateProgram();
    m_vertexShader_replay = glCreateShader(GL_VERTEX_SHADER);
    m_fragmentShader_replay = glCreateShader(GL_FRAGMENT_SHADER);

    glAttachShader(m_program_replay, m_vertexShader_replay);
    glAttachShader(m_program_replay, m_fragmentShader_replay);

    glBindFragDataLocation(m_program_replay, 0, "out_color");
    
    //create resources for record AVC program

    m_program_record_AVC = glCreateProgram();
    m_vertexShader_record_AVC = glCreateShader(GL_VERTEX_SHADER);
    m_geometryShader_record_AVC = glCreateShader(GL_GEOMETRY_SHADER);

    glAttachShader(m_program_record_AVC, m_vertexShader_record_AVC);
    glAttachShader(m_program_record_AVC, m_geometryShader_record_AVC);
    glAttachShader(m_program_record_AVC, m_fragmentShader_record);

    glBindFragDataLocation(m_program_record_AVC, 0, "out_color");

    // create resources for benchmark program

    m_program_benchmark = glCreateProgram();
    m_fragmentShader_minimal = glCreateShader(GL_FRAGMENT_SHADER);

    glAttachShader(m_program_benchmark, m_vertexShader_record);
    glAttachShader(m_program_benchmark, m_fragmentShader_minimal);

    glBindFragDataLocation(m_program_benchmark, 0, "out_color");

    //create resources for benchmark AVC program

    m_program_benchmark_AVC = glCreateProgram();

    glAttachShader(m_program_benchmark_AVC, m_vertexShader_record_AVC);
    glAttachShader(m_program_benchmark_AVC, m_geometryShader_record_AVC);
    glAttachShader(m_program_benchmark_AVC, m_fragmentShader_minimal);

    glBindFragDataLocation(m_program_benchmark_AVC, 0, "out_color");

    loadShaders();

    // Fragment Index Render Target

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_R32F), m_width, m_height, 0, GL_RED, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_NEAREST));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_NEAREST));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));

    // create and configure framebuffer

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0);

    static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // create and configure atomic counter

    glGenBuffers(1, &m_acbuffer);
    glBindBuffer(gl32ext::GL_ATOMIC_COUNTER_BUFFER, m_acbuffer);
    glBufferData(gl32ext::GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(gl32ext::GL_ATOMIC_COUNTER_BUFFER, 0);

    // setup time measurement

    glGenQueries(1, &m_query);

    // test whether the NV_fill_rectangle extension is known and supported. If not, warn user
    const auto extension = glbinding::aux::Meta::getExtension("GL_NV_fill_rectangle");

    if (extension == gl::GLextension::UNKNOWN) 
    {
        std::cout << "The NV_fill_rectangle extension is unknown on your system." << std::endl << "Draw mode 3 will not work properly." << std::endl << std::endl;
        m_NV_extension_supported = false;
        return;
    }

    const std::set<gl::GLextension> requiredExtensions = {extension};
    if ( !glbinding::aux::ContextInfo::supported(requiredExtensions) )
    {
        std::cout << "Your graphics card does not support the NV_fill_rectangle extension." << std::endl << "Draw mode 3 will not work properly." << std::endl << std::endl;
        m_NV_extension_supported = false;
    }
}

namespace 
{

// extract method
bool loadShader(const std::string & sourceFile, const gl::GLuint & shader)
{
    const auto shaderSource = cgutils::textFromFile(sourceFile.c_str());
    const auto shaderSource_ptr = shaderSource.c_str();
    if (shaderSource_ptr)
        glShaderSource(shader, 1, &shaderSource_ptr, 0);

    glCompileShader(shader);
    return cgutils::checkForCompilationError(shader, sourceFile);
}

}

bool ScreenAlignedTriangle::loadShaders()
{
    static const auto sourceFiles = std::array<std::string, 7>{
        "data/record.vert",
        "data/record-empty.vert",
        "data/record.geom",
        "data/record.frag",
        "data/benchmark-minimal.frag",
        "data/replay.vert",
        "data/replay.frag"  };

    // setup shaders of record program 

    bool success = loadShader(sourceFiles[0], m_vertexShader_record);
    success &= loadShader(sourceFiles[3], m_fragmentShader_record);
    if (!success)
        return false;

    gl::glLinkProgram(m_program_record);

    success &= cgutils::checkForLinkerError(m_program_record, "record program");
    if (!success)
        return false;

    // setup shaders of replay program

    success = loadShader(sourceFiles[5], m_vertexShader_replay);
    success &= loadShader(sourceFiles[6], m_fragmentShader_replay);
    if (!success)
        return false;

    gl::glLinkProgram(m_program_replay);

    success &= cgutils::checkForLinkerError(m_program_replay, "replay program");
    if (!success)
        return false;
 
    // setup shaders of record AVC program

    success = loadShader(sourceFiles[1], m_vertexShader_record_AVC);
    success &= loadShader(sourceFiles[2], m_geometryShader_record_AVC);
    if (!success)
        return false;

    gl::glLinkProgram(m_program_record_AVC);

    success &= cgutils::checkForLinkerError(m_program_record_AVC, "record AVC program");
    if (!success)
        return false;

    // setup shaders of benchmark program 

    success = loadShader(sourceFiles[4], m_fragmentShader_minimal);
    if (!success)
        return false;

    gl::glLinkProgram(m_program_benchmark);

    success &= cgutils::checkForLinkerError(m_program_record, "record benchmark program");
    if (!success)
        return false;

    // setup shaders of record AVC benchmark program

    gl::glLinkProgram(m_program_benchmark_AVC);

    success &= cgutils::checkForLinkerError(m_program_benchmark_AVC, "record benchmark AVC program");
    if (!success)
        return false;

    loadUniformLocations();

    return true;
}

void ScreenAlignedTriangle::loadUniformLocations()
{
    m_uniformLocation_indexThreshold = glGetUniformLocation(m_program_replay, "threshold");

    const gl::GLuint uniformLocation_indexSampler = glGetUniformLocation(m_program_replay, "fragmentIndex");
    
    glUseProgram(m_program_replay);
    glUniform1i(uniformLocation_indexSampler, 0);
    glUseProgram(0);
}

void ScreenAlignedTriangle::resize(int w, int h)
{
    m_width = w;
    m_height = h;

    m_recorded = false;

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_R32F), m_width, m_height, 0, GL_RED, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glViewport(0, 0, m_width, m_height);
}

void ScreenAlignedTriangle::render()
{
    if (!m_recorded)
    {
        std::cout << "  draw mode (" << static_cast<unsigned int>(m_vaoMode) + 1u << ") - " << s_modeDescriptions[static_cast<unsigned int>(m_vaoMode)];
        
        if (m_vaoMode == Mode::Quad_Fill_Rectangle_Extension && !m_NV_extension_supported)
            std::cout << "   since NV_fill_rectangle extension is not supported this result may not be representative for draw mode 3";
        std::cout << std::endl;

        record();
        m_recorded = true;

        m_startIndex = 0;
        m_startTime = std::chrono::high_resolution_clock::now();
    }

    replay();
    updateThreshold();
}

void ScreenAlignedTriangle::record()
{
    // reset and bind atomic counter
    const auto counter = 0u;
    glBindBuffer(gl32ext::GL_ATOMIC_COUNTER_BUFFER, m_acbuffer);
    glBufferSubData(gl32ext::GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &counter);
    glBindBuffer(gl32ext::GL_ATOMIC_COUNTER_BUFFER, 0);

    glBindBufferBase(gl32ext::GL_ATOMIC_COUNTER_BUFFER, 0, m_acbuffer);

    record(false);

    // unbind atomic counter and read value
    glBindBufferBase(gl32ext::GL_ATOMIC_COUNTER_BUFFER, 0, 0);
    glBindBuffer(gl32ext::GL_ATOMIC_COUNTER_BUFFER, m_acbuffer);

    auto data = 0u;
    gl32ext::glMemoryBarrier(gl32ext::GL_ATOMIC_COUNTER_BARRIER_BIT);
    glGetBufferSubData(gl32ext::GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &data);

    glBindBuffer(gl32ext::GL_ATOMIC_COUNTER_BUFFER, 0);
}

std::uint64_t ScreenAlignedTriangle::record(const bool benchmark)
{
    // clear record buffer

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClear(GL_COLOR_BUFFER_BIT);

    static const GLfloat color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearBufferfv(GL_COLOR, 0, color);

    if (benchmark)
        glUseProgram(m_vaoMode == Mode::AVC_One_DrawCall ? m_program_benchmark_AVC : m_program_benchmark );
    else
        glUseProgram(m_vaoMode == Mode::AVC_One_DrawCall ? m_program_record_AVC : m_program_record);

    // draw

    auto elapsed = std::uint64_t{ 0 };
    if (benchmark)
        glBeginQuery(gl::GL_TIME_ELAPSED, m_query);

    switch (m_vaoMode)
    {
    case Mode::Two_Triangles_Two_DrawCalls:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(m_VAO_screenAlignedQuad);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_TRIANGLES, 1, 3);
        break;
    case Mode::Quad:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(m_VAO_screenAlignedQuad);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        break;
    case Mode::Triangle:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(m_VAO_screenAlignedTriangle);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        break;
    case Mode::Quad_Fill_Rectangle_Extension:
        glPolygonMode(GL_FRONT_AND_BACK, gl::GL_FILL_RECTANGLE_NV);
        glBindVertexArray(m_VAO_screenAlignedQuad);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        break;
    case Mode::AVC_One_DrawCall:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(m_VAO_empty);
        glDrawArrays(GL_POINTS, 0, 1);
        break;
    }

    if (benchmark)
    {
        glEndQuery(gl::GL_TIME_ELAPSED);

        auto done = 0;
        while (!done)
            glGetQueryObjectiv(m_query, GL_QUERY_RESULT_AVAILABLE, &done);

        glGetQueryObjectui64v(m_query, GL_QUERY_RESULT, &elapsed);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return elapsed;
}

void ScreenAlignedTriangle::replay()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glUseProgram(m_program_replay);
    glUniform1f(m_uniformLocation_indexThreshold, m_thresholdIndex);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(m_VAO_screenAlignedTriangle);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glUseProgram(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ScreenAlignedTriangle::benchmarkAll()
{
    static const unsigned int warmUpIterations = 150000u;
    static const unsigned int iterations = 10000u;

    static const auto modes = std::array<Mode, 5>{
        Mode::Quad, 
        Mode::Triangle,
        Mode::Quad_Fill_Rectangle_Extension,
        Mode::Two_Triangles_Two_DrawCalls, 
        Mode::AVC_One_DrawCall};

    const Mode currentMode = m_vaoMode;

    std::cout << std::endl << "Measure Performance" << std::endl 
              << "  warming up ..." << std::endl;

    for (auto i = 0u; i < warmUpIterations; ++i)
        record(true);

    std::cout << "  benchmarking ... " << std::endl << std::endl;

    for( const auto mode : modes)
    {
        m_vaoMode = mode;
        auto elapsed = std::uint64_t{ 0 };

        for (auto i = 0u; i < iterations; ++i)
            elapsed += record(true);

        std::cout << "  (" << static_cast<unsigned int>(mode) + 1 << ") " << s_modeDescriptions[static_cast<unsigned int>(mode)] << cgutils::humanTimeDuration(elapsed / iterations);
        if(mode == Mode::Quad_Fill_Rectangle_Extension && !m_NV_extension_supported)
            std::cout << "   since NV_fill_rectangle extension is not supported this result may not be representative for draw mode 3";
        std::cout << std::endl;
    }

    std::cout << std::endl << std::endl;

    m_vaoMode = currentMode;
    m_recorded = false;
}

void ScreenAlignedTriangle::reset()
{
    m_recorded = false;
}

void ScreenAlignedTriangle::updateThreshold()
{
	const auto msecsSinceStart = msecs(std::chrono::high_resolution_clock::now() - m_startTime).count();
    m_thresholdIndex = m_startIndex  
		+ powf(10.f, static_cast<float>(m_timeDurationMagnitude)) * msecsSinceStart;
}

void ScreenAlignedTriangle::switchDrawMode()
{
    switchDrawMode(static_cast<Mode>((static_cast<unsigned int>(m_vaoMode) + 1u) % 5u));
}

void ScreenAlignedTriangle::switchDrawMode(const Mode mode)
{
    m_recorded = false;
    m_vaoMode = mode;
}

void ScreenAlignedTriangle::incrementReplaySpeed()
{
    updateThreshold();
    
    m_startIndex = m_thresholdIndex;
    m_startTime = std::chrono::high_resolution_clock::now();

    ++m_timeDurationMagnitude;
}

void ScreenAlignedTriangle::decrementReplaySpeed()
{
    if (m_timeDurationMagnitude == 0)
        return;

    updateThreshold();

    m_startIndex = m_thresholdIndex;
    m_startTime = std::chrono::high_resolution_clock::now();

    --m_timeDurationMagnitude;
}
