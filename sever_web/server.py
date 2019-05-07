from werkzeug.utils import secure_filename
from flask import Flask, render_template, jsonify, request, make_response, send_from_directory, abort
import os
from demo.face_apk.laugh_detection import laugh_detection

app = Flask(__name__)
UPLOAD_FOLDER = 'upload'
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
basedir = os.path.abspath(os.path.dirname(__file__))
ALLOWED_EXTENSIONS = set(['png', 'jpg', 'JPG', 'PNG', 'bmp', 'BMP'])  # 'gif', 'GIF',


def allowed_file(filename):
    return '.' in filename and filename.rsplit('.', 1)[1] in ALLOWED_EXTENSIONS


@app.route('/upload')
def upload_test():
    return render_template('receive_pic.html')


# 上传文件
@app.route('/upload_pic', methods=['POST'], strict_slashes=False)
def api_upload():
    file_dir = os.path.join(basedir, app.config['UPLOAD_FOLDER'])
    keys_dir = os.path.join(basedir, 'keys')
    # print(file_dir)
    if not os.path.exists(file_dir):
        os.makedirs(file_dir)

    if 'photo' in request.files:
        f = request.files['photo']
    else:
        f = False
    iv_name = request.form["iv_name"]
    iv_file_name = os.path.join(keys_dir, iv_name + '.txt')
    # print(request.files)
    # print(f.filename)
    if f and allowed_file(f.filename) and iv_name:
        f_name = secure_filename(f.filename)
        # print(f_name)
        if len(f_name.rsplit('.', 1))>1:
            ext = f_name.rsplit('.', 1)[1]
        else:
            ext = f_name.rsplit('.', 1)[0]
        # print(ext)
        new_filename = iv_name + '.' + ext
        pic_name = os.path.join(file_dir, new_filename)
        f.save(pic_name)
        
        whether_laugh = laugh_detection(pic_name)
        if whether_laugh:
            if os.path.exists(iv_file_name):
                with open(iv_file_name, 'r',encoding='UTF-8') as f:
                    iv_info = f.read()
                    #print(iv_info)
                    f.close()
            else:
                return "Please confirm the correctness of the input of iv.txt"
        else:
            return "Please re-upload the picture meets our requirements(laugh)"

        return "Your key is: " + iv_info
    else:
        return "Uploading failed: Please ensure that you have entered the content in iv.txt and upload the picture meets the requirement"


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)
