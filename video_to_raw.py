#!/usr/bin/python
# Converts a video file to a raw file
# The size of the raw file is approximately
# 12 * video frame rate * video length * video width (pixels) * video height bytes
# This is VERY BIG for long/high-resolution videos.
# 10 seconds of 1920x1080 30fps video will be about 7.5 GB.
# A warning will be shown if the size of the output file will exceed 1GB.
# It will also take a long time for long videos, so it is recommended that
# you reduce the resolution and/or framerate and/or length of the video
# before you input it into here.
# On my computer, it takes 5.8 seconds to process a 10 second 30 fps 640x360 video
# (and the actual color reduction takes around 19 seconds)
# This won't handle audio by the way, so you'll need to separate then rejoin
# it, which you can use ffmpeg to do

import cv2
import sys
import numpy as np

if len(sys.argv) < 3:
    print 'Error: Usage: {} <input file> <output file>'.format(sys.argv[0])

cap = cv2.VideoCapture(sys.argv[1])
nframes = int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_COUNT))
width = int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT))
framerate = int(cap.get(cv2.cv.CV_CAP_PROP_FPS))
output_file = open(sys.argv[2], "wb")
ndata = nframes * width * height
if ndata > (2 ** 32 - 1):
    raise ValueError('Video file or pixels_to_use_per_frame is too big. It must be that the number of frames in the video multiplied by pixels_to_use_per_frame is less than 2^32.')

filesize = 12 * nframes * width * height
if filesize >= 1e9:
    sys.stderr.write('Warning: This program is about to create a large file ({} GB).\nWould you like to create the file (yes/no)? '.format(filesize/1e9))
    sys.stderr.flush()
    cont = raw_input('').lower()
    if cont[0] != 'y':
        print 'Aborted.'
        sys.exit(0)

data_size = 3
size = np.array([ndata, data_size], dtype='uint32')
size.tofile(output_file)

frameno = 0
while True:
    ret, frame = cap.read()
    if not ret:
        break
    frameno += 1
    print 'Processing frame {}/{}...'.format(frameno, nframes)
    height, width, colors = frame.shape
    if colors < 3:
        raise AttributeError('Video must have at least 3 color channels')

    (frame.flatten()/256.).astype(np.float32).tofile(output_file)

cap.release()
output_file.close()
print 'Done.'
print 'When you are converting the raw file back to a video use:'
print 'python raw_to_video.py <input filename> <output filename> {} {} {}'.format(width, height, framerate)
