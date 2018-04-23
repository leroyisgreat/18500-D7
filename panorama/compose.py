#!/usr/bin/python
import time
import os
import string

# take_photos(n):
# take n images with base name 'name' 
# use wait to allow user to move camera at reasonable pace 
# in between capturing images
def take_photos(n, name):
    
    for i in range(0,n):
        os.system("ls -l") 
        os.system("raspistill -vf -hf -o /home/pi/private/18500-D7/panorama/output/" + str(name) +  "_" + str(i) + ".jpg") # enter raspistill 




# panorama():
# take photos in succession and then call the example image stitcher class used 
# in the opencv library 

def panorama():
    n = 5 # this is a filler value for now 
    name = 'pano'
    take_photos(n,name) 
    os.system("./opencv_stitching --mode panorama output/pano_*.jpg")



def main():
    panorama()
    print("done!")


if __name__ == "__main__":
    main



main()
