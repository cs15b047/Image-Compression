#ifndef COMPRESS_H
#define COMPRESS_H

#include <iostream>
#include <string>
#include <experimental/filesystem>
#include <zlib.h>
#include <vector>

unsigned long file_size(char* filename);
void decompress(std::string filename);
void compress(std::string filename);

void compress(std::vector<char> buffer, std::string filename);
std::vector<char> decompress1(std::string filename);

#endif // COMPRESS_H