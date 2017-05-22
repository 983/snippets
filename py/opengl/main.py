from OpenGL.GL import *
import OpenGL.GL.shaders
import ctypes
import pygame
import numpy as np
import scipy.misc
import util
import time
import os

"""
class Mesh2D(object):
    def __init__(self, shader, positions, uvs):
        n_vertices = len(positions)//2

        assert(len(uvs) == 2*n_vertices)
        
        vao = glGenVertexArrays(1)
        glBindVertexArray(vao)

        vbo = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, vbo)
        
        position = glGetAttribLocation(shader, 'in_position')
        uv = glGetAttribLocation(shader, 'in_uv')
        
        glEnableVertexAttribArray(position)
        glEnableVertexAttribArray(uv)
        
        BYTES_PER_FLOAT = 4
        FLOATS_PER_VERTEX = 2 + 2
        
        STRIDE = FLOATS_PER_VERTEX*BYTES_PER_FLOAT

        vertices = np.empty(n_vertices*FLOATS_PER_VERTEX, dtype=np.float32)
        
        vertices[0::4] = positions[0::2] # x
        vertices[1::4] = positions[1::2] # y
        vertices[2::4] =       uvs[0::2] # u
        vertices[3::4] =       uvs[1::2] # v
        
        glVertexAttribPointer(position, 2, GL_FLOAT, False, STRIDE, ctypes.c_void_p(BYTES_PER_FLOAT*0))
        glVertexAttribPointer(      uv, 2, GL_FLOAT, False, STRIDE, ctypes.c_void_p(BYTES_PER_FLOAT*2))

        glBufferData(GL_ARRAY_BUFFER, vertices.nbytes, vertices, GL_STATIC_DRAW)
        
        glBindVertexArray(0)
        glDisableVertexAttribArray(position)
        glDisableVertexAttribArray(uv)
        glBindBuffer(GL_ARRAY_BUFFER, 0)

        self.n_vertices = n_vertices
        self.vao = vao
        self.vbo = vbo
        self.shader = shader

    def draw(self):
        glUseProgram(self.shader)
        
        glBindVertexArray(self.vao)
        glDrawArrays(GL_TRIANGLES, 0, self.n_vertices)
        glBindVertexArray(0)
        
        glUseProgram(0)
"""
class Mesh3D(object):
    def __init__(self, shader, texture, positions, uvs):
        n_vertices = len(positions)//3

        assert(len(uvs) == 2*n_vertices)
        
        vao = glGenVertexArrays(1)
        glBindVertexArray(vao)

        vbo = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, vbo)
        
        position = glGetAttribLocation(shader, 'in_position')
        uv = glGetAttribLocation(shader, 'in_uv')
        
        glEnableVertexAttribArray(position)
        glEnableVertexAttribArray(uv)
        
        BYTES_PER_FLOAT = 4
        FLOATS_PER_VERTEX = 3 + 2
        
        STRIDE = FLOATS_PER_VERTEX*BYTES_PER_FLOAT

        vertices = np.empty(n_vertices*FLOATS_PER_VERTEX, dtype=np.float32)
        
        vertices[0::5] = positions[0::3] # x
        vertices[1::5] = positions[1::3] # y
        vertices[2::5] = positions[2::3] # z
        vertices[3::5] =       uvs[0::2] # u
        vertices[4::5] =       uvs[1::2] # v
        
        glVertexAttribPointer(position, 3, GL_FLOAT, False, STRIDE, ctypes.c_void_p(0*BYTES_PER_FLOAT))
        glVertexAttribPointer(      uv, 2, GL_FLOAT, False, STRIDE, ctypes.c_void_p(3*BYTES_PER_FLOAT))

        glBufferData(GL_ARRAY_BUFFER, vertices.nbytes, vertices, GL_STATIC_DRAW)
        
        glBindVertexArray(0)
        glDisableVertexAttribArray(position)
        glDisableVertexAttribArray(uv)
        glBindBuffer(GL_ARRAY_BUFFER, 0)

        self.n_vertices = n_vertices
        self.vao = vao
        self.vbo = vbo
        self.shader = shader
        self.texture = texture

    def draw(self):
        glUseProgram(self.shader)
        
        glActiveTexture(GL_TEXTURE0)
        glBindTexture(GL_TEXTURE_2D, self.texture)
        texture0_location = glGetUniformLocation(self.shader, "texture0")
        glUniform1i(texture0_location, 0)
        
        glBindVertexArray(self.vao)
        glDrawArrays(GL_TRIANGLES, 0, self.n_vertices)
        glBindVertexArray(0)
        
        glUseProgram(0)

