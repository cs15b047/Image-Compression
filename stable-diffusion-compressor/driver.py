import os
import shutil
import time
from tqdm import tqdm

from codec import *
from util import *
from codecs_util import compress_input

rescaled_folder = input_folder + "rescaled/"

if not os.path.isdir(rescaled_folder):
    os.mkdir(rescaled_folder) 
print('rescaling images to 512x512')
for i, filename in tqdm(enumerate(os.listdir(input_folder))):
    f_in = os.path.join(input_folder, filename)
    f_out = os.path.join(rescaled_folder, os.path.splitext(filename)[0] + ".png")
    if os.path.isfile(f_in) and not os.path.isfile(f_out):
        try:
            resize_to_512(f_in, f_out)
        except:
            print("skipping {} because the file could not be opened.".format(filename))

if os.path.isdir(output_folder):
    shutil.rmtree(output_folder)
os.mkdir(output_folder)
for filename in os.listdir(rescaled_folder):
    f = os.path.join(rescaled_folder, filename)
    if os.path.isfile(f):
        compress_input(f, os.path.splitext(os.path.join(output_folder, filename))[0])
        time.sleep(0.1) # sleep so execution can be interrupted