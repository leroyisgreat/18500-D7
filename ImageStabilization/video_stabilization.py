'''
example usage:
    python python_video_stab.py --video input_video.mov --output output_directory
'''

import cv2
import numpy as np
import pandas as pd
import argparse
import imutils

# 1. Get previous to current fram transformation (dx, dy, da) for all frames
# 2. Accumulate the transformations to get the image trajectory
# 3. Smooth out the trajectory using an averaging window
# 4. Generate new set of previous to current transformation, such that the 
#    trajectory ends up beign the same as the smoothed trajectory
# 5. Apply the new transformation to the video.

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-v", "--video", required=True, help="path to the video to stabilize")
ap.add_argument("-o", "--output", default='.', help="path to dir to save video and transformation files")
args = vars(ap.parse_args())


#####
# CALC T MAT FOR STAB
#####
# set up video capture
cap = cv2.VideoCapture(args['video'])
N = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
fps = int(cap.get(cv2.CAP_PROP_FPS))

# read first frame
status, prev = cap.read()
prev_gray = cv2.cvtColor(prev, cv2.COLOR_BGR2GRAY)
(h, w) = prev.shape[:2]

prev_to_cur_transform = []
for k in range(N - 1):
    status, cur = cap.read()
    cur_gray = cv2.cvtColor(cur, cv2.COLOR_BGR2GRAY)
    prev_corner = cv2.goodFeaturesToTrack(prev_gray, maxCorners=200, qualityLevel=0.01, minDistance=30.0, blockSize=5)
    cur_corner, status, err = cv2.calcOpticalFlowPyrLK(prev_gray, cur_gray, prev_corner, None)
    prev_corner2 = []
    cur_corner2 = []
    for i, st in enumerate(status):
        if st == 1:
            # store coords of keypoints that appear in both
            prev_corner2.append(prev_corner[i])
            cur_corner2.append(cur_corner[i])
    prev_corner2 = np.array(prev_corner2)
    cur_corner2 = np.array(cur_corner2)
    # estimate partial transform (resource: http://nghiaho.com/?p=2208)
    T_new = cv2.estimateRigidTransform(prev_corner2, cur_corner2, False)
    if T_new is not None:
        T = T_new
    dx = T[0, 2]
    dy = T[1, 2]
    da = np.arctan2(T[1, 0], T[0, 0])

    prev_to_cur_transform.append([dx, dy, da])
    prev = cur[:]
    prev_gray = cur_gray[:]


prev_to_cur_transform = np.array(prev_to_cur_transform)
trajectory = np.cumsum(prev_to_cur_transform, axis=0)

trajectory = pd.DataFrame(trajectory)
smoothed_trajectory = trajectory.rolling(window=30, center=False).mean()
smoothed_trajectory = smoothed_trajectory.fillna(method='bfill')

new_prev_to_cur_transform = prev_to_cur_transform + (smoothed_trajectory - trajectory)

#####
# APPLY VIDEO STAB
#####
# initialize transformation matrix
T = np.zeros((2, 3))
new_prev_to_cur_transform = np.array(new_prev_to_cur_transform)
# setup video cap
cap = cv2.VideoCapture(args['video'])
w_write = w
h_write = imutils.resize(prev_gray, width=w_write).shape[0]
out = cv2.VideoWriter('{}/stabilized_output.avi'.format(args['output']),
                      cv2.VideoWriter_fourcc('P', 'I', 'M', '1'), 
                      fps, (w_write, h_write), True)
# loop through frame count
for k in range(N - 1):
    # read current frame
    status, cur = cap.read()
    # read/build transformation matrix
    T[0, 0] = np.cos(new_prev_to_cur_transform[k][2])
    T[0, 1] = -np.sin(new_prev_to_cur_transform[k][2])
    T[1, 0] = np.sin(new_prev_to_cur_transform[k][2])
    T[1, 1] = np.cos(new_prev_to_cur_transform[k][2])
    T[0, 2] = new_prev_to_cur_transform[k][0]
    T[1, 2] = new_prev_to_cur_transform[k][1]
    cur2 = cv2.warpAffine(cur, T, (w, h))
    cur2 = imutils.resize(cur2, width=w)
    out.write(cur2)

print("Done")