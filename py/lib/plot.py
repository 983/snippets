import tkinter as tk
from threading import Thread
import random, time, sys, os
import numpy as np

"""
mouse = None
mouse_old = None

def motion_event(e):
    global mouse
    global mouse_old
    
    mouse_old = mouse
    mouse = (e.x, e.y)

    if mouse and mouse_old:
        x0, y0 = mouse_old
        x1, y1 = mouse
        
        canvas.create_line(x0, y0, x1, y1)

window.bind('<Motion>', motion_event)

window.mainloop()
"""
LABEL_FONT = ('Arial', 10)
PAD = 5

class Plot(object):
    def __init__(self, width=512, height=512, title="Plot"):
        window = tk.Tk()
        window.title(title)
        #window.iconbitmap(r"C:\Users\owl\Anaconda3\DLLs\py.ico")
        
        canvas = tk.Canvas(
            window,
            width=width,
            height=height,
            highlightthickness=0)
        canvas.pack(fill=tk.BOTH, expand=tk.YES)

        self.window = window
        self.canvas = canvas

        def close_window(e):
            window.destroy()

        def on_resize(e):
            self.width = e.width
            self.height = e.height
            canvas.config(width=self.width, height=self.height)
        
        window.bind('<Escape>', close_window)
        canvas.bind('<Configure>', on_resize)

        self.width = width
        self.height = height

    def clear(self):
        self.window.update()
        self.canvas.delete("all")

    def plot_y(self, y, **kwargs):
        self.plot_ys([y], **kwargs)

    def plot_ys(self, ys, **kwargs):
        xs = [np.linspace(0, 1, len(y)) for y in ys]
        self.plot_xys(xs, ys, **kwargs)
    
    def plot_xys(self, xs, ys, x_lim=None, y_lim=None):
        canvas = self.canvas
        width = self.width
        height = self.height
        
        if x_lim is None:
            x0 = np.min(xs)
            x1 = np.max(xs)
        else:
            x0, x1 = x_lim

        if y_lim is None:
            y0 = np.min(ys)
            y1 = np.max(ys)
        else:
            y0, y1 = y_lim
        
        text = f"Bounds: (%.3f, %.3f)x(%.3f, %.3f)"%(x0, x1, y0, y1)

        canvas.create_text(
            PAD,
            self.height - PAD,
            text=text,
            font=LABEL_FONT,
            anchor=tk.SW)

        y = height - 1 - (0 - y0)/(y1 - y0)*height
        canvas.create_line(0, y, width, y, fill='black', stipple="gray50")

        colors = """
        #FF0303
        #0042FF
        #1CE6B9
        #540081
        #FFFC01
        #fEBA0E
        #20C000
        #E55BB0
        #959697
        #7EBFF1
        #106246
        #4E2A04
        """.split()
        
        for i, (x,y) in enumerate(zip(xs, ys)):
            color = colors[i % len(colors)]
            
            x = (x - x0)/(x1 - x0)
            y = (y - y0)/(y1 - y0)

            x *= width
            y *= height
            y = height - 1 - y

            xy = list(zip(x, y))

            canvas.create_line(*xy, fill=color)

    def write(self, text):        
        self.canvas.create_text(
            PAD,
            PAD,
            text=text,
            font=LABEL_FONT,
            anchor=tk.NW)

if __name__ == '__main__':
    p = Plot()

    while True:
        p.clear()
        time.sleep(0.016)

        x = np.linspace(0, 2*np.pi*5, 512, endpoint=False) + time.perf_counter()

        ys = []
        for offset in np.linspace(0, 2*np.pi, 20, endpoint=False):
            y = np.cos(x + offset)
            ys.append(y)

        p.plot_ys(ys, y_lim=[-2, 2])
