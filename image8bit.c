/// image8bit - A simple image processing module.
///
/// This module is part of a programming project
/// for the course AED, DETI / UA.PT
///
/// You may freely use and modify this code, at your own risk,
/// as long as you give proper credit to the original and subsequent authors.
///
/// João Manuel Rodrigues <jmr@ua.pt>
/// 2013, 2023

// Student authors (fill in below):
// NMec:113402  Name:Hugo Santos Ribeiro
// NMec:115243  Name:Ricardo Alexandre Antunes
// 
// 
// Date:
//

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#include "image8bit.h"

#include <string.h>
#include <math.h>
#include <assert.h>
#define NDEBUG
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "instrumentation.h"



// The data structure
//
// An image is stored in a structure containing 3 fields:
// Two integers store the image width and height.
// The other field is a pointer to an array that stores the 8-bit gray
// level of each pixel in the image.  The pixel array is one-dimensional
// and corresponds to a "raster scan" of the image from left to right,
// top to bottom.
// For example, in a 100-pixel wide image (img->width == 100),
//   pixel position (x,y) = (33,0) is stored in img->pixel[33];
//   pixel position (x,y) = (22,1) is stored in img->pixel[122].
// 
// Clients should use images only through variables of type Image,
// which are pointers to the image structure, and should not access the
// structure fields directly.

// Maximum value you can store in a pixel (maximum maxval accepted)
const uint8 PixMax = 255;

// Internal structure for storing 8-bit graymap images
struct image {
  int width;
  int height;
  int maxval;   // maximum gray value (pixels with maxval are pure WHITE)
  uint8* pixel; // pixel data (a raster scan)
};


//declaring summation tables as global variables so that they can be used on multiple functions (altough we will only use them in 3)
int* valuesum1 = NULL;
int* valuesum2 = NULL;

static void InitializeSum1(Image img){
  valuesum1 = (int*) malloc(sizeof(int*) * img->height * img->width); //allocating space for summation table
}

static void InitializeSum2(Image img){
  valuesum2 = (int*) malloc(sizeof(int*) * img->height * img->width); //allocating space for summation table
}


// This module follows "design-by-contract" principles.
// Read `Design-by-Contract.md` for more details.

/// Error handling functions

// In this module, only functions dealing with memory allocation or file
// (I/O) operations use defensive techniques.
// 
// When one of these functions fails, it signals this by returning an error
// value such as NULL or 0 (see function documentation), and sets an internal
// variable (errCause) to a string indicating the failure cause.
// The errno global variable thoroughly used in the standard library is
// carefully preserved and propagated, and clients can use it together with
// the ImageErrMsg() function to produce informative error messages.
// The use of the GNU standard library error() function is recommended for
// this purpose.
//
// Additional information:  man 3 errno;  man 3 error;

// Variable to preserve errno temporarily
static int errsave = 0;

// Error cause
static char* errCause;

/// Error cause.
/// After some other module function fails (and returns an error code),
/// calling this function retrieves an appropriate message describing the
/// failure cause.  This may be used together with global variable errno
/// to produce informative error messages (using error(), for instance).
///
/// After a successful operation, the result is not garanteed (it might be
/// the previous error cause).  It is not meant to be used in that situation!
char* ImageErrMsg() { ///
  return errCause;
}


// Defensive programming aids
//
// Proper defensive programming in C, which lacks an exception mechanism,
// generally leads to possibly long chains of function calls, error checking,
// cleanup code, and return statements:
//   if ( funA(x) == errorA ) { return errorX; }
//   if ( funB(x) == errorB ) { cleanupForA(); return errorY; }
//   if ( funC(x) == errorC ) { cleanupForB(); cleanupForA(); return errorZ; }
//
// Understanding such chains is difficult, and writing them is boring, messy
// and error-prone.  Programmers tend to overlook the intricate details,
// and end up producing unsafe and sometimes incorrect programs.
//
// In this module, we try to deal with these chains using a somewhat
// unorthodox technique.  It resorts to a very simple internal function
// (check) that is used to wrap the function calls and error tests, and chain
// them into a long Boolean expression that reflects the success of the entire
// operation:
//   success = 
//   check( funA(x) != error , "MsgFailA" ) &&
//   check( funB(x) != error , "MsgFailB" ) &&
//   check( funC(x) != error , "MsgFailC" ) ;
//   if (!success) {
//     conditionalCleanupCode();
//   }
//   return success;
// 
// When a function fails, the chain is interrupted, thanks to the
// short-circuit && operator, and execution jumps to the cleanup code.
// Meanwhile, check() set errCause to an appropriate message.
// 
// This technique has some legibility issues and is not always applicable,
// but it is quite concise, and concentrates cleanup code in a single place.
// 
// See example utilization in ImageLoad and ImageSave.
//
// (You are not required to use this in your code!)


// Check a condition and set errCause to failmsg in case of failure.
// This may be used to chain a sequence of operations and verify its success.
// Propagates the condition.
// Preserves global errno!
static int check(int condition, const char* failmsg) {
  errCause = (char*)(condition ? "" : failmsg);
  return condition;
}



/// Init Image library.  (Call once!)
/// Currently, simply calibrate instrumentation and set names of counters.
void ImageInit(void) { ///
  InstrCalibrate();
  InstrName[0] = "pixmem"; // InstrCount[0] will count pixel array acesses
  InstrName[1] = "pixcomp"; // InstrCount[1] will count pixel array comparisons (this counts summation tables)
  InstrName[2] = "iter"; // InstrCount[2] will count any iterations made on anything (aka all iterations made under for or while loops)
  // Name other counters here...
  
}

// Macros to simplify accessing instrumentation counters:
#define PIXMEM InstrCount[0]
#define PIXCOMP InstrCount[1]
#define ITER InstrCount[2]
// Add more macros here...

// TIP: Search for PIXMEM or InstrCount to see where it is incremented!


/// Image management functions

/// Create a new black image.
///   width, height : the dimensions of the new image.
///   maxval: the maximum gray level (corresponding to white).
/// Requires: width and height must be non-negative, maxval > 0.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageCreate(int width, int height, uint8 maxval) { ///
  assert (width >= 0);
  assert (height >= 0);
  assert (0 < maxval && maxval <= PixMax);
  Image image = malloc(sizeof(struct image)); //allocated memory for image
  if(check(image != NULL, "Failed memory allocation")){ //checks if allocation was successful

    image->width = width;
    image->height = height;
    image->maxval = maxval;
    image->pixel = (uint8*) malloc(sizeof(uint8*)*height*width); //allocated memory for pixels (initialization)
    if(check(image != NULL, "Failed memory allocation")){ //checks if allocation was successful
      return image;
    }
    else{ //frees memory if unsuccessful
      free(image->pixel);
      free(image);
      return NULL;
    }
  }
  else{ //frees memory if unsuccessful (pixel not created yet)
    free(image);
    return NULL;
  }
}

/// Destroy the image pointed to by (*imgp).
///   imgp : address of an Image variable.
/// If (*imgp)==NULL, no operation is performed.
/// Ensures: (*imgp)==NULL.
/// Should never fail, and should preserve global errno/errCause.
void ImageDestroy(Image* imgp) { ///frees everything
  assert (imgp != NULL);
  free((*imgp)->pixel);
  free(*imgp);
}


/// PGM file operations

// See also:
// PGM format specification: http://netpbm.sourceforge.net/doc/pgm.html

// Match and skip 0 or more comment lines in file f.
// Comments start with a # and continue until the end-of-line, inclusive.
// Returns the number of comments skipped.
static int skipComments(FILE* f) {
  char c;
  int i = 0;
  while (fscanf(f, "#%*[^\n]%c", &c) == 1 && c == '\n') {
    i++;
  }
  return i;
}

