sudo apt update
sudo apt install software-properties-common -y
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt install python3.9
sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.9 1
sudo apt install python3-pip python3.9-distutils
pip3 install diffusers accelerate libimagequant mozjpeg-lossless-optimization scikit-image tqdm tensorflow-compression huggingface-hub Pillow transformers ftfy -t /mnt/
pip3 install torch torchvision torchaudio --extra-index-url https://download.pytorch.org/whl/cpu -t /mnt/