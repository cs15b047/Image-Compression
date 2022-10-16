#ifndef RLE_H
#define RLE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void rle_encode(std::vector<uint8_t> image, int width, int height, std::string filename);
std::vector<uint8_t> rle_decode(std::string filename);

void write_encoded_image(std::vector<std::pair<uint8_t, short int>>& encoded_image, int width, int height, std::string filename);

#endif // RLE_H