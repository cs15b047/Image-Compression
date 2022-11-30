import os
import sys
import shutil
import time
from tqdm import tqdm

from codec import *
from util import *
from codecs_util import compress_input
import time

if __name__ == "__main__":
    input_filepath = sys.argv[1]
    output_filepath = sys.argv[2]

    if not os.path.isfile(input_filepath):
        print("Input file does not exist")
        sys.exit(1)

    print("Start image resize")
    img = resize_to_512(input_filepath)
    print("Image resize complete")

    print("Starting compression...")
    start_compress = time.time()
    compress_input(img, output_filepath)
    time.sleep(0.1) # sleep so execution can be interrupted
    end_compress = time.time()
    print("Compression took {} seconds".format(end_compress - start_compress))