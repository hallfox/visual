# Assignment 1
This assignment is all about negating, equalizing, and detecting regions on grayscale images.

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

s - Saves the current displayed image as 'out.tif'
