/* 
label.cpp

Authors: Adam Liber, Andrew Goodney + staff

Description: This is a program converts a bitmap(bmp) image into a 
bitmap image with all the seperate bodies seperated by black space labeled
with randomly selected deifferent colors. This is accomplished using a
 breadth-first-search fill algorithm.

*/


#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "bmplib.h" 
#include "queue.h"
using namespace std;

void usage() { 
    cerr << "usage: ./label <options>" << endl;
    cerr <<"Examples" << endl;
    cerr << "./label test_queue" << endl;
    cerr << "./label gray <input file> <outputfile>" << endl;
    cerr << "./label binary <inputfile> <outputfile>" << endl;
    cerr << "./label segment <inputfile> <outputfile>" << endl;
}

//function prototypes
void test_queue();
void rgb2gray(unsigned char ***in,
unsigned char **out,int height,int width);
void gray2binary(unsigned char **in,unsigned char **out,
int height,int width);
int component_labeling(unsigned char **binary_image,
int **labeled_image,int height,int width);
void label2RGB(int  **labeled_image, unsigned char ***rgb_image,
int num_segment,int height,int width);
void clean(unsigned char ***input,unsigned char **gray,
unsigned char **binary, int **labeled_image,int height , int width);

// main function 
int main(int argc,char **argv) {

    srand( time(0) );
    if(argc < 2 )  {
        usage();
        return -1;
    }        
    unsigned char ***input=0;
    unsigned char **gray=0;
    unsigned char **binary=0;
    int **labeled_image=0;
    if( strcmp("test_queue",argv[1]) == 0 ) { 
        test_queue();
    } 
    else if(strcmp("gray",argv[1]) == 0 ) {
        if(argc <4 ) {
            cerr << "not enough arguemnt for gray" << endl;
            return -1;
        }
        int height,width;
        input = readRGBBMP(argv[2],&height,&width);
        if(input == 0)
        {
            cerr << "unable to open " << argv[2] << " for input." << endl;
            return -1;
        }            
        gray = new unsigned char*[height];
        for(int i=0;i<height;i++) 
            gray[i] = new unsigned char[width];
        rgb2gray(input,gray,height,width);
        if(writeGSBMP(argv[3],gray,height,width) != 0) { 
            cerr << "error writing file " << argv[3] << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }
        clean(input,gray,binary,labeled_image,height,width);

    }
    else if(strcmp("binary",argv[1]) == 0 ) {
        if(argc <4 ) {
            cerr << "not enough arguemnt for binary" << endl;
            return -1;
        }            
        int height,width;
        input = readRGBBMP(argv[2],&height,&width);
        if(input == 0)
        {
            cerr << "unable to open " << argv[2] << " for input." << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }            

        gray = new unsigned char*[height];
        for(int i=0;i<height;i++) 
            gray[i] = new unsigned char[width];
        rgb2gray(input,gray,height,width);

        binary = new unsigned char*[height];
        for(int i=0;i<height;i++)
            binary[i] = new unsigned char[width];

        gray2binary(gray,binary,height,width);
        if(writeBinary(argv[3],binary,height,width) != 0) { 
            cerr << "error writing file " << argv[3] << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }
        clean(input,gray,binary,labeled_image,height,width);
     
    }
    else if(strcmp("segment",argv[1]) == 0 ) {
    
        if(argc <4 ) {
            cerr << "not enough arguemnt for segment" << endl;
            return -1;
        } 
        int height,width;
     
        input = readRGBBMP(argv[2],&height,&width);
        if(input == 0)
        {
            cerr << "unable to open " << argv[2] << " for input." << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }            



        gray = new unsigned char*[height];
        for(int i=0;i<height;i++) 
            gray[i] = new unsigned char[width];
        rgb2gray(input,gray,height,width);

        binary = new unsigned char*[height];
        for(int i=0;i<height;i++)
            binary[i] = new unsigned char[height];

        gray2binary(gray,binary,height,width);


        labeled_image = new int*[height];
        for(int i=0;i<height;i++) labeled_image[i] = new int[width];
        int segments= component_labeling(binary, 
        labeled_image,height,width); 

        for(int i=0;i<height;i++) 
            for(int j=0;j<width;j++) 
                for(int k=0;k<RGB;k++) 
                    input[i][j][k] = 0;
        label2RGB(labeled_image, input ,segments, height,width);
        if(writeRGBBMP(argv[3],input,height,width) != 0) {
            cerr << "error writing file " << argv[3] << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }
        clean(input,gray,binary,labeled_image,height,width);

    }

   return 0;
}

//This function used to test queue implementation. 


void test_queue() { 
    // create locations;
    Location three_one, two_two;
    three_one.row = 3; three_one.col = 1;
    two_two.row = 2; two_two.col = 2;

    //create an Queue with max capacity 5
    Queue q(5);

    cout << boolalpha;
    cout << q.is_empty() << endl;           // true
    q.push(three_one);
    cout << q.is_empty() << endl;           // false
    q.push(two_two);

    Location loc = q.pop();
    cout << loc.row << " " << loc.col << endl; // 3 1
    loc = q.pop();
    cout << loc.row << " " << loc.col << endl; // 2 2
    cout << q.is_empty() << endl;           // true
}

