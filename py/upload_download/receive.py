from flask import Flask, request, Response
import os

MIN_FILE_SIZE = 100 # bytes

os.makedirs("files", exist_ok=True)

app = Flask(__name__)

def get_ext(data):
    if data.startswith(bytes([0x52, 0x49, 0x46, 0x46])): return "webp"
    if data.startswith(bytes([0x47, 0x49, 0x46, 0x38])): return"gif"
    if data.startswith(bytes([0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A])): return "png"
    if data.startswith(bytes([0xFF, 0xD8, 0xFF])): return"jpg"
    if data.startswith(bytes([0x3c, 0x73, 0x76, 0x67])): return "svg"
    return None

@app.route("/", methods=["GET", "HEAD", "OPTIONS", "POST"])
def home():
    data = request.data
    if len(data) >= MIN_FILE_SIZE:
        ext = get_ext(data)

        if ext is not None:
            path = f"files/{os.urandom(16).hex()}.{ext}"

            with open(path, "wb") as f:
                f.write(data)

            print(f"Wrote {len(data)} bytes to {path}")

    resp = Response()
    resp.headers['Access-Control-Allow-Origin'] = '*'
    resp.headers['Access-Control-Allow-Headers'] = 'content-type'
    return resp

app.run(debug=True)