def get_mvp(window_size):
    width, height = window_size

    aspect_ratio = float(width)/float(height)
    
    projection = util.make_projection(-aspect_ratio, aspect_ratio, -1, 1, 1, 10)
    
    projection = util.make_perspective(45, aspect_ratio, 1, 100)
    
    camera = np.float32([0, 5, 0])
    target = np.float32([0, 0, 0])
    up = np.float32([0, 0, 1])
    view = util.make_look_at(camera, target, up)

    angle = time.clock()
    
    model = util.make_rotz(angle)
    model = model@util.make_roty(angle*0.7)
    model = model@util.make_rotx(angle*-0.3)

    mvp = projection@view@model

    return mvp, model, view, projection
    

def upload_mvp(window_size, shader, mvp):
    glUseProgram(shader)
    mvp_loc = glGetUniformLocation(shader, "mvp")
    assert(mvp_loc != -1)
    glUniformMatrix4fv(mvp_loc, 1, True, mvp)
    glUseProgram(0)

def display(window_size, shader, meshes, mvp):
    width, height = window_size
    glViewport(0, 0, width, height)

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    upload_mvp(window_size, shader, mvp)
    
    for mesh in meshes:
        mesh.draw()

def make_shader(vertex_shader_source, fragment_shader_source):
    vertex_shader = OpenGL.GL.shaders.compileShader(vertex_shader_source, GL_VERTEX_SHADER)
    fragment_shader = OpenGL.GL.shaders.compileShader(fragment_shader_source, GL_FRAGMENT_SHADER)
    
    shader = OpenGL.GL.shaders.compileProgram(vertex_shader, fragment_shader)

    return shader

def make_boring_shader():
    vertex_shader_source = """
    #version 330

    in vec4 in_position;
    in vec2 in_uv;

    uniform mat4 mvp;

    out vec2 uv;

    void main(){
        uv = in_uv;
        gl_Position = mvp*in_position;
    }
    """

    fragment_shader_source = """
    #version 330

    in vec2 uv;

    uniform sampler2D texture0;

    void main(){
       gl_FragColor = texture(texture0, uv);
    }
    """
    
    shader = make_shader(vertex_shader_source, fragment_shader_source)

    return shader

def texture_from_image(image):
    texture = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, texture)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    
    height, width, n_channels = image.shape

    if n_channels == 3:
        data_type = GL_RGB
    elif n_channels == 4:
        data_type = GL_RGBA
    
    glTexImage2D(GL_TEXTURE_2D, 0, data_type, width, height, 0, data_type, GL_UNSIGNED_BYTE, image)

    glGenerateMipmap(GL_TEXTURE_2D)

    return texture

def load_texture(path):
    image = util.load_image(path)
    return texture_from_image(image)

def main():
    pygame.init()
    window_size = (512, 512)
    screen = pygame.display.set_mode(window_size, pygame.OPENGL | pygame.DOUBLEBUF | pygame.RESIZABLE)

    #pygame.display.gl_set_attribute(pygame.GL_DEPTH_SIZE, 32)
    
    glCullFace(GL_BACK)

    glDepthFunc(GL_LEQUAL)
    glEnable(GL_DEPTH_TEST)
    
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    glClearColor(0.1, 0.2, 0.3, 1.0)

    shader = make_boring_shader()

    images = util.load_images("textures")
    textures = [texture_from_image(image) for image in images]

    #mesh = Mesh3D(shader, util.cube_vertex_positions, util.cube_texture_coordinates)
    meshes = []
    positions = util.split_n(util.cube_vertex_positions, 6)
    tex_coords = util.split_n(util.cube_texture_coordinates, 6)
    for i in range(6):
        mesh = Mesh3D(shader, textures[i], positions[i], tex_coords[i])
        meshes.append(mesh)
        
    clock = pygame.time.Clock()

    try:
        os.mkdir("frames")
    except FileExistsError:
        pass

    FPS = 60
    frame = 0
    while True:
        clock.tick(FPS)
        for event in pygame.event.get():
            if event.type == pygame.VIDEORESIZE:
                window_size = event.size
            if event.type == pygame.QUIT:
                return
            if event.type == pygame.KEYUP and event.key == pygame.K_ESCAPE:
                return

        mvp, model, view, projection = get_mvp(window_size)

        display(window_size, shader, meshes, mvp)
        
        pygame.display.flip()

        # save frame
        pygame.image.save(screen, "frames/%d.jpg"%frame)
        with open("matrices.npy", "ab") as f:
            np.save(f, model)
            np.save(f, view)
            np.save(f, projection)
        
        frame += 1

if __name__ == '__main__':
    try:
        main()
    finally:
        pygame.quit()
