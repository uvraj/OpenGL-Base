# OpenGL-Base
A base for my (future) OpenGL-based projects. At the moment, it renders a simple triangle using ```glDrawElements()```. 
Classes for texture loading and shader loading are present (see /src/include/). The base is currently targeted towards OpenGL 3.3. It will most likely 
be targeted towards OpenGL 4.x in the future for debug groups and compute shaders.

![screenshot](https://raw.githubusercontent.com/uvraj/OpenGL-Base/b514fd0fd26bc6316fc407f57662b657f41e6000/doc/img1.png)

# Building
## Windows: 
Prerequisites: CMake 3.10, MinGW GCC or MSVC.
Example:

```mkdir build```

```cd build```

```cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release```

```make```

```cd bin && ./OpenGL-Base.exe```

## Linux:
...
