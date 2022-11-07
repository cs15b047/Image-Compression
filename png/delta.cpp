#include "mycodec.h"
#include <vector>

using namespace std;

template <typename T>
vector<T> delta_encode(vector<T>& image) {
    int image_size = (image.size()) / 3;
    vector<vector<T>> channels = separate_channels<T>(image, image_size);
    vector<vector<T>> encoded_channels(3, vector<T>(image_size));
    
    for(int i = 0; i < 3; i++) {
        encoded_channels[i][0] = channels[i][0];
        for(int j = 1; j < image_size; j++) {
            encoded_channels[i][j] = (T)(channels[i][j] - channels[i][j - 1]);
        }
    }
    vector<T> encoded_image = merge_channels<T>(encoded_channels, image_size);
    return encoded_image;
}

template <typename T>
vector<T> delta_decode(vector<T>& encoded_image) {
    int image_size = (encoded_image.size()) / 3;
    vector<vector<T>> encoded_channels = separate_channels<T>(encoded_image, image_size);
    vector<vector<T>> channels(3, vector<T>(image_size));

    for(int i = 0; i < 3; i++) {
        channels[i][0] = encoded_channels[i][0];
        for(int j = 1; j < image_size; j++) {
            channels[i][j] = (T)(encoded_channels[i][j] + channels[i][j - 1]);
        }
    }
    
    vector<T> decoded_image = merge_channels<T>(channels, image_size);

    return decoded_image;
}


template vector<uint8_t> delta_encode(vector<uint8_t>& image);
template vector<short> delta_encode(vector<short>& image);
template vector<int> delta_encode(vector<int>& image);
template vector<double> delta_encode(vector<double>& image);

template vector<uint8_t> delta_decode(vector<uint8_t>& encoded_image);
template vector<short> delta_decode(vector<short>& image);
template vector<int> delta_decode(vector<int>& encoded_image);
template vector<double> delta_decode(vector<double>& encoded_image);