// #pragma GCC optimize("0fast", "unroll-loops", "omit-frame-pointer", "inline")
#pragma GCC option("arch=native", "tune=native", "no-zero-upper")
#pragma GCC target("avx")
#include <x86intrin.h>
#
#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include "Filter.h"
#include <omp.h>

using namespace std;

#include "rdtsc.h"
//Use chmod +x Judge then make judge to see your score
//stride 1 is good, stride n meh(up to down), stride 1 =goes through he same stuff that's in the cache
//input =original picture
//output=picture after we apply the filter
//to unzip use command unzip nameoffile.zip

//
// Forward declare the functions
//
Filter * readFilter(string filename);
double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

int
main(int argc, char **argv)
{

  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;

  for (int inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sum += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
  }
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

struct Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size = 0;
    input >> size;
    Filter *filter = new Filter(size);
    int div;
    input >> div;
    filter -> setDivisor(div);
    for (int i=0; i < size; i++) {
      for (int j=0; j < size; j++) {
	int value;
	input >> value;
	filter -> set(i,j,value);
      }
    }
    return filter;
  } else {
    cerr << "Bad input in readFilter:" << filename << endl;
    exit(-1);
  }
}


double
applyFilter(struct Filter *filter, cs1300bmp *input, cs1300bmp *output)
{

  long long cycStart, cycStop;

  cycStart = rdtscll();

    int inwidth=output -> width = input -> width;
    int inheight=output -> height = input -> height;

    int newh1=inwidth - 1;
    int neww1=inheight - 1;

//     int filterfunc=filter -> getSize();
    int divide=filter -> getDivisor();

//     int sizefilter=filterfunc-1;

    int matrix=0;
    int matrix1, matrix2,matrix3,matrix4,matrix5,matrix6,matrix7,matrix8,matrix9;
    int ij1, ij2, ij3, ij4, ij5,ij6,ij7,ij8,ij9;
    ij1=filter->get(0,0);
    ij2=filter->get(0,1);
    ij3=filter->get(0,2);
    ij4=filter->get(1,0);
    ij5=filter->get(1,1);
    ij6=filter->get(1,2);
    ij7=filter->get(2,0);
    ij8=filter->get(2,1);
    ij9=filter->get(2,2);


    //Changed order of for loop, for easier access for cache
    //We are accessing the elements of the 3d matrix in the order they are layed  //out in memory
    //by accessing each elements in the order it's layed out in memory we are making use of spatial locality because the cache will bring in each of the memory location neighbors and therefore have a cache hit on each successive iteration of the loop

//Plane 0
#pragma omp parallel for
 for(int row = 1; row < newh1 ; row++) {
  for(int col = 1; col < neww1-1; col = col +2) {
    //matrix1=(input ->color[0][row-1][col-1]*filterunroll1)
// 	output -> color[plane][row][col] = 0;
//     output -> color[plane][row][col+1] = 0;

                                                //loop unrolling 1x2 is when we unroll the inner loop of a double for loop
                                                //did loop unrolling 2x2
	       //the inner loop does half of the maintenance work as before, same body                                                        work tho
	 	 //-1 is to make sure we don't get out of bound when we jump by 2


	      matrix1+=(input -> color[0][row  - 1][col - 1] //go through each pixel and then multiplying them with the filter
		 * ij1);
          //-------------
          matrix7+=(input -> color[0][row - 1][col+1] //go through each pixel and then multiplying them with the filter
		 * ij3);
          //-------------
          matrix3+=(input -> color[0][row - 1][col] //go through each pixel and then multiplying them with the filter
		 * ij2);
          //-------------


          //----------------------------
          matrix2+=(input -> color[0][row][col  - 1] //go through each pixel and then multiplying them with the filter
		 * ij4);
          //-------------
          matrix9+= (input -> color[0][row][col+1] //go through each pixel and then multiplying them with the filter
		 * ij6);
          //-------------
           matrix6+=(input -> color[0][row][col] //go through each pixel and then multiplying them with the filter
		 * ij5);
          //----------------------------


          matrix5+=(input -> color[0][row + 1][col-1] //go through each pixel and then multiplying them with the filter
		 * ij7);
          //-------------
          matrix8+= (input -> color[0][row + 1][col+1] //go through each pixel and then multiplying them with the filter
		 * ij9);
         //-------------
          matrix4+=(input -> color[0][row + 1][col] //go through each pixel and then multiplying them with the filter
		 * ij8);
          if(divide!=1){
          matrix=(matrix1+matrix2+matrix3+matrix4+matrix5+matrix6+matrix7+matrix8+matrix9/divide);
          }

//               if (matrix<0)
//               {
//                   matrix=0;
//               }
//               if(matrix>255)
//               {
//                   matrix=255;
//               }

      matrix=matrix < 0 ? 0 :matrix > 255 ? 255 : matrix; //same as what's ab
              output -> color[0][row][col] = matrix;




      //------------------------



      matrix1=(input -> color[0][row  - 1][col] //go through each pixel and then multiplying them with the filter
		 * ij1);
          //-------------
          matrix7=(input -> color[0][row - 1][col+2] //go through each pixel and then multiplying them with the filter
		 * ij3);
          //-------------
          matrix3=(input -> color[0][row - 1][col+1] //go through each pixel and then multiplying them with the filter
		 * ij2);
          //-------------


          //----------------------------
          matrix2=(input -> color[0][row][col] //go through each pixel and then multiplying them with the filter
		 * ij4);
          //-------------
          matrix9= (input -> color[0][row][col+2] //go through each pixel and then multiplying them with the filter
		 * ij6);
          //-------------
           matrix6=(input -> color[0][row][col+1] //go through each pixel and then multiplying them with the filter
		 * ij5);
          //----------------------------


          matrix5=(input -> color[0][row + 1][col] //go through each pixel and then multiplying them with the filter
		 * ij7);
          //-------------
          matrix8= (input -> color[0][row + 1][col+2] //go through each pixel and then multiplying them with the filter
		 * ij9);
         //-------------
          matrix4=(input -> color[0][row + 1][col+1] //go through each pixel and then multiplying them with the filter
		 * ij8);

             if(divide!=1){
          matrix=(matrix1+matrix2+matrix3+matrix4+matrix5+matrix6+matrix7+matrix8+matrix9/divide);
          }
//               if (matrix<0)
//               {
//                   matrix=0;
//               }
//               if(matrix>255)
//               {
//                   matrix=255;
//               }
      matrix=matrix < 0 ? 0 :matrix > 255 ? 255 : matrix;
              output -> color[0][row][col+1] = matrix;
  }

	}

// 	//replace dereferecing
// 	if(divide>1){output -> color[plane][row][col] =
// 	  output -> color[plane][row][col] / divide;   //division and call function is expensive
//     }
// 	else if ( output -> color[plane][row][col]  < 0 ) {
// 	  output -> color[plane][row][col] = 0;
// 	}

// 	else if ( output -> color[plane][row][col]  > 255 ) {
// 	  output -> color[plane][row][col] = 255;
// 	}

//     //-------------

//       //replace dereferecing
//    if(divide>1){
//     output -> color[plane][row][col+1] =
// 	  output -> color[plane][row][col+1] / divide;   //division and call function is expensive
//     }
// 	else if ( output -> color[plane][row][col+1]  < 0 ) {
// 	  output -> color[plane][row][col+1] = 0;
// 	}

// 	else if ( output -> color[plane][row][col+1]  > 255 ) {
// 	  output -> color[plane][row][col+1] = 255;
// 	}

//       }
//     }

//Plane 1
#pragma omp parallel for
 for(int row = 1; row < newh1 ; row++) {
  for(int col = 1; col < neww1-1; col = col +2) {
    //matrix1=(input ->color[0][row-1][col-1]*filterunroll1)
// 	output -> color[plane][row][col] = 0;
//     output -> color[plane][row][col+1] = 0;

                                                //loop unrolling 1x2 is when we unroll the inner loop of a double for loop
                                                //did loop unrolling 2x2
	       //the inner loop does half of the maintenance work as before, same body                                                        work tho
	 	 //-1 is to make sure we don't get out of bound when we jump by 2


	      matrix1=(input -> color[1][row  - 1][col - 1] //go through each pixel and then multiplying them with the filter
		 * ij1);
          //-------------
          matrix7=(input -> color[1][row - 1][col+1] //go through each pixel and then multiplying them with the filter
		 * ij3);
          //-------------
          matrix3=(input -> color[1][row - 1][col] //go through each pixel and then multiplying them with the filter
		 * ij2);
          //-------------


          //----------------------------
          matrix2=(input -> color[1][row][col  - 1] //go through each pixel and then multiplying them with the filter
		 * ij4);
          //-------------
          matrix9= (input -> color[1][row][col+1] //go through each pixel and then multiplying them with the filter
		 * ij6);
          //-------------
           matrix6=(input -> color[1][row][col] //go through each pixel and then multiplying them with the filter
		 * ij5);
          //----------------------------


          matrix5=(input -> color[1][row + 1][col-1] //go through each pixel and then multiplying them with the filter
		 * ij7);
          //-------------
          matrix8= (input -> color[1][row + 1][col+1] //go through each pixel and then multiplying them with the filter
		 * ij9);
         //-------------
          matrix4=(input -> color[1][row + 1][col] //go through each pixel and then multiplying them with the filter
		 * ij8);

            if(divide!=1){
          matrix=(matrix1+matrix2+matrix3+matrix4+matrix5+matrix6+matrix7+matrix8+matrix9/divide);
          }
//               if (matrix<0)
//               {
//                   matrix=0;
//               }
//               if(matrix>255)
//               {
//                   matrix=255;
//               }
      matrix=matrix < 0 ? 0 :matrix > 255 ? 255 : matrix;
              output -> color[1][row][col] = matrix;




      //------------------------



      matrix1=(input -> color[1][row  - 1][col] //go through each pixel and then multiplying them with the filter
		 * ij1);
          //-------------
          matrix7=(input -> color[1][row - 1][col+2] //go through each pixel and then multiplying them with the filter
		 * ij3);
          //-------------
          matrix3=(input -> color[1][row - 1][col+1] //go through each pixel and then multiplying them with the filter
		 * ij2);
          //-------------


          //----------------------------
          matrix2=(input -> color[1][row][col] //go through each pixel and then multiplying them with the filter
		 * ij4);
          //-------------
          matrix9= (input -> color[1][row][col+2] //go through each pixel and then multiplying them with the filter
		 * ij6);
          //-------------
           matrix6=(input -> color[1][row][col+1] //go through each pixel and then multiplying them with the filter
		 * ij5);
          //----------------------------


          matrix5=(input -> color[1][row + 1][col] //go through each pixel and then multiplying them with the filter
		 * ij7);
          //-------------
          matrix8= (input -> color[1][row + 1][col+2] //go through each pixel and then multiplying them with the filter
		 * ij9);
         //-------------
          matrix4=(input -> color[1][row + 1][col+1] //go through each pixel and then multiplying them with the filter
		 * ij8);

            if(divide!=1){
          matrix=(matrix1+matrix2+matrix3+matrix4+matrix5+matrix6+matrix7+matrix8+matrix9/divide);
          }
//               if (matrix<0)
//               {
//                   matrix=0;
//               }
//               if(matrix>255)
//               {
//                   matrix=255;
//               }
      matrix=matrix < 0 ? 0 :matrix > 255 ? 255 : matrix;
              output -> color[1][row][col+1] = matrix;
  }

	}


    //----------------------------------
//Plane 2
#pragma omp parallel for
 for(int row = 1; row < newh1 ; row++) {
  for(int col = 1; col < neww1-1; col = col +2) {
    //matrix1=(input ->color[0][row-1][col-1]*filterunroll1)
// 	output -> color[plane][row][col] = 0;
//     output -> color[plane][row][col+1] = 0;

                                                //loop unrolling 1x2 is when we unroll the inner loop of a double for loop
                                                //did loop unrolling 2x2
	       //the inner loop does half of the maintenance work as before, same body                                                        work tho
	 	 //-1 is to make sure we don't get out of bound when we jump by 2


	      matrix1=(input -> color[2][row  - 1][col - 1] //go through each pixel and then multiplying them with the filter
		 * ij1);
          //-------------
          matrix7=(input -> color[2][row - 1][col+1] //go through each pixel and then multiplying them with the filter
		 * ij3);
          //-------------
          matrix3=(input -> color[2][row - 1][col] //go through each pixel and then multiplying them with the filter
		 * ij2);
          //-------------


          //----------------------------
          matrix2=(input -> color[2][row][col  - 1] //go through each pixel and then multiplying them with the filter
		 * ij4);
          //-------------
          matrix9= (input -> color[2][row][col+1] //go through each pixel and then multiplying them with the filter
		 * ij6);
          //-------------
           matrix6=(input -> color[2][row][col] //go through each pixel and then multiplying them with the filter
		 * ij5);
          //----------------------------


          matrix5=(input -> color[2][row + 1][col-1] //go through each pixel and then multiplying them with the filter
		 * ij7);
          //-------------
          matrix8= (input -> color[2][row + 1][col+1] //go through each pixel and then multiplying them with the filter
		 * ij9);
         //-------------
          matrix4=(input -> color[2][row + 1][col] //go through each pixel and then multiplying them with the filter
		 * ij8);

             if(divide!=1){
          matrix=(matrix1+matrix2+matrix3+matrix4+matrix5+matrix6+matrix7+matrix8+matrix9/divide);
          }
//               if (matrix<0)
//               {
//                   matrix=0;
//               }
//               if(matrix>255)
//               {
//                   matrix=255;
//               }
      matrix=matrix < 0 ? 0 :matrix > 255 ? 255 : matrix;
              output -> color[2][row][col] = matrix;




      //------------------------



      matrix1=(input -> color[2][row  - 1][col] //go through each pixel and then multiplying them with the filter
		 * ij1);
          //-------------
          matrix7=(input -> color[2][row - 1][col+2] //go through each pixel and then multiplying them with the filter
		 * ij3);
          //-------------
          matrix3=(input -> color[2][row - 1][col+1] //go through each pixel and then multiplying them with the filter
		 * ij2);
          //-------------


          //----------------------------
          matrix2=(input -> color[2][row][col] //go through each pixel and then multiplying them with the filter
		 * ij4);
          //-------------
          matrix9= (input -> color[2][row][col+2] //go through each pixel and then multiplying them with the filter
		 * ij6);
          //-------------
           matrix6=(input -> color[2][row][col+1] //go through each pixel and then multiplying them with the filter
		 * ij5);
          //----------------------------


          matrix5=(input -> color[2][row + 1][col] //go through each pixel and then multiplying them with the filter
		 * ij7);
          //-------------
          matrix8= (input -> color[2][row + 1][col+2] //go through each pixel and then multiplying them with the filter
		 * ij9);
         //-------------
          matrix4=(input -> color[2][row + 1][col+1] //go through each pixel and then multiplying them with the filter
		 * ij8);

         if(divide!=1){
          matrix=(matrix1+matrix2+matrix3+matrix4+matrix5+matrix6+matrix7+matrix8+matrix9/divide);
          }
//               if (matrix<0)
//               {
//                   matrix=0;
//               }
//               if(matrix>255)
//               {
//                   matrix=255;
//               }
      matrix=matrix < 0 ? 0 :matrix > 255 ? 255 : matrix;
              output -> color[2][row][col+1] = matrix;
  }

	}

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}
