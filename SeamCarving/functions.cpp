#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

Pixel** createImage(int width, int height) {
  cout << "Start createImage... " << endl;
  
  // Create a one dimensional array on the heap of pointers to Pixels 
  //    that has width elements (i.e. the number of columns)
  Pixel** image = new Pixel*[width];
  
  bool fail = false;
  
  for (int i=0; i < width; ++i) { // loop through each column
    // assign that column to a one dimensional array on the heap of Pixels
    //  that has height elements (i.e. the number of rows)
    image[i] = new Pixel[height];
    
    if (image[i] == nullptr) { // failed to allocate
      fail = true;
    }
  }
  
  if (fail) { // if any allocation fails, clean up and avoid memory leak
    // deallocate any arrays created in for loop
    for (int i=0; i < width; ++i) {
      delete [] image[i]; // deleting nullptr is not a problem
    }
    delete [] image; // dlete array of pointers
    return nullptr;
  }
  
  // initialize cells
  //cout << "Initializing cells..." << endl;
  for (int row=0; row<height; ++row) {
    for (int col=0; col<width; ++col) {
      //cout << "(" << col << ", " << row << ")" << endl;
      image[col][row] = { 0, 0, 0 };
    }
  }
  cout << "End createImage... " << endl;
  return image;
}

void deleteImage(Pixel** image, int width) {
  cout << "Start deleteImage..." << endl;
  // avoid memory leak by deleting the array
  for (int i=0; i<width; ++i) {
    delete [] image[i]; // delete each individual array placed on the heap
  }
  delete [] image;
  image = nullptr;
}

int* createSeam(int length) {
	int* point = new int[length];
	for(int index = 0; index < length; index++){
		point[index] = 0;
	}
	return point;
}

void deleteSeam(int* seam) {
	delete[] seam;
	seam = nullptr;
	
}

bool loadImage(string filename, Pixel** image, int width, int height) {
	ifstream ifs (filename);
	
	//check if file opened correctly
	if(!ifs.is_open()){
		cout << "Error: failed to open input file - " << filename << endl;
		return false;
	}
	
	//check is right kind of file
	char type[3];
	ifs >> type; // should be P3
	if ((toupper(type[0]) != 'P') || (type[1] != '3')) { // check that type is correct
		cout << "Error: type is " << type << " instead of P3" << endl;
		return false;
	}
	
	//check if width and height match file
	int w = 0;
	int h = 0;
	ifs >> w >> h;
	if(!ifs.good()){
		cout << "Error: read non-integer value" << endl;
		return false;
	}
	if (w != width) { // check that width matches what was passed into the function
		cout << "Error: input width (" << width << ")";
		cout << " does not match value in file (" << w << ")" << endl;
		return false;
	  }
	if (h != height) { // check that height matches what was passed into the function
		cout << "Error: input height (" << height << ")";
		cout << " does not match value in file (" << h << ")" << endl;
		return false;
	}
	
	//CHECK max colo is 255
	int colorMax = 0;
	ifs >> colorMax;
	if(!ifs.good()){
		cout << "Error: read non-integer value" << endl;
		return false;
	}
	if (colorMax != 255) {
		cout << "Error: file is not using RGB color values." << endl;
		return false;
	}
	for(int row = 0; row < height; row++){
		for(int col = 0; col < width; ++col){
			
			ifs >> image[col][row].r;
			
			if(ifs.fail()){
				if(ifs.eof()){
					cout << "Error: not enough color values" << endl;
					return false;
				}
				cout << "Error: read non-integer value" <<endl;
				return false;
			}
			if(image[col][row].r > 255 || (image[col][row].r < 0)){
				cout << "Error: invalid color value " << image[col][row].r << endl;
				return false;
			}
			
			ifs >> image[col][row].g;
			
			if(ifs.fail()){
				if(ifs.eof()){
					cout << "Error: not enough color values" << endl;
					return false;
				}
				cout << "Error: read non-integer value" <<endl;
				return false;
			}
			if(image[col][row].g > 255 || (image[col][row].g < 0)){
				cout << "Error: invalid color value " << image[col][row].g << endl;
				return false;
			}
			
			ifs >> image[col][row].b;
			if(ifs.eof()){
				if(col != width-1){
					cout << "Error: not enough color values" << endl;
					return false;
				}
			}
			if(ifs.fail()){
				if(ifs.eof()){
					cout << "Error: not enough color values" << endl;
					return false;
				}
				cout << "Error: read non-integer value" <<endl;
				return false;
			}
			if(image[col][row].b > 255 || (image[col][row].b < 0)){
				cout << "Error: invalid color value " << image[col][row].b << endl;
				return false;
			}	
		}
	}
	if(!ifs.eof()){
		int check = 0;
		ifs >> check;
		if(ifs.fail()){
			ifs.close();
			return true;
		}
		cout << "Error: too many color values" << endl;
		return false;
	}
	ifs.close();
	return true;
}

bool outputImage(string filename, Pixel** image, int width, int height) {
	ofstream ofs (filename);
	
	if (!ofs.is_open()) {
		cout << "Error: failed to open output file " << filename << endl;
		return false;
	}
	
	ofs << "P3" << endl;
	ofs << width << " " << height << endl;
	ofs << 255 << endl;
	
	for(int row = 0; row < height; row++){
		for(int col = 0; col < width; col++){
			ofs << image[col][row].r << " ";
			ofs << image[col][row].g << " ";
			ofs << image[col][row].b << " ";
		}
		ofs << endl;
	}
	return true;
}