/// Load a raw PGM file.
/// Only 8 bit PGM files are accepted.
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageLoad(const char* filename) { ///
  int w, h;
  int maxval;
  char c;
  FILE* f = NULL;
  Image img = NULL;

  int success = 
  check( (f = fopen(filename, "rb")) != NULL, "Open failed" ) &&
  // Parse PGM header
  check( fscanf(f, "P%c ", &c) == 1 && c == '5' , "Invalid file format" ) &&
  skipComments(f) >= 0 &&
  check( fscanf(f, "%d ", &w) == 1 && w >= 0 , "Invalid width" ) &&
  skipComments(f) >= 0 &&
  check( fscanf(f, "%d ", &h) == 1 && h >= 0 , "Invalid height" ) &&
  skipComments(f) >= 0 &&
  check( fscanf(f, "%d", &maxval) == 1 && 0 < maxval && maxval <= (int)PixMax , "Invalid maxval" ) &&
  check( fscanf(f, "%c", &c) == 1 && isspace(c) , "Whitespace expected" ) &&
  // Allocate image
  (img = ImageCreate(w, h, (uint8)maxval)) != NULL &&
  // Read pixels
  check( fread(img->pixel, sizeof(uint8), w*h, f) == w*h , "Reading pixels" );
  PIXMEM += (unsigned long)(w*h);  // count pixel memory accesses

  // Cleanup
  if (!success) {
    errsave = errno;
    ImageDestroy(&img);
    errno = errsave;
  }
  if (f != NULL) fclose(f);
  return img;
}

/// Save image to PGM file.
/// On success, returns nonzero.
/// On failure, returns 0, errno/errCause are set appropriately, and
/// a partial and invalid file may be left in the system.
int ImageSave(Image img, const char* filename) { ///
  assert (img != NULL);
  int w = img->width;
  int h = img->height;
  uint8 maxval = img->maxval;
  FILE* f = NULL;

  int success =
  check( (f = fopen(filename, "wb")) != NULL, "Open failed" ) &&
  check( fprintf(f, "P5\n%d %d\n%u\n", w, h, maxval) > 0, "Writing header failed" ) &&
  check( fwrite(img->pixel, sizeof(uint8), w*h, f) == w*h, "Writing pixels failed" ); 
  PIXMEM += (unsigned long)(w*h);  // count pixel memory accesses

  // Cleanup
  if (f != NULL) fclose(f);
  return success;
}


/// Information queries

/// These functions do not modify the image and never fail.

/// Get image width
int ImageWidth(Image img) { ///
  assert (img != NULL);
  return img->width;
}

/// Get image height
int ImageHeight(Image img) { ///
  assert (img != NULL);
  return img->height;
}

/// Get image maximum gray level
int ImageMaxval(Image img) { ///
  assert (img != NULL);
  return img->maxval;
}

/// Pixel stats
/// Find the minimum and maximum gray levels in image.
/// On return,
/// *min is set to the minimum gray level in the image,
/// *max is set to the maximum.
void ImageStats(Image img, uint8* min, uint8* max) { ///
  assert (img != NULL);
  *min = ImageGetPixel(img, 0, 0); //setting min to the first pixel
  *max = ImageGetPixel(img, 0, 0); //setting max to the first pixel
  int hei = img->height;
  int wid = img->width;
  for (int x = 0; x < wid; x++){
    for (int y = 0; y < hei; y++){
      uint8 pix = ImageGetPixel(img, x, y); //saving the value in pix so that we don't need to constantly access it
      *min = MIN(*min, pix); //updating *min accordingly
      *max = MAX(*max, pix); //updating *max accordingly
    }
  }
}

/// Check if pixel position (x,y) is inside img.
int ImageValidPos(Image img, int x, int y) { ///
  assert (img != NULL);
  return (0 <= x && x < img->width) && (0 <= y && y < img->height); //x must be between 0 and width - 1 (inclusive) / y must be between 0 and height - 1 (inclusive)
}

/// Check if rectangular area (x,y,w,h) is completely inside img.
int ImageValidRect(Image img, int x, int y, int w, int h) { ///
  assert (img != NULL);
  int hei = img->height;
  int wid = img->width;
  return (x + w <= wid && y + h <= hei); //x + w must be at most width / y + h must be at most height
}

