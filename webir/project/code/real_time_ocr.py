# coding: utf-8
# =====================================================================
#  py Ver:      python 3.6 or later
#
#  Description: Recognizes regions of text in a given video or through the webcam feed
#  Note: Requires opencv 3.4.2 or later
# =====================================================================

from imutils.video import VideoStream
from imutils.video import FPS
from imutils.object_detection import non_max_suppression
import numpy as np
import argparse
import imutils
import time
import cv2
import pytesseract
from pytesseract import Output
from imutils.video import count_frames
import os

minArea = 1000

def calcThresh(frame):
    thresh = cv2.threshold(frame,0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)[1]
    return cv2.dilate(thresh, None, iterations=2)

def detectContours(thresh):
    cnts = cv2.findContours(
        thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    cnts = cnts[0] if imutils.is_cv2() else cnts[1]

    validCnts = []

    # loop over the contours
    for c in cnts:
        # if the contour is too small, ignore it
        if cv2.contourArea(c) < minArea:
            continue

        validCnts.append(c)

    return validCnts


def get_arguments():
    ap = argparse.ArgumentParser()
    ap.add_argument('-v', '--video', type=str,
                    help='path to optional video file')
    ap.add_argument('-c', '--min_confidence', type=float, default=0.5,
                    help='minimum confidence to process a region')
    ap.add_argument('-p', '--padding', type=float, default=0.0,
                    help='padding on each ROI border')
    arguments = vars(ap.parse_args())

    return arguments


def main_ocr(vidpath):
    progressShow = 5
    min_conf = 0.5

    num_frames_saved = 0

    vs = cv2.VideoCapture(vidpath)

    outdir = vidpath[:-4] + "/"
    if not os.path.exists(outdir):
        os.makedirs(outdir)

    fps = FPS().start()

    firstFrame = None
    # current pos in vid
    currentPosition = 0
    totalFrames = count_frames(vidpath, override=False)

    custom_config = "-l eng --oem 1 --psm 3  -c preserve_interword_spaces=1"

    while True:

        frame = vs.read()
        frame = frame[1]

        if frame is None:
            break

        ################# detect slide change ##################
        h, w = frame.shape[:2]
        centreframe = frame[int(h/4): int(3*h/4), int(w/4): int(3*w/4), :]
        gray = cv2.cvtColor(centreframe, cv2.COLOR_BGR2GRAY)
        gray = cv2.GaussianBlur(gray, (21, 21), 0)
        
        if firstFrame is None:
            firstFrame = np.zeros(gray.shape, np.uint8)
            firstFrameTime = time.time()

        frameDelta = cv2.absdiff(firstFrame, gray)
        thresh = calcThresh(frameDelta)
        cnts = detectContours(thresh)

        # we have motion, new firstFrame, and apply OCR
        if len(cnts) > 0:
            updateTime = time.time()

            #only update new slide if previous slide no motion for 3s
            if ((updateTime - firstFrameTime > 2) or (currentPosition==0)):
                
                num_frames_saved += 1

                firstFrameTime = updateTime

                f=open(outdir + str(currentPosition)+".txt", "a+")

                firstFrame = gray

                #save frame
                cv2.imwrite(outdir + str(currentPosition) + ".jpg", frame)

                #################### OCR ##################
                # load the input image, convert it from BGR to RGB channel ordering,
                # and use Tesseract to localize each area of text in the input image
                rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                results = pytesseract.image_to_data(rgb, config=custom_config, output_type=Output.DICT)

                # loop over each of the individual text localizations
                for i in range(0, len(results["text"])):
                    # extract the bounding box coordinates of the text region from
                    # the current result
                    x = results["left"][i]
                    y = results["top"][i]
                    w = results["width"][i]
                    h = results["height"][i]
                    # extract the OCR text itself along with the confidence of the
                    # text localization
                    text = results["text"][i]
                    conf = int(results["conf"][i])
                # filter out weak confidence text localizations
                    if conf > min_conf:
                        #save text into text file
                        f.write(text + " ")

                        cv2.rectangle(rgb, (x, y), (x+w, y+h), (0, 0, 255), 2)
                        cv2.putText(rgb, text, (x, y - 20),
                                    cv2.FONT_HERSHEY_SIMPLEX, 1.2, (0, 0, 255), 3)


                        #cv2.imshow("Detection", rgb)
                        #save frame
                        if (currentPosition ==0):
                            cv2.imwrite(outdir + str(currentPosition) + "_text.jpg", rgb)
        
        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):
            break


        currentPosition += 1
        progress = (currentPosition / totalFrames) * 100

        if (progress > progressShow):
            print("progress: " + str(progress))
            progressShow += 5

        fps.update()

    fps.stop()
    print(f"[INFO] elapsed time {round(fps.elapsed(), 2)}")
    print(f"[INFO] approx. FPS : {round(fps.fps(), 2)}")
    
    return num_frames_saved, fps.elapsed(), fps.fps()


if __name__ == '__main__':

    main_ocr("webcrawler_lecture.mp4")