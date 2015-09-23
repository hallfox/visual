import cv2
import sys
from matplotlib import pyplot as plt

img = cv2.imread(sys.argv[1], 0)
plt.hist(img.ravel(), 256, [0, 256])
plt.xlim([0, 255])
plt.show()
