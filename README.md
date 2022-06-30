# Sudoku-augmented-reality

This program solves a sudoku using video capture and displays the solution in real time.

### Example:
<img src="https://github.com/EtoileScintillante/sudoku-ar/blob/master/docs/example.gif" width="400" height="200" alt="example gif" />

### How it works:
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
    the getAffineTransfrom function (so that the corners of the mask match the corners of the grid) and then the warpAffine function is used
    to place the mask on the right position. 
    
    <img src="https://github.com/EtoileScintillante/sudoku-ar/blob/master/docs/maskWarped.png" width="400" height="200" alt="warped mask" />
    
    Note that the image of the mask now has the same size as the video window, but the background is still black. 
    The last step is to blend the video frame and mask together, which results in an augmented reality effect.
    
    <img src="https://github.com/EtoileScintillante/sudoku-ar/blob/master/docs/result.png" width="400" height="200" alt="result" />
    
### Limitations:
  * The sudoku grid must the biggest blob in video frame.
  * After finding a solution it is not possible to show a new sudoku to the camera and let the program solve the new one.
    The program must be restarted in order to do that.
  * The program can't deal with too much skewness or when the grid is held upside down for example. The AR effect will be lost in those situations. 
  * If, after starting the program, the sudoku grid is not clearly visible in the video (or only a part of the grid is visible)
    the program will still try to detect a grid (in the terminal you will see a message "grid detected") but when this 'grid' 
    (the thing the program wrongly detected as a sudoku grid) is passed to the findCells function it will result in an error.
    So make sure that the grid is clearly visible. 
