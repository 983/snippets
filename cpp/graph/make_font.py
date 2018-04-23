from PIL import Image, ImageDraw, ImageFont
import numpy as np

fontsize = 128

font = ImageFont.truetype("Gidole-Regular.ttf", fontsize)

images = []

shapes = np.zeros((256, 2), dtype=np.int32)

for i in range(256):
    try:
        text = chr(i)
        image_size = font.getsize(text)

        image = Image.new("RGBA", image_size, (0,0,0,0))
        draw = ImageDraw.Draw(image)
        draw.text((0, 0), text, (255,255,255,255), font=font)

        width, height = image_size

        width = width//4
        height = height//4

        shapes[i, 0] = width
        shapes[i, 1] = height

        image = image.resize((width, height), Image.ANTIALIAS)

        images.append(np.array(image))
    except Exception as e:
        print(i, e)
        images.append(None)

dx = np.max(shapes[:, 0])
dy = np.max(shapes[:, 1])

result = np.zeros((dy*16, dx*16, 4), dtype=np.uint8)
for y in range(16):
    for x in range(16):
        image = images[y*16 + x]
        
        if image is not None:
            
            y0 = y*dy
            y1 = y0 + image.shape[0]
            
            x0 = x*dx
            x1 = x0 + image.shape[1]

            result[y0:y1, x0:x1] = image

image = Image.fromarray(result)
image.save("font.png")

image = np.array(image)

with open("font", "wb") as f:
    f.write(image.tobytes())

with open("font_info", "wb") as f:
    f.write(np.int32([
        image.shape[1],
        image.shape[0],
        dx,
        dy,
    ]).tobytes())
    f.write(shapes.tobytes())