/// Pixel get & set operations

/// These are the primitive operations to access and modify a single pixel
/// in the image.
/// These are very simple, but fundamental operations, which may be used to 
/// implement more complex operations.

// Transform (x, y) coords into linear pixel index.
// This internal function is used in ImageGetPixel / ImageSetPixel. 
// The returned index must satisfy (0 <= index < img->width*img->height)
static inline int G(Image img, int x, int y) {
  /*
  The image begins in (0,0) and goes sideways as in (1,0), (2,0), (3,0) etc until it reaches (width - 1, 0)
  Once it switches lines to (0, 1) the value of the index becomes width - 1 + 1 = width.
  The same process repeats so the value of the index is x + y*width
  */
  int index = x + y*(img->width);
  assert (0 <= index && index < img->width*img->height);
  return index;
}

/// Get the pixel (level) at position (x,y).
uint8 ImageGetPixel(Image img, int x, int y) { ///
  assert (img != NULL);
  assert (ImageValidPos(img, x, y));
  PIXMEM += 1;  // count one pixel access (read)
  return img->pixel[G(img, x, y)];
} 

/// Set the pixel at position (x,y) to new level.
void ImageSetPixel(Image img, int x, int y, uint8 level) { ///
  assert (img != NULL);
  assert (ImageValidPos(img, x, y));
  PIXMEM += 1;  // count one pixel access (store)
  img->pixel[G(img, x, y)] = level;
} 


/// Pixel transformations

/// These functions modify the pixel levels in an image, but do not change
/// pixel positions or image geometry in any way.
/// All of these functions modify the image in-place: no allocation involved.
/// They never fail.


/// Transform image to negative image.
/// This transforms dark pixels to light pixels and vice-versa,
/// resulting in a "photographic negative" effect.
void ImageNegative(Image img) { ///
  assert (img != NULL);
  for (int x = 0; x<img->width; x++){
    for (int y = 0; y<img->height; y++){
      ImageSetPixel(img, x, y, img->maxval - ImageGetPixel(img, x, y)); //maxval is white, therefore doing this operation sets the pixel to the other side of the "brightness" spectrum
    }
  }
}

/// Apply threshold to image.
/// Transform all pixels with level<thr to black (0) and
/// all pixels with level>=thr to white (maxval).
void ImageThreshold(Image img, uint8 thr) { ///
  assert (img != NULL);
  for (int x = 0; x<img->width; x++){
    for (int y = 0; y<img->height; y++){
      ImageSetPixel(img, x, y, (ImageGetPixel(img, x, y) < thr) ? 0 : img->maxval); //sets the value to 0 if the original value is below thr, otherwise maxval
    }
  }
}

/// Brighten image by a factor.
/// Multiply each pixel level by a factor, but saturate at maxval.
/// This will brighten the image if factor>1.0 and
/// darken the image if factor<1.0.
void ImageBrighten(Image img, double factor) { ///
  assert (img != NULL);
  for (int x = 0; x<img->width; x++){
    for (int y = 0; y<img->height; y++){
      ImageSetPixel(img, x, y, MIN((uint8)(ImageGetPixel(img, x, y) * factor + 0.5), img->maxval)); //a negative factor also negates the image while applying the brightening factor. 0.5 is added for rounding purposes (the value is floored by default)
    }
  }
}


/// Geometric transformations

/// These functions apply geometric transformations to an image, a
/// returning a new image as a result.
/// 
/// Success and failure are treated as in ImageCreate:
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.

// Implementation hint: 
// Call ImageCreate whenever you need a new image!

/// Rotate an image.
/// Returns a rotated version of the image.
/// The rotation is 90 degrees anti-clockwise.
/// Ensures: The original img is not modified.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageRotate(Image img) { ///
  assert (img != NULL);
  int wid = img->width;
  int hei = img->height; 
  uint8 maxvalu = img->maxval;
  Image rotated = ImageCreate(hei, wid, maxvalu); // in a 400x200 picture, the rotated version will be 200x400
  for (int x = 0; x<hei; x++){
    for (int y = 0; y<wid; y++){
      ImageSetPixel(rotated, x, y, ImageGetPixel(img, img->width - 1 - y, x)); // anti-clockwise rotation
    }
  }
  return rotated;
}

