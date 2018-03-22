# Raytracer C++ framework for Introduction to Computer Graphics

This is a C++ framework for a raytracer. It is created for the Computer
Science course "Introduction to Computer Graphics" taught at the
University of Groningen.

This framework is inspired by and uses (some) code of the raytracer framework of
Bert Freudenberg which unfortunately is no longer available.

## Compiling the code

We provided a [CMakeLists.txt](CMakeLists.txt) file to be used with `cmake`.
This can be used in the following way (on the command line/shell):
```
# cd into the main directory of the framework
mkdir build   # create a new build directory
cd build      # cd into the directory
cmake ..      # creates a Makefile
# Comipiling the code can then be done with:
make
# or
make -j4      # replacing 4 with the number of cores of your pc
```
**Note!** After adding new `.cpp` files (when adding new shapes)
`cmake ..` needs to be called again or you might get linker errors.

## Running the Raytracer
After compilation you should have the `ray` executable.
This can be used like this:
```
./ray <path to .json file> [output .png file]
# when in the build directory:
./ray ../Scenes/scene01.json
```
Specifying an output is optional and by default an image will be created in
the same directory as the source scene file with the `.json` extension replaced
by `.png`.

## Description of the included files

### Scene files
* `Scenes/*.json`: Scene files are structured in JSON. If you have never
    worked with json, please see [here](https://en.wikipedia.org/wiki/JSON#Data_types,_syntax_and_example)
    or [here](https://www.json.org/).

    Take a look at the provided example scenes for the general structure.
    You are free (and encouraged) to define your own scene files later on.

### The raytracer source files (Code directory)

* `main.cpp`: Contains main(), starting point. Responsible for parsing
    command-line arguments.

* `raytracer.cpp/.h`: Raytracer class. Responsible for reading the scene
    description, starting the raytracer and writing the result to an image file.

* `scene.cpp/.h`: Scene class. Contains code for the actual raytracing.

* `image.cpp/.h`: Image class, includes code for reading from and writing to PNG
    files.

* `light.h`: Light class. Plain Old Data (POD) class. Colored light at a
    position in the scene.

* `ray.h`: Ray class. POD class. Ray from an origin point in a direction.

* `hit.h`: Hit class. POD class. Intersection between an `Ray` and an `Object`.

* `object.h`: virtual `Object` class. Represents an object in the scene.
    All your shapes should derive from this class. See

* `shapes (directory/folder)`: Folder containing all your shapes.

* `sphere.cpp/.h (inside shapes)`: Sphere class, which is a subclass of the
    `Object` class. Represents a sphere in the scene.

* `example.cpp/.h (inside shapes)`: Example shape class. Copy these two files
    and replace/rename **every** instance of `Example` `example.h` or `EXAMPLE`
    with your new shape name.

* `triple.cpp/.h`: Triple class. Represents a 3-dimensional vector which is
    used for colors, points and vectors.
    Includes a number of useful functions and operators, see the comments in
    `triple.h`.
    Classes of `Color`, `Vector`, `Point` are all aliases of `Triple`.

* `objloader.cpp/.h`: Is a similar class to Model used in the OpenGL
    exercises to load .obj model files. It produces a std::vector
    of Vertex structs. See `vertex.h` on how you can retrieve the
    coordinates and other data defined at vertices.

### Supporting source files (Code directory)

* `lode/*`: Code for reading from and writing to PNG files,
    used by the `Image` class.
    lodepng is created by Lode Vandevenne and can be found on
    [github](https://github.com/lvandeve/lodepng).
* `json/*`: Code for parsing JSON documents.
    Created by Niels Lohmann and available under the MIT license on
    [github](https://github.com/nlohmann/json).
    **Recommended:** Especially take a look at their README for more
    info on how to work with json files.


### Additions

We added support for the following basic shapes: Sphere, Triangle, and Plane.
We also added support for meshes (.obj files), these are read by the OBJLoader class. In this class the model is resized and translated to fit inside a unit cube located at (0, 0, 0). The user can specify the desired position and scale of the model by using `position` and `scale` attributes in their scene's JSON file. The .obj files are rendered using triangle basic shapes.
For lighting we used full phong shading. The ambient, diffuse and specular illumination variables can be set per object in their scene's JSON files.

We added support for a ResolutionMultiplier variable in the config. This determines the size of the output image multiplier by 400. A ResolutionMultiplier of 1 results in a 400x400 image, a ResolutionMultiplier of 2 in an 800x800 image. We also added a AspectRatio variable to the config. This is the ratio of x/y. To get a 1920x1080 image, use ResolutionMultiplier 2.7 and AspectRatio 1.777778 (16/9).
Other scene variables added to the config are: Shadows (enabled / disabled (default=disabled)), MaxRecursionDepth (how deep recursion goes for reflections (default=0)) and SuperSamplingFactor (how many samples taken per pixel (default=1)).

We also added a rotation vector, texture file path, shadows and angle variables to objects, which determine how the textures are displayed. Note that while all objects accept these variables, only spheres use them correctly. The same goes for textures: Only spheres use them correctly. Disabling shadows for an object means that this object won't cast shadows on other objects.

Rotations are done using Rodrigues' rotation formula (https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula) using the axis and angle set in the JSON file along with a hit point and its normal. In our case k is the rotation and N is the normal. Note that zero vectors are not allowed! If there is a zero vector in the JSON, the program uses (1.0, 1.0, 1.0) with a 0 degrees angle instead, thus not rotating at all.