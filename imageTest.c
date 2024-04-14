// imageTest - A program that performs some image processing.
//
// This program is an example use of the image8bit module,
// a programming project for the course AED, DETI / UA.PT
//
// You may freely use and modify this code, NO WARRANTY, blah blah,
// as long as you give proper credit to the original and subsequent authors.
//
// João Manuel Rodrigues <jmr@ua.pt>
// 2023

#include <assert.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "image8bit.h"
#include "instrumentation.h"

#define PIXMEM InstrCount[0]
#define PIXCOMP InstrCount[1]
#define ITER InstrCount[2]

void MassSetting(Image img, uint8 value){
  for (int x = 0; x < ImageWidth(img); x++){
    for (int y = 0; y < ImageHeight(img); y++){
      ImageSetPixel(img, x, y, value);
    }
  }
}

int main(int argc, char* argv[]) {
  /*
  if (argc != 3) {
    error(1, 0, "Usage: imageTest input.pgm output.pgm"); a
  }
  */

  int px, py;

  long long int old_count = 0;
  long long int new_count = 0;
  double division = 0;

  long long int old_comps = 0;
  long long int new_comps = 0;
  double division_comps = 0;

  long long int old_iters = 0;
  long long int new_iters = 0;
  double division_iters = 0;



  ImageInit();
  /*printf("OLD BLUR ANALYSIS\n");

  printf("Analysis of image size in time complexity (Window 5x5 / Image 5x5 - 1280x1280)\n");
  for (int window = 5; window <= 2560; window*=2){
    Image image = ImageCreate(window, window, PixMax);
    printf("# Blur - Window 5x5 | Image %dx%d\n", window, window);
    InstrReset();
    ImageOldBlur(image, 5, 5);
    if (old_count != 0){
      new_count = PIXMEM;
      division = (long double) new_count/ (long double) old_count;
    }
    old_count = PIXMEM;
    if (old_comps != 0){
      new_comps = PIXCOMP;
      division_comps = (long double) new_comps/ (long double) old_comps;
    }
    old_comps = PIXCOMP;
    if (old_iters != 0){
      new_iters = ITER;
      division_iters = (long double) new_iters/ (long double) old_iters;
    }
    old_iters = ITER;
    InstrPrint();
    printf("PIXMEM 4n/n == %f\n", division);
    printf("PIXCOMPS 4n/n == %f\n", division_comps);
    printf("ITERS 4n/n == %f\n\n", division_iters);
    fflush(stdout);
  }

  new_count = 0, old_count = 0, division = 0;
  new_comps = 0, old_comps = 0, division_comps = 0;
  new_iters = 0, old_iters = 0, division_iters = 0;

  printf("Analysis of window size in time complexity (Window 1x1-128x128 / Image 1000x1000)\n");
  for (int window = 1; window <= 200; window*=2){
    Image image = ImageCreate(1000, 1000, PixMax);
    printf("# Blur - Window %dx%d | Image 1000x1000\n", window, window);
    InstrReset();
    ImageOldBlur(image, window, window);
    if (old_count != 0){
      new_count = PIXMEM;
      division = (long double) new_count/ (long double) old_count;
    }
    old_count = PIXMEM;
    if (old_comps != 0){
      new_comps = PIXCOMP;
      division_comps = (long double) new_comps/ (long double) old_comps;
    }
    old_comps = PIXCOMP;
    if (old_iters != 0){
      new_iters = ITER;
      division_iters = (long double) new_iters/ (long double) old_iters;
    }
    old_iters = ITER;
    InstrPrint();
    ImageDestroy(&image);
    printf("PIXMEM 4n/n == %f\n", division);
    printf("PIXCOMPS 4n/n == %f\n", division_comps);
    printf("ITERS 4n/n == %f\n\n", division_iters);
    fflush(stdout);
  }

  printf("\n----------\n\nNEW BLUR ANALYSIS\n");

  new_count = 0, old_count = 0, division = 0;
  new_comps = 0, old_comps = 0, division_comps = 0;
  new_iters = 0, old_iters = 0, division_iters = 0;

  printf("Analysis of image size in time complexity (Window 5x5 / Image 5x5 - 1280x1280)\n");
  for (int window = 5; window <= 2560; window*=2){
    Image image = ImageCreate(window, window, PixMax);
    printf("# Blur - Window 5x5 | Image %dx%d\n", window, window);
    InstrReset();
    ImageBlur(image, 5, 5);
    if (old_count != 0){
      new_count = PIXMEM;
      division = (long double) new_count/ (long double) old_count;
    }
    old_count = PIXMEM;
    if (old_comps != 0){
      new_comps = PIXCOMP;
      division_comps = (long double) new_comps/ (long double) old_comps;
    }
    old_comps = PIXCOMP;
    if (old_iters != 0){
      new_iters = ITER;
      division_iters = (long double) new_iters/ (long double) old_iters;
    }
    old_iters = ITER;
    InstrPrint();
    ImageDestroy(&image);
    printf("PIXMEM 4n/n == %f\n", division);
    printf("PIXCOMPS 4n/n == %f\n", division_comps);
    printf("ITERS 4n/n == %f\n\n", division_iters);
    fflush(stdout);
  }

  new_count = 0, old_count = 0, division = 0;
  new_comps = 0, old_comps = 0, division_comps = 0;
  new_iters = 0, old_iters = 0, division_iters = 0;

  printf("Analysis of window size in time complexity (Window 1x1 - 512x512 / Image 1000x1000)\n");
  for (int window = 1; window <= 1000; window*=2){
    Image image = ImageCreate(1000, 1000, PixMax);
    printf("# Blur - Window %dx%d | Image 1000x1000\n", window, window);
    InstrReset();
    ImageBlur(image, window, window);
    if (old_count != 0){
      new_count = PIXMEM;
      division = (long double) new_count/ (long double) old_count;
    }
    old_count = PIXMEM;
    if (old_comps != 0){
      new_comps = PIXCOMP;
      division_comps = (long double) new_comps/ (long double) old_comps;
    }
    old_comps = PIXCOMP;
    if (old_iters != 0){
      new_iters = ITER;
      division_iters = (long double) new_iters/ (long double) old_iters;
    }
    old_iters = ITER;
    InstrPrint();
    ImageDestroy(&image);
    printf("PIXMEM 4n/n == %f\n", division);
    printf("PIXCOMPS 4n/n == %f\n", division_comps);
    printf("ITERS 4n/n == %f\n\n", division_iters);
    fflush(stdout);
  }*/

  Image image = ImageLoad(argv[1]);
  Image smaller = ImageCrop(image, 1000, 1000, 30, 30);
  printf("# Image Sublocate - Example on Original Image\n");
  InstrReset();
  int res = ImageLocateSubImage(image, &px, &py, smaller);
  printf("%d\n", res);
  InstrPrint();
  InstrReset();
  OldImageLocateSubImage(image, &px, &py, smaller);
  InstrPrint();
  ImageDestroy(&image);
  ImageDestroy(&smaller);

  /*printf("\n==========================\n\nOLD IMAGE SUBLOCATE ANALYSIS\n");
  printf("Analysis of bigger image size in time complexity (Bigger Image 5x5 - 1280x1280 / Smaller Image 3x3) BC\n");
  for (int window = 5; window <= 1280; window*=2){
    Image smaller = ImageCreate(3,3,PixMax);
    printf("# Image Sublocate - Big %dx%d | Small 3x3\n", window, window);
    InstrReset();
    ImageLocateSubImage(image, &px, &py, smaller);
    if (old_count != 0){
      new_count = PIXMEM;
      division = (long double) new_count/ (long double) old_count;
    }
    old_count = PIXMEM;
    if (old_comps != 0){
      new_comps = PIXCOMP;
      division_comps = (long double) new_comps/ (long double) old_comps;
    }
    old_comps = PIXCOMP;
    if (old_iters != 0){
      new_iters = ITER;
      division_iters = (long double) new_iters/ (long double) old_iters;
    }
    old_iters = ITER;
    InstrPrint();
    ImageDestroy(&image);
    ImageDestroy(&smaller);
    printf("PIXMEM 4n/n == %f\n", division);
    printf("PIXCOMPS 4n/n == %f\n", division_comps);
    printf("ITERS 4n/n == %f\n\n", division_iters);
    fflush(stdout);
  }


  printf("Analysis of smaller image size in time complexity (Bigger Image 800x800 / Smaller Image 1x1 - 512x512)\n");
  for (int window = 1; window <= 512; window*=2){
    Image image = ImageCreate(800, 800, PixMax);
    Image smaller = ImageCreate(window,window,PixMax);
    MassSetting(image, 100);
    MassSetting(smaller, 100);
    printf("# Image Sublocate - Big 800x800 | Small %dx%d\n", window, window);
    InstrReset();
    ImageLocateSubImage(image, &px, &py, smaller);
    if (old_count != 0){
      new_count = PIXMEM;
      division = (long double) new_count/ (long double) old_count;
    }
    old_count = PIXMEM;
    if (old_comps != 0){
      new_comps = PIXCOMP;
      division_comps = (long double) new_comps/ (long double) old_comps;
    }
    old_comps = PIXCOMP;
    if (old_iters != 0){
      new_iters = ITER;
      division_iters = (long double) new_iters/ (long double) old_iters;
    }
    old_iters = ITER;
    InstrPrint();
    ImageDestroy(&image);
    ImageDestroy(&smaller);
    printf("PIXMEM 4n/n == %f\n", division);
    printf("PIXCOMPS 4n/n == %f\n", division_comps);
    printf("ITERS 4n/n == %f\n\n", division_iters);
    fflush(stdout);
  }
  */
  /*
  printf("\n----------\n\nNEW IMAGE SUBLOCATE ANALYSIS\n");

  printf("Analysis of bigger image size in time complexity (Bigger Image 5x5 - 1280x1280 / Smaller Image 3x3)\n");

  new_count = 0, old_count = 0, division = 0;
  new_comps = 0, old_comps = 0, division_comps = 0;
  new_iters = 0, old_iters = 0, division_iters = 0;

  for (int window = 5; window <= 1280; window*=2){
    Image image = ImageCreate(window, window, PixMax);
    Image smaller = ImageCreate(3,3,PixMax);
    MassSetting(image, 100);
    MassSetting(smaller, 100);
    ImageSetPixel(smaller, ImageWidth(smaller) - 1, ImageHeight(smaller) - 1, ImageGetPixel(smaller, ImageWidth(smaller)-1, ImageHeight(smaller)-1) - 1);
    ImageSetPixel(smaller, ImageWidth(smaller) - 1, ImageHeight(smaller) - 2, ImageGetPixel(smaller, ImageWidth(smaller)-1, ImageHeight(smaller)-2) + 1);
    ImageSetPixel(smaller, ImageWidth(smaller) - 2, ImageHeight(smaller) - 1, ImageGetPixel(smaller, ImageWidth(smaller)-2, ImageHeight(smaller)-1) + 1);
    ImageSetPixel(smaller, ImageWidth(smaller) - 2, ImageHeight(smaller) - 2, ImageGetPixel(smaller, ImageWidth(smaller)-2, ImageHeight(smaller)-2) - 1);
    printf("# Image Sublocate - Big %dx%d | Small 3x3\n", window, window);
    InstrReset();
    ImageLocateSubImage(image, &px, &py, smaller);
    if (old_count != 0){
      new_count = PIXMEM;
      division = (long double) new_count/ (long double) old_count;
    }
    old_count = PIXMEM;
    if (old_comps != 0){
      new_comps = PIXCOMP;
      division_comps = (long double) new_comps/ (long double) old_comps;
    }
    old_comps = ITER;
    if (old_iters != 0){
      new_iters = ITER;
      division_iters = (long double) new_iters/ (long double) old_iters;
    }
    old_iters = ITER;
    InstrPrint();
    ImageDestroy(&image);
    ImageDestroy(&smaller);
    printf("PIXMEM 4n/n == %f\n", division);
    printf("PIXCOMPS 4n/n == %f\n", division_comps);
    printf("ITERS 4n/n == %f\n\n", division_iters);
    fflush(stdout);
  }

  printf("Analysis of smaller image size in time complexity (Bigger Image 800x800 / Smaller Image 2x2 - 512x512)\n");

  new_count = 0, old_count = 0, division = 0;
  new_comps = 0, old_comps = 0, division_comps = 0;
  new_iters = 0, old_iters = 0, division_iters = 0;
  for (int window = 2; window <= 5000; window*=2){
    Image image = ImageCreate(10000, 10000, PixMax);
    Image smaller = ImageCreate(window,window,PixMax);
    MassSetting(image, 100);
    MassSetting(smaller, 100);
    ImageSetPixel(smaller, 0, 0, ImageGetPixel(smaller, 0, 0) - 1);
    ImageSetPixel(smaller, 0, 1, ImageGetPixel(smaller, 0, 1) + 1);
    ImageSetPixel(smaller, 1, 0, ImageGetPixel(smaller, 1, 0) + 1);
    ImageSetPixel(smaller, 1, 1, ImageGetPixel(smaller, 1, 1) - 1);
    printf("# Image Sublocate - Big 800x800 | Small %dx%d\n", window, window);
    InstrReset();
    ImageLocateSubImage(image, &px, &py, smaller);
    if (old_count != 0){
      new_count = PIXMEM;
      division = (long double) new_count/ (long double) old_count;
    }
    old_count = PIXMEM;
    if (old_comps != 0){
      new_comps = PIXCOMP;
      division_comps = (long double) new_comps/ (long double) old_comps;
    }
    old_comps = PIXCOMP;
    if (old_iters != 0){
      new_iters = ITER;
      division_iters = (long double) new_iters/ (long double) old_iters;
    }
    old_iters = ITER;
    InstrPrint();
    ImageDestroy(&image);
    ImageDestroy(&smaller);
    printf("PIXMEM 4n/n == %f\n", division);
    printf("PIXCOMPS 4n/n == %f\n", division_comps);
    printf("ITERS 4n/n == %f\n\n", division_iters);
    fflush(stdout);
  }

  printf("Finding most complicated img2 size (Bigger Image 800x800 / Smaller Image 256x256 - 512x512)\n");

  new_count = 0, old_count = 0, division = 0;
  new_comps = 0, old_comps = 0, division_comps = 0;
  new_iters = 0, old_iters = 0, division_iters = 0;
  for (int window = 256; window <= 512; window+=10){
    Image image = ImageCreate(800, 800, PixMax);
    Image smaller = ImageCreate(window,window,PixMax);
    MassSetting(image, 100);
    MassSetting(smaller, 100);
    ImageSetPixel(smaller, 0, 0, ImageGetPixel(smaller, 0, 0) - 1);
    ImageSetPixel(smaller, 0, 1, ImageGetPixel(smaller, 0, 1) + 1);
    ImageSetPixel(smaller, 1, 0, ImageGetPixel(smaller, 1, 0) + 1);
    ImageSetPixel(smaller, 1, 1, ImageGetPixel(smaller, 1, 1) - 1);
    printf("# Image Sublocate - Big 800x800 | Small %dx%d\n", window, window);
    InstrReset();
    ImageLocateSubImage(image, &px, &py, smaller);
    if (old_count != 0){
      new_count = PIXMEM;
      division = (long double) new_count/ (long double) old_count;
    }
    old_count = PIXMEM;
    if (old_comps != 0){
      new_comps = PIXCOMP;
      division_comps = (long double) new_comps/ (long double) old_comps;
    }
    old_comps = PIXCOMP;
    if (old_iters != 0){
      new_iters = ITER;
      division_iters = (long double) new_iters/ (long double) old_iters;
    }
    old_iters = ITER;
    InstrPrint();
    ImageDestroy(&image);
    ImageDestroy(&smaller);
    printf("PIXMEM 4n/n == %f\n", division);
    printf("PIXCOMPS 4n/n == %f\n", division_comps);
    printf("ITERS 4n/n == %f\n\n", division_iters);
    fflush(stdout);
  }

  
*/


}

  /*Image giant = ImageCreate(3000, 3000, PixMax);
  MassSetting(giant, 100);
  int prev_count = 0, cur_count;
  double division;
  for (int i = 1; i < ImageWidth(giant); i*=2){
    Image subgiant = ImageCreate(i, i, PixMax);
    ImageSetPixel(subgiant, ImageWidth(subgiant)-1, ImageHeight(subgiant)-1, ImageGetPixel(subgiant,ImageWidth(subgiant)-1, ImageHeight(subgiant)-1) - 1);
    if (i > 1){
      ImageSetPixel(subgiant, ImageWidth(subgiant)-1, ImageHeight(subgiant)-2, ImageGetPixel(subgiant,ImageWidth(subgiant)-1, ImageHeight(subgiant)-2) + 1);
      ImageSetPixel(subgiant, ImageWidth(subgiant)-2, ImageHeight(subgiant)-1, ImageGetPixel(subgiant,ImageWidth(subgiant)-2, ImageHeight(subgiant)-1) + 1);
      ImageSetPixel(subgiant, ImageWidth(subgiant)-2, ImageHeight(subgiant)-2, ImageGetPixel(subgiant,ImageWidth(subgiant)-2, ImageHeight(subgiant)-2) - 1);
    }
    InstrReset();
    printf("\n# SUBIMAGE LOCATING GIANT size %d square - WORST CASE (sucesss: %d)\n", i, res);
    ImageLocateSubImage(giant, px, py, subgiant);
    if (prev_count != 0){
      cur_count = PIXMEM - (10000*10000);
      division = (double) cur_count/prev_count;
      printf("PIXMEM COMP (2n/n): %f\n", division);
    }
    prev_count = PIXMEM - (10000*10000);
    InstrPrint();
  }*/

  /*printf("# LOAD image");
  ImageInit();
  InstrReset(); // Reset instrumentation
  Image img1 = ImageLoad(argv[1]);
  printf("\nHeight: %d, Width: %d\n", ImageHeight(img1), ImageWidth(img1));

  printf("\n# CROP image");
  Image cp1 = ImageCrop(img1, 0, 0, 150, 150);
  printf("\nCropped Image - Height: %d, Width: %d\n", ImageHeight(cp1), ImageWidth(cp1));

  printf("\n# LOCATE subimage");
  int x, y;
  InstrReset(); // Reset instrumentation
  if (ImageLocateSubImage(img1, &x, &y, cp1)) {
    printf("\nSubimage found at coordinates - x: %d, y: %d\n", x, y);
  } else {
    printf("\nSubimage not found.\n");
  }
  InstrPrint(); // Print instrumentation
  // Free memory
  ImageDestroy(&img1);
  ImageDestroy(&cp1);



  
  // Try changing the behaviour of the program by commenting/uncommenting
  // the appropriate lines.
  int px, py;
  int output = ImageLocateSubImage(cp1, &px, &py, img2);
  printf("%d %d %d\n", output, px, py);
  if (img2 == NULL) {
    error(2, errno, "Rotating img2: %s", ImageErrMsg());
  }


  ImageNegative(img2);
  ImageThreshold(img2, 100);
  ImageBrighten(img2, 1.3);

  if (ImageSave(img2, argv[2]) == 0) {
    error(2, errno, "%s: %s", argv[2], ImageErrMsg());
  }

  ImageDestroy(&img1);
  ImageDestroy(&img2);  */