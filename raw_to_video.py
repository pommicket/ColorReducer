#!/usr/bin/python
# Converts a raw file to a video file

import cv2
import sys
import numpy as np

if len(sys.argv) < 6:
    print 'Error: Usage: {} <input file> <output file> <width> <height> <framerate>'.format(sys.argv[0])
    print 'The width in pixels, height in pixels, and framerate should have been outputted when you created the raw file.'
    sys.exit(-1)

filename_in = sys.argv[1]
filename_out = sys.argv[2]
width = int(sys.argv[3])
height = int(sys.argv[4])
framerate = int(sys.argv[5])

if not filename_out.endswith('.avi'):
    raise ValueError('This only supports .avi files for output. Please change the name of the output file so that it ends in .avi.')

out = cv2.VideoWriter(filename_out,cv2.cv.CV_FOURCC('M','J','P','G'), framerate, (width,height))
input_file = open(filename_in, 'rb')

sizes = np.fromfile(input_file, dtype='uint32', count=2)
assert sizes[1] == 3
assert sizes[0] % (width * height) == 0
nframes = sizes[0] // (width * height)
for frameno in xrange(nframes):
    print 'Processing frame {}/{}...'.format(frameno+1,nframes)
    frame = np.fromfile(input_file, dtype='float32', count=width * height * 3)
    frame = (frame*255).astype(np.uint8)
    frame = np.reshape(frame, (height, width, 3))
    out.write(frame)

out.release()
input_file.close()