/// Mirror an image = flip left-right.
/// Returns a mirrored version of the image.
/// Ensures: The original img is not modified.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageMirror(Image img) { ///
  assert (img != NULL);
  int wid = img->width;
  int hei = img->height; 
  uint8 maxvalu = img->maxval;
  Image mirror = ImageCreate(wid, hei, maxvalu);
  for (int x = 0; x<wid; x++){
    for (int y = 0; y<hei; y++){
      ImageSetPixel(mirror, x, y, ImageGetPixel(img, img->width - 1 - x, y)); //mirrored image
    }
  }
  return mirror;
}

/// Crop a rectangular subimage from img.
/// The rectangle is specified by the top left corner coords (x, y) and
/// width w and height h.
/// Requires:
///   The rectangle must be inside the original image.
/// Ensures:
///   The original img is not modified.
///   The returned image has width w and height h.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageCrop(Image img, int x, int y, int w, int h) { ///
  assert (img != NULL);
  assert (ImageValidRect(img, x, y, w, h));
  uint8 maxvalu = img->maxval;
  Image subimg = ImageCreate(w, h, maxvalu);
  for (int xa = 0; xa<w; xa++){
    for (int ya = 0; ya<h; ya++){ //iterates through the pixels of the cropped image
      ImageSetPixel(subimg, xa, ya, ImageGetPixel(img, xa + x, ya + y)); //sets the pixels by correctly locating them in the bigger picture
    }
  }
  return subimg;
}


/// Operations on two images

/// Paste an image into a larger image.
/// Paste img2 into position (x, y) of img1.
/// This modifies img1 in-place: no allocation involved.
/// Requires: img2 must fit inside img1 at position (x, y).
void ImagePaste(Image img1, int x, int y, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidRect(img1, x, y, img2->width, img2->height));
  for (int xa = 0; xa<img2->width; xa++){
    for (int ya = 0; ya<img2->height; ya++){
      ImageSetPixel(img1, xa + x, ya + y, ImageGetPixel(img2, xa, ya)); //copies img2 onto img1
    }
  }
}

/// Blend an image into a larger image.
/// Blend img2 into position (x, y) of img1.
/// This modifies img1 in-place: no allocation involved.
/// Requires: img2 must fit inside img1 at position (x, y).
/// alpha usually is in [0.0, 1.0], but values outside that interval
/// may provide interesting effects.  Over/underflows should saturate.
void ImageBlend(Image img1, int x, int y, Image img2, double alpha) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidRect(img1, x, y, img2->width, img2->height));
  for (int xa = 0; xa<img2->width; xa++){
    for (int ya = 0; ya<img2->height; ya++){
      PIXMEM++;
      img1->pixel[G(img1, xa + x, ya + y)] = MIN(MAX(ImageGetPixel(img2,xa,ya)*alpha + ImageGetPixel(img1, xa + x, ya + y)*(1-alpha) + 0.5, 0),img1->maxval);
      //MAX(value,0) will return 0 if an underflow occured. MIN(value, img1->maxval) will return img1->maxval if an overflow occured. value is either smaller than 0 or greater than maxval, therefore this is enough to handle both overflow and underflow.
    }
  }
}

