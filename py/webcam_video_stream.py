import numpy as np
import cv2
import os

def main():
    cam = cv2.VideoCapture(2)
    
    try:
        while True:
            ok, frame = cam.read()
            
            cv2.imshow("frame", frame)
            
            print(frame.shape)
            
            key = cv2.waitKey(1)
                   
            #cv2.imwrite(f"frames/{card_counter}/{frame_counter}.jpg", frame)
            
            if key == ord('q') or key == 27:
                print("quit")
                break
    
    except Exception as e:
        print(e)

    cam.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
