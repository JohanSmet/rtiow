# Ray Tracing In One Weekend

Yet another C++ implementation of the book [Ray Tracing In One Weekend](https://raytracing.github.io/) by Peter Shirley.

Or, more accurately, ray tracing in a spare hour here and there. Seemed like a fun thing to do when Advent of Code 2021 wrapped up. And besides, a ray tracer, just like a 6502 emulator, is something you've got write at least once in your lifetime, right?

Nothing really special here, I followed the book quite closely but I tried to avoid using inheritance hierarchies and virtual functions. I don't have multiple material classes, just one that has parameters to control the amount of diffuse/specular reflection and refraction (inspired by articles on [The blog at the bottom of the sea](https://blog.demofox.org/2020/06/14/casual-shadertoy-path-tracing-3-fresnel-rough-refraction-absorption-orbit-camera/)).

Uses [glslSmartDeNoise](https://github.com/BrutPitt/glslSmartDeNoise) to do a denoise post-processing step on the rendered image.

## Future Goals
- Reimplement the ray tracer to run entirely on the GPU (vulkan, glsl)

## Building
Developed and tested primarely on x64 Linux but should work fine on Windows (and maybe MacOS?).
Dependencies are either included directly or as a git submodule. If you have CMake and can build OpenGL programs you should be good to go.
