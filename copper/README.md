# Copper

![Alt text](https://github.com/CobaltXII/copper/blob/master/img/copper_lucy_3072.png?raw=true)

## Summary

Copper is a pathtracer that was made by recycling the source code of my raytracer, Orb. It supports raytracing spheres, ellipsoids, cones, capsules, cylinders and planes. It does not support explicit light sampling (yet!). It supports lambert, mirror and emissive materials.

## Libraries

I make heavy use of Sean T. Barrett's libraries. They add a significant amount to compilation time, so I compile them once and then link them for evermore. To compile them, use the following commands.

### stb_image

```bash
clang -D STB_IMAGE_IMPLEMENTATION -c stb/stb_image.c -o lib/stb_image.o -O3
```

### stb_image_write

```bash
clang -D STB_IMAGE_WRITE_IMPLEMENTATION -c stb/stb_image_write.c -o lib/stb_image_write.o -O3
```

### stb_perlin

```bash
clang -D STB_PERLIN_IMPLEMENTATION -c stb/stb_perlin.c -o lib/stb_perlin.o -O3
```

I also use Ben Hoyt's INI parsing library for loading portable scene files. The library adds a small delay to compilation time, so I compile it once and then link it. To compile it, use the following command.

### inih

```bash
clang -c inih/ini.c -o inih/ini.o -O3
```

I also use Syoyo Fujita's Wavefront OBJ parsing library for loading portable model files. The library adds a large delay to compilation time, so I compile it once and then link it. To compile it, use the following command.

### tinyobjloader

```bash
clang -c tinyobjloader/tiny_obj_loader.cc -o tinyobjloader/tiny_obj_loader.o -O3
```

## Compiling

Just link the libraries and compile as default. I was too lazy to write a Makefile.

```bash
./copper.sh
```

## Scenes

There are a bunch of scene files included in this repository that you can look at to see how they work. Every primitive is mentioned at least once, so it should be easy to tweak a few parameters and see what they do. There is no formal documentation, so you will have to take a peek at the code if you get stumped.

## Credits

Thanks to Inigo Quilez for his great articles and intersector functions. Thanks to Cyrille Favreau for his ellipsoid intersection function. Thanks to Brook Heisler for a great tutorial on getting started with raytracing. Thanks to Ben Hoyt for his great INI library. Thanks to Jonas Wagner for a great pathtracer reference implementation. Without you guys, this project would never have happened, so thanks again!