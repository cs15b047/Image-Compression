BASE_DIR=/mnt/Image-Compression/
DATA_DIR=$BASE_DIR/data
RESULTS_DIR=$BASE_DIR/benchmarking/results

MYCODEC_DIR=$BASE_DIR/png
CLUSTERING_DIR=$BASE_DIR/clustering
DIFFUSION_DIR=$BASE_DIR/stable-diffusion-compressor
HIFIC_DIR=$BASE_DIR/hific

CLIC_DATA_DIR=$DATA_DIR/clic
C10N_DATA_DIR=$DATA_DIR/c10n_benchmark

mkdir -p $RESULTS_DIR/mycodec/compressed
mkdir -p $RESULTS_DIR/mycodec/reconstructed
mkdir -p $RESULTS_DIR/clustering/compressed
mkdir -p $RESULTS_DIR/clustering/reconstructed
mkdir -p $RESULTS_DIR/stable_diffusion/compressed
mkdir -p $RESULTS_DIR/stable_diffusion/reconstructed
mkdir -p $RESULTS_DIR/hific/compressed
mkdir -p $RESULTS_DIR/hific/reconstructed


# Run custom codec on C10N benchmark dataset
for i in $C10N_DATA_DIR/*.ppm; do
    img_name=$(basename -s .ppm $i)
    src_img_path=$i

    compressed_img_path=$RESULTS_DIR/mycodec/compressed/$img_name.z
    reconstructed_img_path=$RESULTS_DIR/mycodec/reconstructed/$img_name.png

    $MYCODEC_DIR/mycodec $src_img_path $compressed_img_path $reconstructed_img_path
done


# Run k-means clustering codec on C10N benchmark dataset
K=200
for i in $C10N_DATA_DIR/*.ppm; do
    img_name=$(basename -s .ppm $i)
    src_img_path=$i

    compressed_img_path=$RESULTS_DIR/clustering/compressed/$img_name.z
    reconstructed_img_path=$RESULTS_DIR/clustering/reconstructed/$img_name.png

    python3 $CLUSTERING_DIR/kmeans.py $src_img_path $compressed_img_path $reconstructed_img_path $K
done


# Run stable diffusion codec on C10N benchmark dataset
for i in $C10N_DATA_DIR/*.ppm; do
    img_name=$(basename -s .ppm $i)
    src_img_path=$i

    compressed_img_path=$RESULTS_DIR/stable_diffusion/compressed/$img_name.z
    reconstructed_img_path=$RESULTS_DIR/stable_diffusion/reconstructed/$img_name.png

    python3 $DIFFUSION_DIR/driver.py $src_img_path $compressed_img_path $reconstructed_img_path
done

# Run HiFiC codec on C10N benchmark dataset
model="hific-lo"
for i in $C10N_DATA_DIR/*.ppm; do
    img_name=$(basename -s .ppm $i)
    src_img_path=$i

    compressed_img_path=$RESULTS_DIR/hific/compressed/$img_name.tfci
    reconstructed_img_path=$RESULTS_DIR/hific/reconstructed/$img_name.png

    python3 $HIFIC_DIR/tfci.py compress $model $src_img_path $compressed_img_path
    python3 $HIFIC_DIR/tfci.py decompress $compressed_img_path $reconstructed_img_path
done