/// Compare an image to a subimage of a larger image.
/// Returns 1 (true) if img2 matches subimage of img1 at pos (x, y).
/// Returns 0, otherwise.
int ImageMatchSubImage(Image img1, int x, int y, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidPos(img1, x, y));
  /*
    In a summation table, every pixel is accounted for in the area between the squares (0,0), (x,0), (0,y) and (x,y)
    By comparing the values of the summation table instead of the pixels themselves we will find differences much faster if there are any.
    Usually we would only have to check the last row and columns to check if the images are the same.
    However, it is possible for img2 to fool this system by having 4 pixels of which there are 2 in the same row or column and the average of any 2 of them (from a row / column perspective) is the same as the squares that should be there.
    For example, if every pixel is 100 except a square of 4 pixels in img2 in which their values are 99, 101, 101 and 99 (from left to right and from up to bottom), this would trick the system if we only go through the last row and column.
    In a worst case scenario we always have to check every square, but this way, by using global arrays and adjusting the values of the summation table of img1,
    not only do we access values much faster, we also increase the chance of detection of a difference in the image much sooner, which in a normal image will almost always mean immediate detection of a difference.

    If a summation table is not available (for example, if for some reason this function has been called independently from ImageLocateSubImage) then checking every pixel is the best option, but this is only a backup.
    Usually this function will always be called with ImageLocateSubImage.
  */

  //Is not supposed to ever be triggered unless this function is called alone without going through ImageLocateSubImage
  if (valuesum1 == NULL || valuesum2 == NULL){
    for (int xa = 0; xa<img2->width; xa++){
      for (int ya = 0; ya<img2->height; ya++){
        ITER++;
        PIXCOMP++;
        if (ImageGetPixel(img2,xa,ya) != ImageGetPixel(img1, xa+x, ya+y)){
          return 0;
        }
      }
    }
    return 1;
  }

  //iterating through the last row of the summation table.
  for(int i = 1; i < MAX(ImageWidth(img2), ImageHeight(img1)); i++){
    for (int xa = img2->width - i; xa >= 0; xa--){
      ITER++;
      int x1 = xa + x;
      int y1 = y + ImageHeight(img2) - 1;
      //We calculate the sum of the img1 pixels for the same area that we want for img2, having in account the x and y variables though. Below this method will be explained.
      int value1 = valuesum1[G(img1, x1, y1)] - ((x>0) ? valuesum1[G(img1, x-1, y1)] : 0) - ((y>0) ? valuesum1[G(img1, x1, y - 1)] : 0) + ((x>0 && y>0) ? valuesum1[G(img1, x - 1, y - 1)] : 0);
      PIXCOMP++;
      if (value1 != valuesum2[G(img2, xa, img2->height - 1)]){
        return 0;
      }
    }
    
    //iterating through the last column of the summation table.
    for (int ya = img2->height - i - 1; ya >= 0 ; ya--){
      ITER++;
      int x1 = x + ImageWidth(img2) - 1;
      int y1 = y + ya;
      //We calculate the sum of the img1 pixels for the same area that we want for img2, having in account the x and y variables though. Below this method will be explained.
      int value1 = valuesum1[G(img1, x1, y1)] - ((x>0) ? valuesum1[G(img1, x-1, y1)] : 0) - ((y>0) ? valuesum1[G(img1, x1, y - 1)] : 0) + ((x>0 && y>0) ? valuesum1[G(img1, x - 1, y - 1)] : 0);
      PIXCOMP++;
      if (value1 != valuesum2[G(img2, img2->width - 1, ya)]){
        return 0;
      }
    }
  }
  
  return 1;
}
/*
Classic ImageLocateSubImage implementation



int OldImageMatchSubImage(Image img1, int x, int y, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidPos(img1, x, y));
  for (int xa = 0; xa<img2->width; xa++){
    for (int ya = 0; ya<img2->height; ya++){
      ITER++;
      PIXCOMP++;
      if (ImageGetPixel(img2,xa,ya) != ImageGetPixel(img1, xa+x, ya+y)){
        return 0;
      }
    }
  }
  return 1;
}


int OldImageLocateSubImage(Image img1, int* px, int* py, Image img2) {
  assert (img1 != NULL);
  assert (img2 != NULL);
  for (int xa = 0; xa<img1->width - img2->width + 1; xa++){
    for (int ya = 0; ya<img1->height - img2->height + 1; ya++){
      if (OldImageMatchSubImage(img1, xa, ya, img2)){
        *px = xa;
        *py = ya;
        return 1;
      }
    }
  }
  return 0;
}
*/

