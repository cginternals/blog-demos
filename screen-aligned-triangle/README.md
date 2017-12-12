# Screen-aligned Quads and Triangles

[associated Blog Post](https://staging.cginternals.com/en/blog/2017-11-14-screen-aligned-quads-and-triangles.html) on https://cginternals.com. Please note that this demo uses OpenGL 4.2 features and cannot run on Mac OS for now ([Mac computers that use OpenCL and OpenGL graphics](https://support.apple.com/en-us/HT202823)).

## Build Instructions

##### Prerequisites and Dependencies

Building *screen-aligned-triangle* from source has two mandatory dependencies:

* [CMake](https://cmake.org/) 3.0 or higher for building *glbinding* from source (mandatory for any build from source)
* [glfw3](https://github.com/glfw/glfw)
* [glm](https://github.com/g-truc/glm)


##### Compile Instructions

For compilation, a C++11 compliant compiler, e.g., GCC 4.8, Clang 3.3, MSVC 2013 **Update 3**, is required.
First, download the source code as archive or via git:

```bash
> git clone https://github.com/cginternals/blog-demos.git
> cd blog-demos
```

The actual compilation can be done using CMake and your favorite compiler and IDE. For building *screen-aligned-triangle* CMake via command line can be used (should work on all systems):

First, create a build directory (we do not recommend in-source builds):

```bash
> mkdir build
> cd build
```

Configure *screen-aligned-triangle* with your preferred or default generator, e.g., for Visual Studio 2017 in x64 use
(note: some IDEs have integrated support for CMake projects, e.g., Qt Creator, and allow you to skip the manual project configuration):

```bash
> cmake .. -G "Visual Studio 15 2017 Win64"
```

In order to compile the project, either use you favorite Editor/IDE with the created project or use CMake as follows:

```bash
> cmake --build .
```
