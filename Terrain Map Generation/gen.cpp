#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
//#pragma comment(lib, "libpng")

#include "libraries/lodepng/lodepng.h"

std::vector<unsigned char> convertImage(const char* filname);
int createHeightMap();

int main (int argc, char *argv[])
{
	std::vector<unsigned char> image = convertImage(argv[1]);

	createHeightMap();

	return 1;
}

std::vector<unsigned char> convertImage(const char* filename)
{
	std::vector<unsigned char> image;
	unsigned width, height;

	unsigned errorDec = lodepng::decode(image, width, height, filename);

	if(errorDec) {std::cout << "decoder error " << errorDec << ": " << lodepng_error_text(errorDec) << std::endl; }

	std::vector<unsigned char> imageGrey;

	for ( unsigned i = 0; i < (image.size() / 4); i++)
	{
		int j = 4*i;
		int average = ( image[j] + image[j+1] + image[j+2] ) / 3;
		imageGrey.push_back(average);
		imageGrey.push_back(average);
		imageGrey.push_back(average);
		imageGrey.push_back(255);

		// std::cout << "Average = " << average << std::endl;
	}

	unsigned errorEnc = lodepng::encode("heightmap.png", image, width, height);
	//if there's an error, display it
  			if(errorEnc) { std::cout << "encoder error " << errorEnc << ": "<< lodepng_error_text(errorEnc) << std::endl; }
}

int createHeightMap()
{
	std::vector<unsigned char> image;
	unsigned width, height;

	unsigned errorDec = lodepng::decode(image, width, height, "heightmap.png");

	if(errorDec) {std::cout << "decoder error " << errorDec << ": " << lodepng_error_text(errorDec) << std::endl; }

	std::vector<unsigned> imageOnlyGrey;

	for ( unsigned i = 0; i < imageOnlyGrey.size(); i++)
	{
		std::cout << imageOnlyGrey[i] << std::endl;
	}

	for ( unsigned i = 0; i < (image.size() / 4); i++)
	{
		int j = 4*i;
		int average = ( image[j] + image[j+1] + image[j+2] ) / 3;
		imageOnlyGrey.push_back(average);
		// std::cout << average << std::endl;
		// std::cout << "Average = " << average << std::endl;
		// std::cout << imageOnlyGrey.back() << std::endl;
	}



	std::ofstream outputHeightMap;
	outputHeightMap.open("heightMap.txt");

	int index = 0;

	std::cout << "Height: " << height << " " << "Width: " << width << " " << "vector Length: " << imageOnlyGrey.size() << std::endl; 

	for ( unsigned i = 0; i < height; i++)
	{
		for (unsigned j = 0; j < width; j++)
		{
			index = j + (i * width);
			// std::cout << index << std::endl;
			outputHeightMap << imageOnlyGrey[index] << " ";
		}

		// std::cout << "i: " << i << std::endl;

		outputHeightMap << std::endl;
	}

	return 1;
}