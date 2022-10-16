#ifndef COMPRESS_H
#define COMPRESS_H

#include <iostream>
#include <string>
#include <filesystem>
#include <zlib.h>

unsigned long file_size(char* filename);
void decompress(std::string filename);
void compress(std::string filename);

#endif // COMPRESS_H