//Loop over the 'in' image array and calculate the single 'out' pixel 
//value using the formula
// GS = 0.2989 * R + 0.5870 * G + 0.1140 * B 
void rgb2gray(unsigned char ***in,unsigned char **out,int height,
int width) {

// multiply all rgb values by grayscale formula and store in 2D array
for(int i = 0 ; i < width; i++){
   for(int j = 0 ; j < height; j++){
      double r = double(in[i][j][0]);
      double g = double(in[i][j][1]);
      double b = double(in[i][j][2]);
      out[i][j] =  char( 0.2989*r + 0.5870*g + 0.1140*b);
       
    
   }
}
   
}

//Loop over the 'in' gray scale array and create 
//a binary (0,1) valued image 'out'
//Set the 'out' pixel to 1 if 'in' is above the threshold, else 0
void gray2binary(unsigned char **in,unsigned char **out,int height,
int width) {


// loop through bitmap compare each pixel with threshold and set to 1 or 0
for(int i = 0 ; i < width; i++){
   for(int j = 0 ; j < height; j++){
      if (in[i][j] > THRESHOLD){
         out[i][j] = 1;
      }
      else{
         out[i][j] = 0;
      }
    
   }
}
    
    
}

//This is the function loops over the binary image 
//and doing the connected component labeling
int component_labeling(unsigned char **binary_image,int **label,int height,
int width) {

int maxlength = height*width;

for(int i = 0; i < height; i++){ // initialize label 
   for(int j = 0; j < width; j++){
      label[i][j] = 0;
   }
}
// create array to mark what has been queued already
bool **queued = new bool*[height];
for(int i=0;i<height;i++){
       queued[i] = new bool[width];
       }
            
 for(int i = 0 ; i < height; i++){// set all pixels to not queued
      for(int j = 0 ; j < width; j++){
         queued[i][j] = false;
  }    
} 
 

int current_label = 1; // set label
 for(int i = 0 ; i < height; i++){//loop through entire b/w image
      for(int j = 0 ; j < width; j++){
      
        //  cout << "bw image" << endl;
         if (binary_image[i][j] == 1 && label[i][j] == 0){ // detect white
            
               Location cell;
               cell.row = i;
               cell.col = j;
               Queue segment(maxlength);
               segment.push(cell);
               queued[i][j] = true;
               
            //  cout << "found one" << endl;
                  while(!segment.is_empty()){ // begin bfs
                  
                     cell =  segment.pop();
                     label[cell.row][cell.col] = current_label;
                     queued[cell.row][cell.col] = true;
                    // cout << cell.row << " " << cell.col <<endl; // test
                     
                 //   cout << "label pixel" << endl;
                    // create array of adjacent pixels
                     Location north, east, south, west, loc;
                     Location *adjacent = new Location[4]; 
                    
                        north.row = cell.row+1; north.col = cell.col; 
                           adjacent[0] = north;
                        east.row = cell.row; east.col = cell.col+1; 
                           adjacent[1] = east;
                        south.row = cell.row-1; south.col = cell.col; 
                           adjacent[2] = south;
                        west.row = cell.row; west.col = cell.col-1; 
                           adjacent[3] = west;
                     
                     //loop and check adjacent pixels
                     for(int k = 0; k < 4; k++){
                         //cout << "check adjacent" << endl;
                         loc = adjacent[k];
                           if(loc.row < 256 && loc.col < 256 
                           && loc.row > -1 && loc.col > -1){//edge cases 
                           if(binary_image[loc.row][loc.col] == 1 
                           && label[loc.row][loc.col] == 0 
                           && !queued[loc.row][loc.col]){
                         //  cout << "label adjacent" << endl;
                              segment.push(loc);// add to que
                              queued[loc.row][loc.col] = true;
                   
                            }
                            }
                     }
  
                     delete [] adjacent;
                                     
                 }
         current_label++;//increment label
          }
         
      } 
    }
    //cout << "bw image done" << endl;
    
for(int i=0;i<height;i++){
       delete [] queued[i];
       }
       delete [] queued;
   
// cout << current_label << endl;

 return (current_label-1);


 }


#ifndef AUTOTEST
void label2RGB(int  **labeled_image, unsigned char ***rgb_image,
int num_segments,int height,int width)
{

srand(time(0));




for(int i = 1 ; i< num_segments+1; i++){
unsigned char r = 0, g = 0, b = 0;
   r = rand()%255 + 1;
   g = rand()%255 + 1;
   b = rand()%255 + 1;
   
   for(int j = 0; j < height; j++){
      for(int k = 0 ; k < width; k++){
      //cout << "label to rgb image" << endl; // test
         if(labeled_image[j][k] == i){
            rgb_image[j][k][0] = r;
            rgb_image[j][k][1] = g;
            rgb_image[j][k][2] = b;
         }
         
      }
   }
}

   
}
#endif

//this function deletes all of the dynamic arrays created
void clean(unsigned char ***input,unsigned char **gray,
unsigned char **binary, int **labeled_image,int height , int width) {
    if(input ) {
            for(int i=0;i<height;i++) {
               for(int j = 0; j < width; j++){
                delete [] input[i][j];
                }
            }
            for(int i = 0; i<height; i++){
               delete [] input[i];
            }
            delete [] input;
            //delete allocated input image array 
        } 
        if(gray) {
          
            for(int i=0;i<height;i++) {
                delete [] gray[i];
            }
            delete [] gray;
          
            //delete gray-scale image 
        }
        if(binary) {
         
              for(int i=0;i<height;i++) {
                delete [] binary[i];
            }
            delete [] binary;
        
            //delete binary image array 
        }
        if(labeled_image) {
             for(int i = 0; i<height; i++){
               delete [] labeled_image[i];
            }
            delete [] labeled_image;
               
            //delete labeled array 
        }


}
