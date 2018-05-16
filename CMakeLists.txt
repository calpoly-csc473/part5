cmake_minimum_required(VERSION 2.8)

project(part3)

if(WIN32)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(PARSER
	lib/parser/Objects.hpp
	lib/parser/parse_error.hpp
	lib/parser/Parser.cpp
	lib/parser/Parser.hpp
	lib/parser/Tokenizer.cpp
	lib/parser/Tokenizer.hpp
	lib/parser/TokenStream.cpp
	lib/parser/TokenStream.hpp
)

set(SOURCES
	src/Application.cpp
	src/BlinnPhongBRDF.cpp
	src/Camera.cpp
	src/CookTorranceBRDF.cpp
	src/main.cpp
	src/Object.cpp
	src/Pixel.cpp
	src/Plane.cpp
	src/RayInfo.cpp
	src/RayTracer.cpp
	src/Renderer.cpp
	src/Scene.cpp
	src/SceneInfo.cpp
	src/Sphere.cpp
	src/Triangle.cpp
	src/Util.cpp
)

set(HEADERS
	src/Application.hpp
	src/BlinnPhongBRDF.hpp
	src/BRDF.hpp
	src/Camera.hpp
	src/CookTorranceBRDF.hpp
	src/Light.hpp
	src/Object.hpp
	src/Params.hpp
	src/Pixel.hpp
	src/PixelContext.hpp
	src/Plane.hpp
	src/Ray.hpp
	src/RayInfo.hpp
	src/RayTraceResults.hpp
	src/RayTracer.hpp
	src/Renderer.hpp
	src/Scene.hpp
	src/SceneInfo.hpp
	src/Sphere.hpp
	src/Triangle.hpp
	src/Util.hpp
)

add_library(parser ${PARSER})

add_executable(raytrace ${SOURCES} ${HEADERS})
target_include_directories(raytrace PUBLIC "ext/")
target_include_directories(raytrace PUBLIC "lib/")
target_link_libraries(raytrace parser)

find_package(Threads)
target_link_libraries(raytrace ${CMAKE_THREAD_LIBS_INIT})
