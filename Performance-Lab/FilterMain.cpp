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
// Forward declare the functions

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

    //Calling functions inside of loop is expensive so we want to create a local variable
    // so that it runs faster
    //create local variables, don't have to rereference output
    const short  filterfunc = filter -> getSize();

  //Array to hold filter x, y values
    //Saves room inside of the main loop, no need to run it there
  //create an array of filters to access outside of main loops
    int ArrayfilterXY[filterfunc][filterfunc];


    const short  inwidth=output -> width = input -> width;
    const short  inheight=output -> height = input -> height;

    const short  neww1 = inwidth - 1;
    const short  newh1 = inheight - 1;

    const short  divide = filter -> getDivisor();


  //New loop to process x, y values outside of main loop
    //stride 1
    #pragma omp parallel for
    for(short i = 0; i < filterfunc; i++)
    for(short j = 0; j < filterfunc; j++)
      ArrayfilterXY[i][j] = filter -> get(i, j);

  //Values for each plane in the image
    int plane1, plane2, plane3,
      plane12, plane22, plane32,
      plane13, plane23, plane33;

  //Switched order of variables from Stride-N to Stride-1
  //Removed outer loop and processed each plane within the current structure
  //Step two rows and columns at a time to reduce iterations
    //Changed order of for loop, for easier access for cache
    //We are accessing the elements of the 3d matric in the order they are layed out in memory
    //by accessing each elements in the order it's layed out in memory we are making use of spatial locality because the cache will bring in each of the memory location neighbors and therefore have a cache hit on each successive iteration of the loop
    #pragma omp parallel for
  for(short row = 1; row < newh1; row++) {
    for(short col = 1; col < neww1; col++) {
    //row-major order, the inner loop reads the elements of the first row, good spatial locality
        //references the array in the same row-major order that the array is stored
      //Reinitialize plane values to 0
      plane1= 0;
      plane2= 0;
      plane3= 0;

      plane12= 0;
      plane22= 0;
      plane32= 0;

      plane13= 0;
      plane23= 0;
      plane33= 0;


      //Switched order of variables from Stride-N to Stride-1
      for (short i = 0; i < filterfunc; i++) {

 	//Reinitialize temp variables to 0
	short tempvar = 0;
	short tempvar2 = 0;
	short tempvar3 = 0;

	short tempvar4 = 0;
	short tempvar5 = 0;
	short tempvar6 = 0;

	short tempvar7 = 0;
	short tempvar8 = 0;
	short tempvar9 = 0;


	for (short j = 0; j < filterfunc; j++) {

        //access the filter array rather than trying to compute the filter each time
        //replace filter->get(i,j)
        //basically similar to creating local variable
	  short xy = ArrayfilterXY[i][j];


	  //Process each plane individually instead of looping
	  tempvar += input -> color[0][row + i - 1][col + j - 1] * xy;  //original
	  tempvar2 += input -> color[1][row + i - 1][col + j - 1] * xy;
	  tempvar3 += input -> color[2][row + i - 1][col + j - 1] * xy;

	  tempvar4 += input -> color[0][row + i - 1][col + j] * xy;
	  tempvar5 += input -> color[1][row + i - 1][col + j] * xy;
	  tempvar6 += input -> color[2][row + i - 1][col + j] * xy;

	  tempvar7 += input -> color[0][row + i-1][col + j - 1] * xy;
	  tempvar8 += input -> color[1][row + i-1][col + j - 1] * xy;
	  tempvar9 += input -> color[2][row + i-1][col + j - 1] * xy;

//-----Loop unrolling--
//
//         tempvar += input -> color[0][row + i - 1][col + j - 1] * xy;
//         tempvar4 += input -> color[0][row + i - 1][col + j] * xy;
//         tempvar7 += input -> color[0][row + i][col + j - 1] * xy;



	}

	//Add temp values to plane values
    //update output with temp value
	plane1+= tempvar;
	plane2+= tempvar2;
	plane3+= tempvar3;
 	plane12+= tempvar4;
	plane22+= tempvar5;
	plane32+= tempvar6;
    plane13+= tempvar7;
	plane23+= tempvar8;
	plane33+= tempvar9;
      }

      //Use local divisor variable instead of function call
      //Do not calculate if divisor is 1
        // if(divide>1)
     //  output -> color[plane][row][col] /= divide;   //division and call function is expensive

      if(divide != 1){
	plane1/= divide;
	plane2/= divide;
	plane3/= divide;
	plane12/= divide;
	plane22/= divide;
	plane32/= divide;
	plane13/= divide;
	plane23/= divide;
	plane33/= divide;
      }

      //Use nested conditionals instead of if statements
        // ? is a ternary operator(it needs 3 operands) it can be used to replace if-else statement
        //condition ? expression1 : expression2 (it'll pick the first one(exp1))
        //https://www.tutorialspoint.com/cplusplus/cpp_conditional_operator.html

      plane1= plane1< 0 ? 0 : plane1> 255 ? 255 : plane1;
        //Basically if plane is less than zero equal it to zero else if plane is greater than 255 then equal it to 255
      plane2= plane2< 0 ? 0 : plane2> 255 ? 255 : plane2;
      plane3= plane3< 0 ? 0 : plane3> 255 ? 255 : plane3;
      plane12= plane12< 0 ? 0 : plane12> 255 ? 255 : plane12;
      plane22= plane22< 0 ? 0 : plane22> 255 ? 255 : plane22;
      plane32= plane32< 0 ? 0 : plane32> 255 ? 255 : plane32;
      plane13= plane13< 0 ? 0 : plane13> 255 ? 255 : plane13;
      plane23= plane23< 0 ? 0 : plane23> 255 ? 255 : plane23;
      plane33= plane33< 0 ? 0 : plane33> 255 ? 255 : plane33;

      output -> color[0][row][col] = plane1;
      output -> color[1][row][col] = plane2;
      output -> color[2][row][col] = plane3;
      output -> color[0][row][col+1] = plane12;
      output -> color[1][row][col+1] = plane22;
      output -> color[2][row][col+1] = plane32;
      output -> color[0][row+1][col] = plane13;
      output -> color[1][row+1][col] = plane23;
      output -> color[2][row+1][col] = plane33;



        //^ equiavlent to:
        //replace dereferecing
//
//             else if ( output -> color[plane][row][col]  < 0 ) {
//                 output -> color[plane][row][col] = 0;
//             }

//             else if ( output -> color[plane][row][col]  > 255 ) {
//                 output -> color[plane][row][col] = 255;
//             }

    }
  }

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}