/// Locate a subimage inside another image.
/// Searches for img2 inside img1.
/// If a match is found, returns 1 and matching position is set in vars (*px, *py).
/// If no match is found, returns 0 and (*px, *py) are left untouched.
int ImageLocateSubImage(Image img1, int* px, int* py, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  InitializeSum1(img1); //allocates correct memory for summation table
  //Creation of summation table for image 1. See below to see what a summation table is. We put it in a global array so that we only have to calculate this once.
  for (int x = 0; x < img1->width; x++){
    for (int y = 0; y < img1->height; y++){
      ITER++;
      valuesum1[G(img1, x, y)] = (int)ImageGetPixel(img1, x, y) + ((x > 0) ? valuesum1[G(img1, x-1, y)] : 0) + ((y > 0) ? valuesum1[G(img1, x, y-1)] : 0) - ((x > 0 && y > 0) ? valuesum1[G(img1, x-1, y-1)] : 0);
    }
  }

  InitializeSum2(img2); //allocates correct memory for summation table
  //Creation of summation table for image 2. See below to see what a summation table is. We put it in a global array so that we only have to calculate this once.
  for (int x2 = 0; x2 < img2->width; x2++){
    for (int y2 = 0; y2 < img2->height; y2++){
      ITER++;
      valuesum2[G(img2, x2, y2)] = (int)ImageGetPixel(img2, x2, y2) + ((x2 > 0) ? valuesum2[G(img2, x2-1, y2)] : 0) + ((y2 > 0) ? valuesum2[G(img2, x2, y2-1)] : 0) - ((x2 > 0 && y2 > 0) ? valuesum2[G(img2, x2-1, y2-1)] : 0);
    }
  }

  for (int xa = 0; xa<img1->width - img2->width + 1; xa++){
    for (int ya = 0; ya<img1->height - img2->height + 1; ya++){
      //iterates through every possible starting point for img2
      //         _ _ _ _ _ _
      //        |_|_|_|_|X|X|
      //        |_|_|_|_|X|X|
      //        |_|_|_|_|X|X|
      //        |_|_|_|_|X|X|
      //        |X|X|X|X|X|X|
      //        |X|X|X|X|X|X|
      //
      //If img2 is 3x3 then we know for sure that the first pixel could never be in the area marked with X. 
      //We avoid going out of bounds this way as well so we don't need to check if posititons are correct.
      if (ImageMatchSubImage(img1, xa, ya, img2)){
          *px = xa;
          *py = ya;
          //We cannot reuse the summation tables for further function calls. The image could have been changed by then, or a different img2 could be used.
          //They need to be deleted and remade every time this function is called.
          free(valuesum1);
          free(valuesum2);
          valuesum1 = NULL;
          valuesum2 = NULL;
          return 1;
      }
    }
  }
  //We cannot reuse the summation tables for further function calls. The image could have been changed by then, or a different img2 could be used.
  //They need to be deleted and remade every time this function is called.
  free(valuesum1);
  free(valuesum2);
  valuesum1 = NULL;
  valuesum2 = NULL;
  return 0;
}





/// Filtering

/// Blur an image by a applying a (2dx+1)x(2dy+1) mean filter.
/// Each pixel is substituted by the mean of the pixels in the rectangle
/// [x-dx, x+dx]x[y-dy, y+dy].
/// The image is changed in-place.
/*
Classic implementation of a box-blurring method.

void ImageOldBlur(Image img, int dx, int dy) { 
  int sum;
  int count;
  Image imgaux = ImageCreate(img->width+1, img->height+1, img->maxval);
  ImagePaste(imgaux,0, 0, img);
  for (int x = 0; x<img->width; x++){
    for (int y = 0; y<img->height; y++){
      sum = 0;
      count = 0;
      for (int xa = x - dx; xa <= x + dx; xa++){
        for (int ya = y - dy; ya <= y + dy; ya++ ){
          PIXCOMP++;
          ITER++;
          if (ImageValidPos(img, xa, ya)){
            sum += ImageGetPixel(imgaux, xa, ya);
            count += 1;
          }
        }
      }
      ImageSetPixel(img, x, y, (sum + count/2)/count);
    }
  }
  ImageDestroy(&imgaux);
}
*/

