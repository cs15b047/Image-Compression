#include "mycodec.h"
#include <vector>

using namespace std;

template <typename T>
vector<T> delta_encode(vector<T>& image) {
    vector<T> encoded_image;
    encoded_image.push_back(image[0]);
    for (int i = 1; i < image.size(); i++) {
        T delta = (T)(image[i] - image[i-1]);
        encoded_image.push_back(delta);
    }
    return encoded_image;
}

template <typename T>
vector<T> delta_decode(vector<T>& encoded_image) {
    vector<T> decoded_image;
    decoded_image.push_back(encoded_image[0]);
    for (int i = 1; i < encoded_image.size(); i++) {
        T delta = (T)(encoded_image[i] + decoded_image[i-1]);
        decoded_image.push_back(delta);
    }
    return decoded_image;
}


template vector<int> delta_encode(vector<int>& image);
template vector<double> delta_encode(vector<double>& image);

template vector<int> delta_decode(vector<int>& encoded_image);
template vector<double> delta_decode(vector<double>& encoded_image);