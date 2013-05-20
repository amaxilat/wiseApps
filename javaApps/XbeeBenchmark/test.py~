#!/usr/bin/python
# needs Python Image Library (PIL)
import Image, ImageDraw
import math

# some color constants for PIL
white = (255, 255, 255)
black = (0, 0, 0)
blue = (0, 0, 255)
red = (255, 0, 0)
green = (0,128,0)

width = 1166
height = 866
center = height//2
rrange = width/100
x_increment = 1
# width stretch
x_factor = 0.04
# height stretch
y_amplitude = 80

nodes = []

import random
for i in range(100):
  nodes.append( (int (random.uniform(0, width/2)),int(random.uniform(0, height/2)),int(random.uniform(0, 255))))

img = Image.open("ns_floor0.png","r")

image1 = Image.new("RGB", (width, height), white)
image1.paste(img)

draw = ImageDraw.Draw(image1)

# do the PIL image/draw (in memory) drawings
for i in range(len(nodes)-1):
  draw.ellipse((nodes[i][0]-rrange, nodes[i][1]-rrange, nodes[i][0]+rrange, nodes[i][1]+rrange),fill=(nodes[i][2],0,0))

# PIL image can be saved as .png .jpg .gif or .bmp file
filename = "my_drawing.jpg"
image1.save(filename)