/*
  Both ImageBlur and ImageLocateSubImage use a summation table to represent images in a different way.
  The value at position (x,y) is the sum of all the pixels in the square between (0,0) and (x,y).
  We can fill the array using dynamic programming. We start at (0,0) and go through every position, by adding both the values to the left and up and subtracting the value in the NW diagonal.
  We do this because when we added the values of the positions above and in the left, we added the pixel values of the (0,0) -> (x-1, y-1) square twice, so we need to remove them again.
  If we are on the first row or column we have to make sure we are not trying to add values that are out of bounds. If the index we are trying to access is out of bounds, we add / subtract 0 instead.

  If we want to access a sum of certain pixels that are together, we can easily do that by adding the value at the end of the range we want, then subtracting the value on the left (plus the horizontal range)
  and the one above (plus the vertical range). We subtracted some values twice so we need to add them again. The concept is similar to when we build the table.
         _ _ _ _ _ _
        |_|_|_|_|_|_|
        |_|-|+|_|_|_|
        |_|+|O|_|_|_|
        |_|_|_|_|_|_|
        |_|_|_|_|_|_|
        |_|_|_|_|_|_|

  To get the value of O in the summation table, we add / subtract the values shown on the table
         _ _ _ _ _ _ 
        |_|_|_|_|_|_|
        |_|+|_|-|_|_|
        |_|_|O|O|_|_|
        |_|-|O|S|_|_|
        |_|_|_|_|_|_|
        |_|_|_|_|_|_|

  To get the sum of values marked with O (and S) we add the square marked with S and the one with + and subtract the ones marked with -.
  We can use this to calculate blur values more efficiently.
*/

void ImageBlur(Image img, int dx, int dy) {
  int* valuesum;
  int blurval, xstart, xend, ystart, yend, xlen, ylen, count;
  valuesum = (int*) malloc(sizeof(int*) * img->height * img->width);
  //We do need to handle not being able to allocate memory for the summation table
  if(check(valuesum != NULL, "Failed memory allocation")){
    //Building the summation table
    for (int x = 0; x < img->width; x++){
      for (int y = 0; y < img->height; y++){
        ITER++;
        valuesum[G(img, x, y)] = (int)ImageGetPixel(img, x, y) + ((x > 0) ? valuesum[G(img, x-1, y)] : 0) + ((y > 0) ? valuesum[G(img, x, y-1)] : 0) - ((x > 0 && y > 0) ? valuesum[G(img, x-1, y-1)] : 0);
      }
    }
    //Iterating through the summation table to find which values to set each pixel with.
    //We also need to check if a pixel is in a border to calculate the average adequately.
    for (int x = 0; x < img->width; x++){
      for (int y = 0; y < img->height; y++){
        ITER++;
        xstart = MAX(x - dx, 0); //we don't want xstart to be a negative value
        ystart = MAX(y - dy, 0); //we don't want ystart to be a negative value
        xend = MIN(x + dx, img->width-1); //we don't want xend to be greater than the highest index
        yend = MIN(y + dy, img->height-1); //we don't want yend to be greater than the highest index
        xlen = xend - xstart + 1; 
        ylen = yend - ystart + 1;
        count = ylen * xlen; //calculating the number of pixels in the box (useful for situations such as pixels being in a border)
        blurval = valuesum[G(img, xend, yend)] - ((ystart > 0) ? valuesum[G(img, xend, ystart - 1)] : 0) - ((xstart > 0) ? valuesum[G(img, xstart - 1, yend)] : 0) + ((xstart > 0 && ystart > 0) ? valuesum[G(img, xstart - 1, ystart - 1)] : 0);
        blurval = (blurval + count / 2)/count; //calculated this way due to rounding issues
        ImageSetPixel(img, x, y, blurval); //only 1 iteration needed per pixel unlike the classic implementation which requires 4 at best.
      }
    }
  }
  //We need to free the summation table to avoid memory leaks.
  free(valuesum);
}