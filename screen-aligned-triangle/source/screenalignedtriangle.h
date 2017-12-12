#pragma once

#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols

#include <chrono>


// For more information on how to write C++ please adhere to: 
// http://cginternals.github.io/guidelines/cpp/index.html

class ScreenAlignedTriangle
{
public:
    enum class Mode : unsigned int {
        Quad                          = 0u,
        Triangle                      = 1u,
        Quad_Fill_Rectangle_Extension = 2u,
        Two_Triangles_Two_DrawCalls   = 3u,
        AVC_One_DrawCall              = 4u
    };

    static const std::array<std::string, 5> s_modeDescriptions;

public:
    ScreenAlignedTriangle();
    ~ScreenAlignedTriangle();

    void initialize();
    bool loadShaders();
    void resize(int w, int h);

    void render();
    void benchmarkAll();
    void reset();

    void switchDrawMode();
    void switchDrawMode(const Mode mode);

    void incrementReplaySpeed();
    void decrementReplaySpeed();

protected:
    void loadUniformLocations();

    void record();
    std::uint64_t record(bool benchmark);
    void replay();

    void updateThreshold();

protected:
    gl::GLuint m_fbo;
    gl::GLuint m_texture;
    int m_width;
    int m_height;

    Mode m_vaoMode;

    gl::GLuint m_VAO_screenAlignedTriangle,
               m_VAO_screenAlignedQuad,
               m_VAO_empty;

    gl::GLuint m_VBO_screenAlignedTriangle,
               m_VBO_screenAlignedQuad;

    gl::GLuint m_program_record,
               m_program_replay,
               m_program_record_AVC,
               m_program_benchmark,
               m_program_benchmark_AVC;

    gl::GLuint m_vertexShader_record,
               m_vertexShader_replay,
               m_vertexShader_record_AVC;

    gl::GLuint m_geometryShader_record_AVC;
    
    gl::GLuint m_fragmentShader_record,
               m_fragmentShader_replay,
               m_fragmentShader_minimal;

    gl::GLuint m_uniformLocation_indexThreshold;

    gl::GLuint m_acbuffer;   
    
    bool m_recorded;
    float m_startIndex;
    float m_thresholdIndex;

    gl::GLuint m_query;

    using msecs = std::chrono::duration<float, std::chrono::milliseconds::period>;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
    std::uint32_t m_timeDurationMagnitude;

    bool m_NV_extension_supported;
};
