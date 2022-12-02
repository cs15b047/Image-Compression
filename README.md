# Stable Diffusion - Huggingface weights
## Run codec
python3 driver.py [path of image to be compressed] compressed/[name of compressed image file]

# HiFiC paper - part of Tensorflow compression
## Run codec
python3 tfci.py [mode = compress/decompress] [model=hific-lo/hific-hi]
Encode: python3 tfci.py compress hific-lo ../data/image_compression_benchmark/big_building.ppm_from_latents.png
Decode: python3 tfci.py decompress ../data/image_compression_benchmark/big_building.ppm_from_latents.png.tfci

# K-means clustering-based codec
## Run codec
python3 kmeans.py [path of image to be compressed] compressed/[name of compressed image file] reconstructed/[name of decoded image file] [number of clusters]

# Classical algorithms - JPEG/PNG
## Build codec
Compile: make all
Remove all executables: make clean
## Run codec
./mycodec [path of image to be compressed] compressed/[name of compressed image file] reconstructed/[name of decoded image file]
