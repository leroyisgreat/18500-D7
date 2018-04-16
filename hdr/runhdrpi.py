#! /usr/bin/python
# Main script for running the HDR capture-merge sequence
from capturehdr import *
from mergehdr import *
from time import sleep
from datetime import datetime
from subprocess import call

if __name__=="__main__":
    # Options for capture
    emin = 10
    emax = 90
    nexp = 5
    w = 800
    h = 600

    # Log file
    f = open('hdrpi.log', 'a')
    f.write('Starting HDR sequence.\n')
    f.write('Current Time: ' + datetime.now().isoformat())

    # Initalize camera and set resolution
    camera = InitalizeCamera()
    camera.resolution = (w, h)
    f.write('Initialized Camera.\n')

    
    # Capture our images
    images = CaptureHDRStack(camera, emin, emax, nexp)
    # WriteResponseFile(images)
    f.write('Captured HDR Stack.\n')
    # Merge them into an HDR image
    imgname = 'ouptupt.jpg'
    MergeHDRStack(images, imgname)
    f.write('Merged HDR Stack.\n')
