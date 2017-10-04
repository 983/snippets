from ctypes import *
import cv2
import time, sys, os
import numpy as np
import matplotlib.pyplot as plt
import scipy.misc

dll = cdll.LoadLibrary("my_ps3eye.dll")

dll.start_camera()

get_frame = dll.get_frame
get_frame.restype = c_void_p

free_frame = dll.free_frame
free_frame.argtypes = [c_void_p]

t = time.clock()
counter = 0

# createBackgroundSubtractorKNN
fgbg = cv2.createBackgroundSubtractorMOG2()
fgbg = cv2.createBackgroundSubtractorKNN()

try:
    os.mkdir("frames")
except Exception:
    pass

while True:
    frames = []
    while True:
        frame = get_frame()
        if not frame: break
        frames.append(frame)

    if len(frames) > 0:
        ptr = cast(frames[-1], POINTER(c_ubyte))
        image = np.ctypeslib.as_array(ptr, shape=(480, 640, 3))

        R = image[:, :, 0]
        G = image[:, :, 1]
        B = image[:, :, 2]
        
        dt = time.clock() - t
        if dt > 1.0:
            path = "frames/%d.bmp"%counter
            counter += 1
            print(path)
            scipy.misc.imsave(path, image)
            t = time.clock()
        
        image = np.array([B, G, R]).transpose([1, 2, 0])

        #fgmask = fgbg.apply(image)

        #cv2.imshow("image", fgmask)
        cv2.imshow("image", image)
    
        key = cv2.waitKey(1)
        if key == 27:
            break

    for frame in frames:
        free_frame(frame)

cv2.destroyAllWindows()
