import numpy as np
import matplotlib.pyplot as plt
import scipy.misc
import time, sys

def apply_per_channel(func):
    def func2(image, *args):
        if len(image.shape) == 2:
            return func(image, *args)

        return np.stack([
            func(channel, *args) for channel in image.transpose([2, 0, 1])], axis=2)
    
    return func2

@apply_per_channel
def pad_edges(image, pad):
    height, width = image.shape
    result = np.zeros((height + 2*pad, width + 2*pad))
    
    # center
    result[pad:-pad, pad:-pad] = image
    
    # corners
    result[:pad, :pad]   = image[0, 0]
    result[:pad, -pad:]  = image[0, -1]
    result[-pad:, :pad]  = image[-1, 0]
    result[-pad:, -pad:] = image[-1, -1]

    # sides
    result[pad:-pad, :pad] = image[:, np.newaxis, 0]
    result[:pad, pad:-pad] = image[0, np.newaxis, :]
    result[pad:-pad, -pad:] = image[:, np.newaxis, -1]
    result[-pad:, pad:-pad] = image[-1, np.newaxis, :]
    
    return result

@apply_per_channel
def boxfilter(image, radius):
    height, width = image.shape
    
    padded = pad_edges(image, radius + 1)

    pad = 2*radius + 1

    # blur vertically
    vert = np.cumsum(padded, axis=0)
    #temp = vert[-height:, :] - vert[:height, :]
    temp = vert[pad:-1, :] - vert[:height, :]

    # blur horizontally
    horz = np.cumsum(temp, axis=1)
    #temp = horz[:, -width:] - horz[:, :width]
    temp = horz[:, pad:-1] - horz[:, :width]

    # normalize
    return (1.0/(2*radius + 1)**2)*temp

@apply_per_channel
def resize(image, *args):
    return scipy.misc.imresize(image, *args)

def inv3x3(matrices, eps=0.0):
    a, b, c, d, e, f, g, h, i = matrices.reshape([-1, 9]).T

    if eps != 0.0:
        a = a + eps
        e = e + eps
        i = i + eps

    result = np.array([
        [e*i - f*h, c*h - b*i, b*f - c*e],
        [f*g - d*i, a*i - c*g, c*d - a*f],
        [d*h - e*g, b*g - a*h, a*e - b*d]])

    det = a*result[0, 0] + b*result[0, 1] + c*result[0, 2]

    return 1.0/det[:, np.newaxis, np.newaxis] * result.transpose([2, 0, 1])
    

def guided_filter(image, guide, radius, scale=0.125, eps=1e-5):
    I = resize(image, scale, 'bilinear', 'F')
    p = resize(guide, scale, 'bilinear', 'F')
    r = int(radius * scale)
    height, width = p.shape

    mean_I  = boxfilter(I, r)
    mean_p  = boxfilter(p, r)
    corr_Ip = boxfilter(I*p[..., np.newaxis], r)
    cov_Ip  = corr_Ip - mean_I*mean_p[..., np.newaxis]

    var = np.zeros([height, width, 3, 3])
    for i in range(3):
        for j in range(i, 3):
            corr_I = boxfilter(I[..., i]*I[..., j], r)
            var_I = corr_I - mean_I[..., i]*mean_I[..., j]
            var[..., i, j] = var_I
            var[..., j, i] = var_I

    a = np.matmul(inv3x3(var, eps), cov_Ip.reshape([height*width, 3, 1])).reshape(I.shape)

    b = mean_p - np.sum(a*mean_I, axis=2)

    mean_a = boxfilter(a, r)
    mean_b = boxfilter(b, r)

    mean_a = resize(mean_a, guide.shape, 'bilinear', 'F')
    mean_b = resize(mean_b, guide.shape, 'bilinear', 'F')

    return np.sum(mean_a*image, axis=2) + mean_b

image_path = "sunflower.png"
mask_path = "sunflower_foreground.png"

image  = 1.0/256.0 * scipy.misc.imread(image_path)[:, :, :3]
foreground = scipy.misc.imread(mask_path, flatten=True) > 0.5

if 1:
    alpha = foreground
    for _ in range(10):
        alpha = guided_filter(image, alpha, 100)
        alpha = np.clip(alpha, 0, 1)

    plt.subplot(2, 2, 1)
    plt.imshow(image, cmap='gray')
    plt.axis('off')

    plt.subplot(2, 2, 2)
    plt.imshow(foreground, cmap='gray')
    plt.axis('off')

    plt.subplot(2, 2, 3)
    plt.imshow(alpha, cmap='gray')
    plt.axis('off')

    plt.subplot(2, 2, 4)
    plt.imshow(np.concatenate([image, alpha[:, :, np.newaxis]], axis=2), cmap='gray')
    plt.axis('off')

    plt.savefig('result.jpg', bbox_inches='tight', dpi=400)
