# Sudoku Solver - Augmented Reality
[![macOS build Status](https://github.com/EtoileScintillante/sudoku-ar/workflows/Build-macOS/badge.svg)](https://github.com/EtoileScintillante/sudoku-ar/actions) [![Windows build Status](https://github.com/EtoileScintillante/sudoku-ar/workflows/Build-Windows/badge.svg)](https://github.com/EtoileScintillante/sudoku-ar/actions)  
  
This program solves a sudoku using video capture and displays the solution in real time.

## Example
<img src="https://github.com/EtoileScintillante/sudoku-ar/blob/master/docs/runner.gif" width="400" height="210" alt="example gif" />

## Compile 
   ```
cmake . && make
```
## Run
   ```
./sudoku-ar
```
## Requirements
* [CMake](https://cmake.org/download/)
* [OpenCV](https://github.com/opencv/opencv)

## How it works
1. The program detects the sudoku grid by finding its contours. 
   The assumption here is that the sudoku grid is the biggest blob in video frame.
   
   <img src="https://github.com/EtoileScintillante/sudoku-ar/blob/master/docs/gridContour.png" width="400" height="200" alt="grid contours" />
2. When the grid contours are found, a cropped image of the sudoku grid will be created

   <img src="https://github.com/EtoileScintillante/sudoku-ar/blob/master/docs/cropped.png" width="200" height="200" alt="cropped grid" />

3. The cropped image will then be processed in such a way that the cell contours can be found (the digits will be filtered out)
   
   *After filtering out digits:*
   
   <img src="https://github.com/EtoileScintillante/sudoku-ar/blob/master/docs/noDigits.png" width="200" height="200" alt="no digits" />
   
   *Contours of cells:*
   
   <img src="https://github.com/EtoileScintillante/sudoku-ar/blob/master/docs/cellContours.png" width="200" height="200" alt="contours of cells" />
   
4. Once the contours of the cells have been detected, they are all stored in a 2d vector and sorted from top left to bottom right.
   Then, the program loops through the 2d vector and creates an image of a cell in each iteration. This image of a single cell will also undergo
   some image processing so that is possible to detect whether or not a cell contains a digit. If the cell does contain a digit, the program
   will recognize which digit by using a trained K-Nearest Neighbor (KNN) model. After iterating through the whole 2d vector of contours.
   All digits are stored in a 2d vector of integers (empty cell = 0). 
   
5. The 2d vector of integers (representing the sudoku grid) is passed to a function which tries to solve the sudoku using a backtracking algorithm. 

6. When a solution is found, the program shows the solution in realtime on the video. It does this by first creating a mask of the sudoku grid.
   The mask only contains digits of cells that were originally empty. 
   
    <img src="https://github.com/EtoileScintillante/sudoku-ar/blob/master/docs/mask.png" width="200" height="200" alt="mask" />
    
    Now that the mask is created, it must be placed at the exact position as the sudoku grid on the video. This is achieved by using 
    the getAffineTransfrom function so that the corners of the mask match the corners of the grid. For this we use the grid contours (step 1). And then the warpAffine function is used
    to put the mask in the right position. This goes as follows: first we create a zeros image (image with only black pixels) of the same size as the video frame and then with the x and y coordinates of the grid contours we can put the mask at the right position. Leading to this result: 
    
    <img src="https://github.com/EtoileScintillante/sudoku-ar/blob/master/docs/maskWarped.png" width="400" height="200" alt="warped mask" />
    
    The last step is to blend the video frame and mask together, which results in an augmented reality effect.
    
    <img src="https://github.com/EtoileScintillante/sudoku-ar/blob/master/docs/result.png" width="400" height="200" alt="result" />
    
    To maintain the AR effect, step 1 and 6 are repeated until the program stops. 
    
## Limitations
  * The sudoku grid must be the biggest blob in the video frame.
  * After finding a solution it is not possible to show a new sudoku to the camera and let the program solve the new one.
    The program must be restarted in order to do that.
  * The program can't deal with too much skewness or when the grid is held upside down for example. The AR effect will be lost in those situations. 
  * If, after starting the program, the sudoku grid is not clearly visible in the video (or only a part of the grid is visible)
    the program will still try to detect a grid (in the terminal you will see a message "grid detected") but when this 'grid' 
    (the thing the program wrongly detected as a sudoku grid) is passed to the findCells function it will result in an error.
    So make sure the grid is clearly visible.
