#ifndef MYCODEC_H
#define MYCODEC_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::string compress_image(std::vector<uint8_t>& image, int width, int height);
std::vector<uint8_t> decompress_image(std::string filename);

#endif // MYCODEC_H