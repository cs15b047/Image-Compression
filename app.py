from flask import Flask, render_template, request
# from clustering import kmeans
from hific import tfci

import os
import json


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
    # if "kmeans" in request.form:
    #     print("kmeans")
    #     try:
    #         kmeans.encode_image(os.path.join("uploads", file.filename), int(request.form["kmeans"]), os.path.join("compressed", file.filename))
    #     except:
    #         return render_template("upload.html", upload_msg="Compression failed")

    if "hific" in request.form:
        try:
            tfci.encode_image(os.path.join("uploads", file.filename), int(request.form["hific"]), os.path.join("compressed", file.filename))   
        except:
            return render_template("upload.html", upload_msg="Compression failed")
        return render_template("upload.html", compress_msg="Compressed successfully") 


if __name__ == '__main__':
    app.run(debug=True)