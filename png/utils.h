#ifndef UTILS_H
#define UTILS_H

#include <vector>

void calculate_roundtrip_error(std::vector<uint8_t>& image, std::vector<uint8_t>& image_restored, int image_size);
void calc_histogram(std::vector<uint8_t>& image);

template <typename T>
std::vector<T> merge_channels(std::vector<std::vector<T> >& image, int image_size);
template <typename T>
std::vector<std::vector<T> > separate_channels(std::vector<T>& image_, int image_size);

double clip(double pix);

#endif // UTILS_H