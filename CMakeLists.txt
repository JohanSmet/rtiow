cmake_minimum_required(VERSION 3.12)

project(rtiow)

# force C++20 for all targets
set(CMAKE_CXX_STANDARD 20)

# export a JSON compilation database for clangd
set (CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# platform detection (preprocessor define)
string(TOUPPER ${CMAKE_SYSTEM_NAME} PLATFORM_NAME)
string(CONCAT PLATFORM_DEF "PLATFORM_" ${PLATFORM_NAME})

# macro to turn on compiler warnings (as errors) for a specific target
macro(target_compile_warning target)
	target_compile_options(${target} PRIVATE
		$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
		-Wall -Wextra -Wconversion -Wsign-conversion -Werror>
		$<$<CXX_COMPILER_ID:MSVC>:
			/W4
			/wd"4200" /wd"4201" /wd"4204"	# disable specific non-standard extension warnings
			>)
endmacro()

# external library - glad
set (GLAD_TARGET glad)
add_library (${GLAD_TARGET} STATIC)
target_sources(${GLAD_TARGET} PRIVATE
	libs/glad/glad.c
	libs/glad/glad.h
	libs/glad/khrplatform.h
)

# external library - glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw)

# external library - OpenGL
find_package(OpenGL REQUIRED)

# static library for the raytracer
set (LIB_TARGET raytrace)
add_library(${LIB_TARGET} STATIC)
target_sources(${LIB_TARGET} PRIVATE
	src/raytrace/camera.cpp
	src/raytrace/camera.h
	src/raytrace/config.h
	src/raytrace/geometry_base.h
	src/raytrace/geometry_spheres.cpp
	src/raytrace/geometry_spheres.h
	src/raytrace/glm.h
	src/raytrace/ray.h
	src/raytrace/raytrace.cpp
	src/raytrace/raytrace.h
	src/raytrace/rgb_buffer.h
	src/raytrace/scene.cpp
	src/raytrace/scene.h
	src/raytrace/thread_pool.h
	src/raytrace/types.h
	src/raytrace/utils.h
)

target_include_directories(${LIB_TARGET} PRIVATE libs)
target_include_directories(${LIB_TARGET} PUBLIC libs/glm)
target_include_directories(${LIB_TARGET} PUBLIC src)
target_compile_definitions(${LIB_TARGET} PUBLIC ${PLATFORM_DEF})
target_compile_warning(${LIB_TARGET})

# front-end executable
set (FRONTEND_TARGET rtiow_gl)
add_executable(${FRONTEND_TARGET})
target_sources(${FRONTEND_TARGET} PRIVATE
	libs/argh/argh.h

	src/frontend/main.cpp
	src/frontend/output_opengl.cpp
	src/frontend/output_opengl.h
)
target_include_directories(${FRONTEND_TARGET} PRIVATE libs)
target_link_libraries(${FRONTEND_TARGET} PRIVATE ${LIB_TARGET})
target_link_libraries(${FRONTEND_TARGET} PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(${FRONTEND_TARGET} PRIVATE ${GLAD_TARGET})
target_link_libraries(${FRONTEND_TARGET} PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(${FRONTEND_TARGET} PRIVATE glfw)
target_compile_warning(${FRONTEND_TARGET})
