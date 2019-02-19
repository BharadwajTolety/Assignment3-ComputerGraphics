The follwoing file explains the code structure of our project. 

1. How to use project
- When program is run, you will be given the option to input a single command line. 
	Please type in the name of the file you wish to see.
	1. Y-M-C-A
	2. two_buildings
- Please indicate the camera orientation you wish to use. We have two options:
	1. Global stationary
	2. Revolving camera	

2. The scenegraphs have been built with a "bottom-top" approach. First we shall look at the design of the Y-M-C-A model.
	The file is divided into two parts:
	1. The first part consists of 4 files. Each file is used to represnt one of the 4 poses. From Y to A.
	2. The second part is an overall scenegraph that takes all 4 files into consideration, apllies translation to each of them to create adequate spacing between each humanoid.

	The humanoid files contains 2 main groupings: upperbody and lowerbody. 
	The upper body is further divided into pelvis, right arm and left arm along. Transformations are applied to each child node as we make our way from 'palm' to 'shoulder' on both sides. We applied rotations on forearms to create the various shapes for the different letters.

	The lower body does not have any rotations applied as we do not require orient them in different ways.
 