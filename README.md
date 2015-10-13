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
