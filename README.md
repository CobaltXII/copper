# Copper

Copper is a naive pathtracer written in C++.

# Dependencies

Copper uses the following libraries: stb_image_write (Sean T. Barrett), inih (Ben Hoyt), and tinyobjloader (Syoyo Fujita). They are slow to compile, so they are linked at compile time. To compile these libraries, please run the associated shell scripts that can be found alongside the source code.

Copper also uses GLM for mathematics. GLM is a large (header only) library, and is not included in this repository. You will need to download GLM and add it to the default include path, if you have not done so already.

# Compilation

Use the build script.

```bash
./copper.sh
```

# Usage

To use Copper, simply run the executable. The usage message of Copper is as follows.

```
Usage: ./copper <path> <samples>
```

You may set the path argument to the path of any of the included scene files, or the path of any scene file you create yourself. The scene file must be of INI syntax. No formal documentation is given for the scene file's structure, so you will have to infer by looking at the structure of the sample scene files.

# Credits

Thanks to Sean T. Barrett, Ben Hoyt, Syoyo Fujita, Ben Kyd, and UglySwedishFisk.