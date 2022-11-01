#include "mycodec.h"

using namespace std;

vector<uint8_t> rgb2ycbcr(vector<uint8_t>& image) {
    vector<uint8_t> ycbcr_image;
    double red_total = 0, green_total = 0, blue_total = 0;
    ycbcr_image.clear();
    for(int i = 0; i < image.size(); i += 3) {
        double r = image[i], g = image[i+1], b = image[i+2];
        red_total += r;
        green_total += g;
        blue_total += b;
        
        double y = 0.299 * r + 0.587 * g + 0.114 * b;
        double cb = -0.169 * r - 0.331 * g + 0.5 * b + 128;
        double cr = 0.5 * r - 0.419 * g - 0.081 * b + 128;
        y = min(max(y, 0.0), 255.0);
        cb = min(max(cb, 0.0), 255.0);
        cr = min(max(cr, 0.0), 255.0);

        // uint8_t y_int = (uint8_t) (y * 219 + 16);
        // uint8_t cb_int = (uint8_t) (cb * 224 + 128);
        // uint8_t cr_int = (uint8_t) (cr * 224 + 128);

        ycbcr_image.push_back((uint8_t)y);
        ycbcr_image.push_back((uint8_t)cb);
        ycbcr_image.push_back((uint8_t)cr);
    }

    cout << "Red average: " << red_total / (image.size() / 3) << endl;
    cout << "Green average: " << green_total / (image.size() / 3) << endl;
    cout << "Blue average: " << blue_total / (image.size() / 3) << endl;


    return ycbcr_image;
}

vector<uint8_t> ycbcr2rgb(vector<uint8_t>& image) {
    vector<uint8_t> rgb_image;
    rgb_image.clear();
    for(int i = 0; i < image.size(); i += 3) {
        double y = (double)image[i], cb = (double)image[i+1], cr = (double)image[i+2];
        // y = (y - 16) / 219.0;
        cb = (cb - 128);
        cr = (cr - 128);

        double r = y + 1.402 * cr;
        double g = y - 0.344136 * cb - 0.714136 * cr;
        double b = y + 1.773 * cb ;

        // uint8_t r_int = min((int) (r * 255), 255);
        // uint8_t g_int = min((int) (g * 255), 255);
        // uint8_t b_int = min((int) (b * 255), 255);

        r = min(max(r, 0.0), 255.0);
        g = min(max(g, 0.0), 255.0);
        b = min(max(b, 0.0), 255.0);

        rgb_image.push_back((uint8_t)r);
        rgb_image.push_back((uint8_t)g);
        rgb_image.push_back((uint8_t)b);
    }

    return rgb_image;
}