from math import log10, sqrt
import cv2
import numpy as np
import sys
from skimage.metrics import structural_similarity as ssim

import os
import sys
from tqdm import tqdm
 
def PSNR(original, compressed):
    mse = np.mean((original - compressed) ** 2)
    if(mse == 0):  # MSE is zero means no noise is present in the signal .
                  # Therefore PSNR have no importance.
        return 100
    max_pixel = 255.0
    psnr = 20 * log10(max_pixel / sqrt(mse))
    return psnr
  
def evaluate_compression_quality(original_image_path, compressed_image_path):
    original = cv2.imread(original_image_path)
    compressed = cv2.imread(compressed_image_path, 1)
    psnr_value = PSNR(original, compressed)
    ssim_value = ssim(original, compressed, channel_axis=2)
    return [psnr_value, ssim_value]


def summarize_results(method):
    original_images_dir = os.path.join(base_dir, "data/c10n_benchmark")
    compressed_images_dir = os.path.join(base_dir, "benchmarking/results", method, "compressed")
    reconstructed_images_dir = os.path.join(base_dir, "benchmarking/results", method, "reconstructed")
    metrics_file = os.path.join(base_dir, "benchmarking/results", method, "metrics.txt")
    results_file = os.path.join(base_dir, "benchmarking/results", method, "results.txt")

    original_images_size = 0
    compressed_images_size = 0


    image_names, PSNR_val, SSIM_val = [], [], []
    for i, image in tqdm(enumerate(os.listdir(original_images_dir))):
        file_ext = image.split(".")[-1]
        if file_ext != "png" and file_ext != "ppm":
            continue

        image_name = image.split(".")[0]
        original_image_path = os.path.join(original_images_dir, image)
        reconstructed_image_path = os.path.join(reconstructed_images_dir, image_name + ".png")
        compressed_image_path = os.path.join(compressed_images_dir, image_name + ".z")
        
        image_names += [image_name]
        # Measure PSNR and SSIM
        [psnr_val, ssim_val] = evaluate_compression_quality(original_image_path, reconstructed_image_path)
        PSNR_val += [psnr_val]
        SSIM_val += [ssim_val]

        # Measure size of original image
        original_images_size += os.path.getsize(original_image_path)
        compressed_images_size += os.path.getsize(compressed_image_path)

    compression_ratio = original_images_size / compressed_images_size
    compressed_images_size = compressed_images_size / pow(2, 20) # MB

    num_images = len(image_names)
    PSNR_val, SSIM_val = np.array(PSNR_val), np.array(SSIM_val)

    with open(metrics_file, "w") as f:
        f.write(f"PSNR: {np.mean(PSNR_val):.2f} +/- {np.std(PSNR_val):.2f}\n")
        f.write(f"SSIM: {np.mean(SSIM_val):.4f} +/- {np.std(SSIM_val):.4f}\n")
        f.write(f"Compressed images size: {compressed_images_size:.2f} MB\n")
        f.write(f"Compression ratio: {compression_ratio:.2f}\n")
    
    with open(results_file, "w") as f:
        f.write("Image,PSNR,SSIM\n")
        for i in range(num_images):
            f.write(f"{image_names[i]},{PSNR_val[i]:.2f},{SSIM_val[i]:.4f}\n")

if __name__ == "__main__":
    base_dir="/mnt/Image-Compression"

    codecs = ["mycodec", "clustering", "hific", "stable_diffusion"]
    for codec in codecs:
        print(f"Summarizing results for {codec}")
        summarize_results(codec)
    