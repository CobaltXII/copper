clang++ copper.cpp stb/stb_image_write.o inih/ini.o tinyobjloader/tiny_obj_loader.o -o copper -std=c++11 -O3 -march=native -Wall && ./copper ini/CornellBox-Dragon.ini 1