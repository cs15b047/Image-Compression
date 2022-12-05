from math import log10, sqrt
import cv2
import numpy as np
import sys
from skimage.metrics import structural_similarity as ssim
  
def PSNR(original, compressed):
    mse = np.mean((original - compressed) ** 2)
    if(mse == 0):  # MSE is zero means no noise is present in the signal .
                  # Therefore PSNR have no importance.
        return 100
    max_pixel = 255.0
    psnr = 20 * log10(max_pixel / sqrt(mse))
    return psnr
  
def main(original_image_path, compressed_image_path):
    original = cv2.imread(original_image_path)
    compressed = cv2.imread(compressed_image_path, 1)
    print(type(original), type(compressed), np.shape(original), np.shape(compressed))
    value = PSNR(original, compressed)
    ssim_value = ssim(original, compressed, channel_axis=2, multichannel=True)
    print(f"PSNR value is {value} dB")
    print(f"SSIM value is {ssim_value}")
       
if __name__ == "__main__":
    original_image_path = sys.argv[1]
    compressed_image_path = sys.argv[2]
    main(original_image_path, compressed_image_path)