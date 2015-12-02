# Assignment 2
This assignment investigates different methods of edge detection including unsharp masking, Sobel operators, and Laplacian of Gauss.

# Compilation
You will need to install opencv 3.0. Then just run make.

# Usage
./visual <input_image>

ESC - exits the program

SPACE - reverts back to the original image

\- - Gets the image negative

= - Equalizes the image

t - Convert the image into a binary image using the threshold specified by the slider

b - Automatically converts the image into a binary image using the average pixel intensity as the cutoff

r - Performs a region detection on the image, assigning sepearate regions different grayscale colors

S - Saves the current displayed image as 'out.tif'

u - Applies the unsharp mask to enhace the image

s - Applies the Sobel operator, generating a gradient image

l - Laplacian of Gaussian 7x7 with sigma = 1.4, generates an edge image

L - Laplacian of Gaussian 11x11 with sigma = 5, generates an edge image
h - Convert color image to HSI
d - Perform forward DCT keeping 9 frequency components
D - Perform forward DCT then backwards again, keeping 9 frequency components
e - Perform forward DCT keeping 1 frequency component
E - Perform forward DCT then backwards again, keeping 1 frequency component
x - Do an ROI detection for lines
X - Do an ROI detection for circles; *note: takes ages*
Tab - If a linear ROI detection was run, show the Hough transform votes
m - Dilate image
M - Erode image
g - Turn RGB image to GS
. - Run image segmentation using morphological ops

## New commands:
1 - produces image M1
2 - produces image N1
3 - produces image N2
4 - produces image N3
5 - produces image N4
6 - produces T1 and prints the error between N1 and T1
7 - produces image K1
8 - produces dumb difference between first two frames
9 - produces dumb difference between second two frames
[ - produces compensated difference between first two frames
] - produces compensated difference between second two frames


## Updates:
