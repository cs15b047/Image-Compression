import torch
import os

# Model
model = torch.hub.load('ultralytics/yolov5', 'yolov5s')  # or yolov5m, yolov5l, yolov5x, custom

base_dir1 = "/mnt/Work/Image-Compression/data/obj-detection-sample-images"
base_dir2 = "/mnt/Work/Image-Compression/benchmarking/results/clustering/reconstructed"

# or file, Path, PIL, OpenCV, numpy, list

for (img_path1, img_path2) in zip(sorted(os.listdir(base_dir1)), sorted(os.listdir(base_dir2))):
    img1 = os.path.join(base_dir1, img_path1)
    img2 = os.path.join(base_dir2, img_path2)
    results = model(img1)
    results.print()
    print("----------------------------------------")
    results = model(img2)
    results.print()
    print("========================================")