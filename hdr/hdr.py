## @brief Main script for running the HDR capture-merge sequence
#
#  @author  Marcus Greer <mgreer@andrew.cmu.edu>
#  @author  LeRoy Gary   <lgary@andrew.cmu.edu>

def hdr():
    # Options for timelapse
    nimages = 1 #2160
    # Options for capture
    emin = 10
    emax = 90
    nexp = 5

    # Log file
    log = open('hdrpi.log', 'a')
    log.write('Starting HDR sequence.\n')
    log.write('Current Time: ' + datetime.now().isoformat())

    images = CaptureHDRStack(camera, emin, emax, nexp)
    log.write('Captured HDR Stack.\n')
    # Merge them into an HDR image
    MergeHDRStack(images)
    log.write('Merged HDR Stack.\n')

"""
    @brief CaptureHDRStack(exposure_min, exposure_max, nimages)
    captures a set of nimages from the pi's camera with
    exposures ranging from min to max.

    @param exposure_min
    @param exposure_max
    @param nimages

    @return a list of cv::Mat frames.
"""
def CaptureHDRStack(camera, exposure_min, exposure_max, nimages):
    exp_step = (exposure_max - exposure_min) / (nimages-1)
    exposures = range(exposure_min, exposure_max+1, exp_step)
    frames = []
    for step in exposures:
        # Set camera properties and capture
        camera.brightness = step
        # Get capture
        frames.append(camera.capture())
    return frames

"""
    @brief MergeHDRStack(images)
    Merge the files contained in images using enfuse.
    options are passed to enfuse.

    @param images

    @return
"""
def MergeHDRStack(images):
    if image_name is None:
        now = datetime.now()
        # Create a formated date for the file name
        date = now.__format__('%Y-%m-%d')
        seconds_since_midnight = (now - now.replace(hour=0, minute=0, second=0, microsecond=0)).total_seconds()
        minutes = int(seconds_since_midnight/60)
        image_name = '%sT%04d.jpg' % (date, minutes)
    
    outfile = '--output=%s' % (image_name)

    call(["enfuse", outfile, "@hdrstack"])
    print "wrote file %s" % (image_name)
    return True
