# OpenGL-Base
A base for my (future) OpenGL-based projects. At the moment, it renders a simple triangle using ```glDrawElements()```. 
Classes for texture loading and shader loading are present.

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
