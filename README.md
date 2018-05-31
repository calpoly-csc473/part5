
# CSC 473 Part 5 Implementation

[![Build Status](https://travis-ci.org/calpoly-csc473/part5.svg?branch=master)](https://travis-ci.org/calpoly-csc473/part5)

This is an implementation of [Part 5](https://iondune.github.io/csc473/project/part5) of the CSC 473 raytracer project.
You have the option of using this as the basis for your Part 6 through Part 7 projects in this class.

Due to the flexible nature of deadlines in this class,
it's important to note that this code may not be used in any
submissions for part 1, 2, 3, 4, or 5 of the raytracer project.
You can look at this project, but for the purposes of those first three parts,
this code is the same as any other source on the internet - it can be read and learned from,
but not copy-and-pasted.


## Documentation

- [`ext/`](ext/) contains external libraries
- [`ext/stb/`](ext/stb/) contains the `stb_image_write.h` utility from the [stb](https://github.com/nothings/stb) library
- [`lib/`](lib/) contains the parsing library
- [`lib/parser/`](lib/parser/) is a slightly modified version of the [released parsing library](https://github.com/calpoly-csc473/parser) for this class
- [`src/`](src/) contains all source code and headers, organized into folders.

The source code of this project is organized into folders by general topic.

- [`Application/`](src/Application/) contains high-level modules such as [int main()](src/Application/main.cpp),
  a class for [parsing commandline arguments and determining which action to execute](src/Application/Application.hpp),
  and a few classes for executing particular categories of commands:
  - [Render commands](src/Application/Renderer.hpp)
  - [Single-ray trace commands](src/Application/RayInfo.hpp)
  - [Scene description commands](src/Application/SceneInfo.hpp)
- [`Objects/`](src/Objects/) contains implementations of each shape supported by the raytracer.
  These shapes implement the [Object](src/Scene/Object.hpp) interface found in the [`Scene/`](src/Scene/) folder.
  - [Plane](src/Objects/Plane.hpp)
  - [Sphere](src/Objects/Sphere.hpp)
  - [Triangle](src/Objects/Triangle.hpp)
- [`RayTracer/`](src/RayTracer/) contains the bulk of the project implementation.
  - [RayTracer](src/RayTracer/RayTracer.hpp) implements most of the functionality related to casting rays and shading.
  - A few basic structs are included here as well:
    - [Ray](src/RayTracer/Ray.hpp) represents a single ray with some basic utilities
    - [Pixel](src/RayTracer/Pixel.hpp) represents a simple pixel color with a utility to convert and clamp floating point color values
    - [RayTraceResults](src/RayTracer/RayTraceResults.hpp) stores the individual contributions (e.g. diffuse, specular, reflection, etc.) from a single ray trace.
      These are stored separately primarily for diagnostic purposes.
    - [PixelContext](src/RayTracer/PixelContext.hpp) stores diagnostic information about every action involved in a single ray trace,
      for the purposes of implementing commands like `printrays` and `pixeltrace`.
- [`Scene/`](src/Scene/) contains the fundamental parts of a scene description.
  - [Scene](src/Scene/Scene.hpp) stores a list of objects, lights, and a camera with utilities for querying ray intersection.
  - [Camera](src/Scene/Camera.hpp) represents the scene camera and generates rays for a given pixel.
  - [Light](src/Scene/Light.hpp) simply contains the position and color of a light source.
  - [Object](src/Scene/Object.hpp) provides the interface for all intersectable scene objects,
    and is implemented by the classes in the [`Objects/`](src/Objects/) folder.
- [`Shading/`](src/Shading/) contains two different BRDF implementations,
  [Blinn-Phong](src/Shading/BlinnPhongBRDF.cpp) and [Cook-Torrance](src/Shading/CookTorranceBRDF.cpp),
  as well as a general [BRDF](src/Shading/BRDF.hpp) interface used by the [RayTracer](src/RayTracer/RayTracer.hpp) to compute shading.


## Building

The only dependency not included is [GLM](https://glm.g-truc.net/).
I recommend installing GLM using your operating system's package manager.

- On Windows, I recommend [vcpkg](https://github.com/Microsoft/vcpkg)
- On Mac OS, [Brew](http://brewformulas.org/Glm)
- On Linux, `apt`, `yum`, etc.

Both CMake and Visual Studio build files are provided.
CMake can of course be used to generate Visual Studio build files.
The `.sln` and `.vcxproj` are provided for convenience.


## FAQ

**Q:** Why did you put copyright notices and a license on this software?
Don't you want use to be free to use this code in our projects?

**A:** Yes I do - I put a permissive open-source license on this code to
*increase* your rights to use this software, not *decrease* them.
Any time you write software, you automatically own the copyright on that software - no registration is necessary.
So if I distributed this code without any notices or licenses attached,
I would still own the copyright, and in using the code you would technically be commiting copyright infringement.
For the purposes of this class you are almost uncertainly protected by **Fair Use**,
but if you decided to sell your raytracer commerically at some later date,
that would definitely be copyright infringement.
Obviously, I am not likely to sue any of my former students...

However, if the software is licensed to you under a permissive open-source license,
you are legally free to use the software in whatever capacity you desire (including commercially).

For more information, give this a read: [https://opensource.org/faq](https://opensource.org/faq)

---

**Q:** You must like the word `const`.

[Yes, I do.](https://isocpp.org/wiki/faq/const-correctness)

