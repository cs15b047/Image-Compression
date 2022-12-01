import argparse
import io
import os
import sys
import urllib
from absl import app
from absl.flags import argparse_flags
import numpy as np
from PIL import Image
import scipy as sc
from scipy.cluster.vq import kmeans, vq
import pickle
import zlib
import faiss


def encode_image(image_path, num_clusters, compressed_path):
    img = Image.open(image_path)
    # img = img.convert('YCbCr')

    original_shape = img.size
    dims = original_shape
    img = img.resize(dims)
    img = np.asarray(img)

    points = img.reshape((-1, 3))
    points = np.float32(points)

    print(points.shape)

    kmeans = faiss.Kmeans(points.shape[1], num_clusters)
    kmeans.train(points)
    centroids = kmeans.centroids
    distance, code = kmeans.index.search(points, 1)
    
    print(np.average(distance))

    code, centroids = np.uint16(code.reshape(-1)), np.uint8(centroids.reshape(-1))
    print(code.shape, centroids.shape)

    code, centroids = zlib.compress(code), zlib.compress(centroids)
    print(len(code), len(centroids))
    compressed_data = [code, centroids, original_shape, img.shape]

    with open(compressed_path, 'wb') as f:
        pickle.dump(compressed_data, f)

def decode_image(compressed_path, output_path):
    with open(compressed_path, 'rb') as f:
        compressed_data = pickle.load(f)
    code, centroids, original_shape, image_shape = compressed_data
    code, centroids = zlib.decompress(code), zlib.decompress(centroids)
    code = np.frombuffer(code, dtype=np.uint16)
    centroids = np.frombuffer(centroids, dtype=np.uint8).reshape(-1, 3)

    points = centroids[code]
    img = points.reshape(image_shape)
    img = np.uint8(img)
    img = Image.fromarray(img, 'RGB')
    img = img.resize(original_shape)
    # img = img.convert('RGB')
    img.save(output_path)


if __name__ == '__main__':
    image_path = sys.argv[1]
    compressed_path = sys.argv[2]
    reconstructed_img_path = sys.argv[3]
    num_clusters = int(sys.argv[4])

    encode_image(image_path, num_clusters, compressed_path)
    decode_image(compressed_path, reconstructed_img_path)
