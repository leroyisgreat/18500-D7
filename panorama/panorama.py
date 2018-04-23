import numpy as np
import cv2

def panorama(base_mat, to_be_merged_mat, degree = 0):
    """Computational pipeline example
    ----------
    base_mat: cv::Mat
        A base image
    to_be_merged_mat: cv::Mat
        An image that will be merged with the original
    degree: int
        Kinda like the number of times merged thus far
    Returns
    -------
    cv::Mat
        The resulting image
    """

    # convert cv::Mat to numpy array
    base_numpy = numpy.asarray(base_mat)
    to_be_merged_numpy = numpy.asarray(to_be_merged_mat)
    append_numpy = np.hsplit(to_be_merged_numpy,degree+1)

    output = np.hstack((base_numpy, append_numpy))

    return cv.fromarray(output.astype(np.uint8))
