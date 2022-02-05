#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

void processData(char * fileName, float timeInterval);
int countLines(FILE * filep);

int main(int argc, char ** argv) {
	float binSize;
	float cropTime;
	char cropT[3];

	if (argc == 1) {
		printf("\nTo process multiple files simultaneously, please close this program and drag your files onto the icon of this program for batch processing.\n");
		printf("\nTo process single file, please specify the location of your file or drag your file into this window, then press Enter: ");
		char file0[200];
		scanf("%s", file0);
		printf("Please input a time interval (in ps), then press Enter: ");
		scanf("%f", &binSize);
		processData(file0, binSize);
		return 0;
	} // Handling an exception case when no file is drag-n-dropped to this program

	printf("Please input a time interval (in ps), then press Enter: ");
	scanf("%f", &binSize);

	for (int n = 1; n < argc; n++) {
		processData(argv[n], binSize);
	}
	return 0;
}

void processData(char * fileName, float timeInterval) {
	FILE * fp, * fpw;
	int lineCounts, dataPosition = 0, dataCounts = 0;
	char file_Drive[3], file_Dir[100], file_Name[100], file_Ext[6];
	float * dataSet;
	float maxIntensity = 0;
	fp = fopen(fileName, "r");
	if (fp == NULL) {
		printf("Could not open file %s", fileName);
		exit(1);
	}

	lineCounts = countLines(fp); // Count number of lines in the file
	rewind(fp);
	char * line_buffer = malloc(200);
	dataSet = (float *)malloc(sizeof(float) * lineCounts);
	while (fgets(line_buffer, 200, fp) != NULL) {
		if (isdigit(line_buffer[0])) {
			dataSet[dataPosition] = atof(line_buffer);
			dataPosition++;
			dataCounts++; // Count number of bins
		}
	} // Parse data file and store data into dataSet
	fclose(fp);

	int dataCountsTemp = dataCounts;
	while (dataCountsTemp-- >= 0) {
		if (*(dataSet + dataCountsTemp) > maxIntensity) {
			maxIntensity = *(dataSet + dataCountsTemp);
		}
	} // Find max intensity
	dataPosition = 0;
	char writeFile[210];
	_splitpath(fileName, file_Drive, file_Dir, file_Name, file_Ext); // Parse file path and name
	_makepath(writeFile, file_Drive, file_Dir, strcat(file_Name, "_normalized"), ".txt"); // Make destination file path
	fpw = fopen(writeFile, "w");
	if (fpw == NULL) {
		printf("Could not write file %s", writeFile);
		exit(1);
	}
	if (roundf(timeInterval) == timeInterval) { // Check if time interval is an integer
		while (dataPosition < dataCounts) {
			fprintf(fpw, "%d\t%G\n", (dataPosition + 1) * (int)timeInterval, *(dataSet + dataPosition) / maxIntensity);
			dataPosition++;
		} // Convert float type to integer type if interval is integer, and output time axis as integers
	}
	else {
		while (dataPosition < dataCounts) {
			fprintf(fpw, "%G\t%G\n", (dataPosition + 1) * timeInterval, *(dataSet + dataPosition) / maxIntensity);
			dataPosition++;
		} // Output time axis as float point data if interval is not integer
	}
	fclose(fpw);
	free(dataSet);
}

int countLines(FILE * filep) {
	char c;
	int count = 0;
	do {
		c = getc(filep);
		if (c == '\n')
			count++;
	} while (c != EOF);
	return count;
}
