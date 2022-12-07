#include "mycodec.h"

using namespace std;

vector<uint8_t> rgb2ycbcr(vector<uint8_t>& image) {
    vector<uint8_t> ycbcr_image;
    ycbcr_image.resize(image.size());
    #pragma omp parallel for
    for(int i = 0; i < image.size(); i += 3) {
        double r = image[i], g = image[i+1], b = image[i+2];
        
        double y = 0.299 * r + 0.587 * g + 0.114 * b;
        double cb = -0.169 * r - 0.331 * g + 0.5 * b + 128;
        double cr = 0.5 * r - 0.419 * g - 0.081 * b + 128;
        y = clip(y);
        cb = clip(cb);
        cr = clip(cr);

        ycbcr_image[i] = (uint8_t)y;
        ycbcr_image[i + 1] = (uint8_t)cb;
        ycbcr_image[i + 2] = (uint8_t)cr;
    }

    return ycbcr_image;
}

vector<uint8_t> ycbcr2rgb(vector<uint8_t>& image) {
    vector<uint8_t> rgb_image;
    rgb_image.resize(image.size());
    #pragma omp parallel for
    for(int i = 0; i < image.size(); i += 3) {
        double y = (double)image[i], cb = (double)image[i+1], cr = (double)image[i+2];
        // y = (y - 16) / 219.0;
        cb = (cb - 128);
        cr = (cr - 128);

        double r = y + 1.402 * cr;
        double g = y - 0.344136 * cb - 0.714136 * cr;
        double b = y + 1.773 * cb ;

        r = clip(r);
        g = clip(g);
        b = clip(b);

        rgb_image[i] = (uint8_t)r;
        rgb_image[i + 1] = (uint8_t)g;
        rgb_image[i + 2] = (uint8_t)b;
    }

    return rgb_image;
}