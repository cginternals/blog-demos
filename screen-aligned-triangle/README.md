# Screen-aligned Quads and Triangles

[associated Blog Post](https://www.cginternals.com/en/blog/2018-01-10-screen-aligned-quads-and-triangles.html) on https://cginternals.com. Please note that this demo uses OpenGL 4.2 features and cannot run on Mac OS for now ([Mac computers that use OpenCL and OpenGL graphics](https://support.apple.com/en-us/HT202823)).

![Example Output](https://staging.cginternals.com/img/posts/blog/scrat-title.min.png)

## Build Instructions

##### Prerequisites and Dependencies

Building *screen-aligned-triangle* from source has the following mandatory dependencies:

* [CMake](https://cmake.org/) 3.0 or higher for building *glbinding* from source (mandatory for any build from source)
* [glbinding]((https://github.com/cginternals/glbinding)
* [glm](https://github.com/g-truc/glm)
* [glfw3](https://github.com/glfw/glfw)


##### Compile Instructions

For compilation, a C++11 compliant compiler, e.g., GCC 4.8, Clang 3.3, MSVC 2013 **Update 3**, is required.
First, download the source code as archive or via git:

```bash
> git clone https://github.com/cginternals/blog-demos.git
> cd blog-demos/screen-aligned-triangle
```

The actual compilation can be done using CMake and your favorite compiler and IDE. For building *screen-aligned-triangle* CMake via command line can be used (should work on all systems):

First, create a build directory (we do not recommend in-source builds):

```bash
> mkdir build
> cd build
```

Make sure CMake will be able to find glbinding, glfw3, and glm directories by either using `set CMAKE_PREFIX_PATH=<GLBINDING_DIR>;<GLM_DIR>;<GLFW_DIR>;` or `$env.CMAKE_PREFIX_PATH="<GLBINDING_DIR>;<GLM_DIR>;<GLFW_DIR>;"` (Power Shell on Windows) or use cmake-gui to setup these paths manually during configuration.

Configure *screen-aligned-triangle* with your preferred or default generator, e.g., for Visual Studio 2017 in x64 use
(note: some IDEs have integrated support for CMake projects, e.g., Qt Creator, and allow you to skip the manual project configuration):

```bash
> cmake .. -G "Visual Studio 15 2017 Win64"
```

In order to compile the project, either use you favorite Editor/IDE with the created project or use CMake as follows:

```bash
> cmake --build .
```

##### Run Instructions

For running the demo make sure that glbinding and glfw3 binaries can be located and that the working directory contains the `data` folder with all shaders (which can be changed and reloaded at run-time).
