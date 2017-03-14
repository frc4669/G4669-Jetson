import cv2
from networktables import NetworkTable
import numpy as np

videoCapture = None
visionTable = None

def main():
	global videoCapture
	videoCapture = cv2.VideoCapture()
	setupVisionTable()
	runVision()

def setupVisionTable():
	global visionTable
	NetworkTable.setTeam(4669)
	NetworkTable.setPort(5800)
	NetworkTable.setIPAddress("10.46.69.21")
	NetworkTable.setClientMode()
	NetworkTable.initialize()
	visionTable = NetworkTable.getTable("vision")
	#setRunVision(False)

def runVision():
	global visionTable
	
	minH = 44;
	minS = 0;
	minV = 226;

	maxH = 84;
	maxS = 67;
	maxV = 255;

	cannyThresh = 450;
	thresholdThresh = 450; #Not sure if this should be the same var as cannyThresh
	cornerThresh = 235;

	blockSize = 2;
	apertureSize = 3;
	k = 0.04;
	
	contour_length_threshold = 40;
	
	while(True):
		src = getCameraImage()
				
		#Blur
		blurred = cv2.GaussianBlur(src,(15,15),0)
			
		#Filter Color
		#assert(blurred.type() == CV_8UC3);
		blurredHVS = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
		tempProcessed = inRange(blurredHSV, cv2.Scalar(minH, minS, minV), cv2.Scalar(maxH, maxS, maxV))
		filtered = cv2.bitwise_and(blurred, blurred, tempProcessed = tempProcessed)
		
		filtered = cv2.cvtColor(filtered, cv2.COLOR_HSV2BGR)
		filtered = cv2.cvtColor(filtered, cv2.COLOR_BGR2GRAY)
		
		#Contour
		contDrawing = np.zeros(src.shape, dtype = cv2.CV_8UC3)
		
		canny = cv2.Canny(filtered, cannyThresh, cannyThresh*2, 3)
		contours, hierarchy = cv2.findContours(canny, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE)
		th, threshold_output = cv2.threshold(filtered, thresholdThresh, 255, cv2.THRESH_BINARY)
			
		#Bounding Rectangle
		contours_poly = []
        	boundRect = []

        	#Filter out small countour
        	for contour in contours:
            		if contour.size()<contour_length_threshold:
                		contours.remove(contour)

        	#Draw contour
        	for contour in contours:
			poly = cv2.approxPolyDP(contour, 3, true)
            	contours_poly.append(poly)
            	boundRect.append(cv2.boundingRect(poly))

        	for bound in boundRect:
            		color = np.Scalar( 0, 255, 0 )
            		contDrawing = cv2.rectangle( contDrawing, bound.tl(), bound.br(), color, 2, 8, 0 )

        	#imshow("contDrawing", contDrawing);

        	#Corner Detection
        	contGray = cv2.cvtColor(contDrawing, cv2.COLOR_BGR2GRAY)

        	cornerDrawing = np.zeros( contDrawing.size(), cv2.32FC1 )
        	cornerDrawing = cv2.cornerHarris( contGray, blockSize, apertureSize, k, cv2.BORDER_DEFAULT )
        	normal = cv2.normalize(cornerDrawing, 0, 255, cv2.NORM_MINMAX, cv2.32FC1, Mat() )
        	normScaled = cv2.convertScaleAbs(normal)

        	for (int x = 0; x < normal.rows; x++)
        	{
            		for (int y = 0; y < normal.cols; y++)
            		{
                		if ((int) normal.at<float>(x,y) > cornerThresh)
                		{
                    			cv2.circle(normScaled, Point(y,x), 5, np.Scalar(0), 2, 8, 0);
                    			cout << x << " " << y << endl;
                		}
            		}
        	}


        	imshow("Drawing", normScaled); #Keep uncommented in development to avoid videoio error
			
		#hull = getConvexHull(getMaxContour(getContours(processedFrame)))
		if visionTable.isConnected():
			putValuesOnVisionTable(hull)
		#setRunVision(False)

def putValuesOnVisionTable(hull):
	x,y,w,h = cv2.boundingRect(hull)
	#print("Success")
	print(x,y,w,h)
	visionTable.putNumber("x", x)
	visionTable.putNumber("y", y)
	visionTable.putNumber("w", w)
	visionTable.putNumber("h", h)

def setRunVision(b):
	global visionTable
	visionTable.putBoolean("runVision", b)

def getRunVision():
	global visionTable
	return visionTable.getBoolean("runVision", False)

def openCapture():
	global videoCapture
	while (not videoCapture.isOpened()):
		print("Camera is NOT open")
		videoCapture.open(0)
	#print("Camera is open")
	#videoCapture.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, 1920)
	#videoCapture.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, 1080)

def releaseCapture():
	global videoCapture
	videoCapture.release()

def getCameraImage():
	global videoCapture
	#videoCapture = cv2.VideoCapture()
	openCapture()
	retval, frame = videoCapture.read()
	#releaseCapture()
	return frame

def getGrayscale(frame):
	grayscale = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
	return grayscale

def threshold(frame):
	th, newFrame = cv2.threshold(frame, 50, 255, cv2.THRESH_BINARY)
	return newFrame

def getContours(frame):
	contoursList, hierarchy = cv2.findContours(frame, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
	return contoursList

def getMaxContour(contoursList):
	maxContour = None
	if (len(contoursList)>0):
		maxContour = contoursList[0]
		maxArea = cv2.contourArea(maxContour)
		for contour in contoursList:
			area = cv2.contourArea(contour)
			if (area>maxArea):
				maxContour = contour
				maxArea = area
	return maxContour

def getConvexHull(contour):
	hull = cv2.convexHull(contour)
	return hull

main()
