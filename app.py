from flask import Flask, render_template, request
from clustering import kmeans
from hific import tfci
import sys
sys.path.append('./stableDiff')
from stableDiff import codec
from stableDiff import codecs_util

import stableDiff
import subprocess

import os
app = Flask(__name__)

@app.route('/')
def home():
    return render_template("upload.html")

@app.route('/upload', methods=['POST'])
def upload():
    if request.method == 'POST':
        file = request.files['file']
        file.save(os.path.join("uploads", file.filename))
        return render_template("upload.html", upload_msg="Uploaded successfully")       


@app.route('/compress', methods=['POST'])
def compress():
    if request.method == 'POST':
        file = request.files['file']
        file.save(os.path.join("uploads", file.filename))

    print("compress")
    print(request.form)
    # absolute path to uploads folder
    uploads = os.path.join(os.getcwd(), "uploads")
    # absolute path to compressed folder
    compressed = os.path.join(os.getcwd(), "compressed")
    if "kmeans" in request.form:
        print("kmeans")
        try:
            kmeans.encode_image(os.path.join(uploads, file.filename), 100, os.path.join(compressed, file.filename))
        except:
            return render_template("upload.html", upload_msg="Compression failed")

    if "hific" in request.form:
        try:
            print("hific")
            tfci.compress("hific-lo", os.path.join(uploads, file.filename), os.path.join(compressed, file.filename))
            # tfci.compress("hific-lo", os.path.join("../uploads", file.filename), os.path.join("../compressed", file.filename))
            print("finished compressing") 
        except:
            return render_template("upload.html", upload_msg="Compression failed")

    if "stable-diffusion" in request.form:
        try:
            print("stable diffusion")
            # import stableDiff codecs_util in a correct way to execute the compress input function

            codecs_util.compress_input(os.path.join(uploads, file.filename), os.path.join(compressed, file.filename))
            # codecs_util.compress_input(os.path.join(uploads, file.filename), os.path.join(compressed, file.filename))
        except:
            return render_template("upload.html", upload_msg="Compression failed")
    if "png" in request.form:
        try:
            print("png")
            reconstructed = os.path.join(os.getcwd(), "reconstructed")
            pngCode = os.path.join(os.getcwd(), "png/mycodec")
            subprocess.run([pngCode, os.path.join(uploads, file.filename), os.path.join(compressed, file.filename), os.path.join(reconstructed, file.filename)])
        except:
            return render_template("upload.html", upload_msg="Compression failed")
    print("finished compressing") 
    return render_template("upload.html", compress_msg="Compressed successfully")


if __name__ == '__main__':
    app.run(debug=True)