int energy(Pixel** image, int x, int y, int width, int height) {
	int value;
	int xSum = 0;
	int ySum = 0;
	if(width == 1){
		xSum = 0;
	}else if(x == 0){
		int redx = pow((image[x+1][y].r - image[width-1][y].r),2);
		int greenx = pow((image[x+1][y].g - image[width-1][y].g),2);
		int bluex = pow((image[x+1][y].b - image[width-1][y].b),2);			
		xSum = redx+greenx+bluex;
	}else if(x == (width - 1)){
		int redx = pow((image[0][y].r - image[x-1][y].r),2);
		int greenx = pow((image[0][y].g - image[x-1][y].g),2);
		int bluex = pow((image[0][y].b - image[x-1][y].b),2);			
		xSum = redx+greenx+bluex;
	}else{
		int redx = pow((image[x+1][y].r - image[x-1][y].r),2);
		int greenx = pow((image[x+1][y].g - image[x-1][y].g),2);
		int bluex = pow((image[x+1][y].b - image[x-1][y].b),2);	
		xSum = redx+greenx+bluex;
	}
	if(height == 1){
		ySum = 0;
	}else if(y == 0){
		int redy = pow((image[x][y+1].r - image[x][height-1].r),2);
		int greeny = pow((image[x][y+1].g - image[x][height-1].g),2);
		int bluey = pow((image[x][y+1].b - image[x][height-1].b),2);
		ySum = redy+greeny+bluey;	
	}else if(y == (height - 1)){
		int redy = pow((image[x][0].r - image[x][y-1].r),2);
		int greeny = pow((image[x][0].g - image[x][y-1].g),2);
		int bluey = pow((image[x][0].b - image[x][y-1].b),2);
		ySum = redy+greeny+bluey;
	}else{
		int redy = pow((image[x][y+1].r - image[x][y-1].r),2);
		int greeny = pow((image[x][y+1].g - image[x][y-1].g),2);
		int bluey = pow((image[x][y+1].b - image[x][y-1].b),2);
		ySum = redy+greeny+bluey;
	}
		
	value = xSum+ySum;	

	return value;
}

int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int* seam) {
	int totEnergy = energy(image, start_col, 0, width, height);
	int col = start_col;
	seam[0] = start_col;
	int energyR;
	int energyL;
	for(int i = 1; i < height; i++){
		int pixMin = energy(image, col, i, width, height);
		if(col == 0){
			energyR = energy(image, col + 1, i, width, height);
			if(energyR < pixMin){
				pixMin = energyR;
				col++;
			}
		}else if(col == (width - 1)){
			energyL = energy(image, col - 1, i, width, height);
			if(energyL < pixMin){
				pixMin = energyL;
				col--;
			}
		}else{
			energyR = energy(image, col + 1, i, width, height);
			energyL = energy(image, col - 1, i, width, height);
			if(energyR <= energyL){
				if(energyR < pixMin){
					pixMin = energyR;
					col++;
				}
			}else{
				if(energyL < pixMin){
					pixMin = energyL;
					col--;
				}
			}
		}
		totEnergy += pixMin;
		seam[i] = col;
	}
	
	return totEnergy;
}

int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam) {
	int totEnergy = energy(image, 0, start_row, width, height);
	int row = start_row;
	seam[0] = start_row;
	int energyU;
	int energyD;
	for(int i = 1; i < width; i++){
		int pixMin = energy(image, i, row, width, height);
		if(row == 0){
			energyD = energy(image, i, row + 1, width, height);
			if(energyD < pixMin){
				pixMin = energyD;
				row++;
			}
		}else if(row == (height - 1)){
			energyU = energy(image, i, row - 1, width, height);
			if(energyU < pixMin){
				pixMin = energyU;
				row--;
			}
		}else{
			energyD = energy(image, i, row + 1, width, height);
			energyU = energy(image, i, row - 1, width, height);
			if(energyD < energyU){
				if(energyD < pixMin){
					pixMin = energyD;
					row++;
				}
			}else{
				if(energyU < pixMin){
					pixMin = energyU;
					row--;
				}
			}
		}
		totEnergy += pixMin;
		seam[i] = row;
	}
		
	return totEnergy;
}

int* findMinVerticalSeam(Pixel** image, int width, int height) {
	int* minSeam = createSeam(height);
	int minEnergy = loadVerticalSeam(image, 0, width, height, minSeam);
	for(int i = 1; i < width; i++){
		int* checkSeam = createSeam(height);
		int checkEnergy = loadVerticalSeam(image, i, width, height, checkSeam);
		if(checkEnergy < minEnergy){
			for(int i = 0; i < height; i++){
				minSeam[i] = checkSeam[i];
			}
			minEnergy = checkEnergy;
		}
		deleteSeam(checkSeam);
		
	}
	return minSeam;
}

int* findMinHorizontalSeam(Pixel** image, int width, int height) {
	int* minSeam = createSeam(width);
	int minEnergy = loadHorizontalSeam(image, 0, width, height, minSeam);
	for(int i = 1; i < height; i++){
		int* checkSeam = createSeam(width);
		int checkEnergy = loadVerticalSeam(image, i, width, height, checkSeam);
		if(checkEnergy < minEnergy){
			for(int i = 0; i < width; i++){
				minSeam[i] = checkSeam[i];
			}
			minEnergy = checkEnergy;
		}
		deleteSeam(checkSeam);
		
	}
	return minSeam;
}

void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam) {
	for(int i = 0; i < height; i++){
		int col = verticalSeam[i];
		for(int j = col; j < (width - 1); j++){
			image[j][i] = image[j+1][i];
		}
	}
}

void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam) {
	for(int i = 0; i < width; i++){
		int row = horizontalSeam[i];
		for(int j = row; j < (height - 1); j++){
			image[i][j] = image[i][j+1];
		}
	}
}

