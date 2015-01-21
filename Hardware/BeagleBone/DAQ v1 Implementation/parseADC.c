#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

FILE *input;
FILE *output;

uint8_t inputString[16];
int16_t conVal;

int main(int argc, char *argv[]) {
	if (argv[2] == NULL) {
		printf("Invalid input.  Correct usage: parseADC input output\r\n");
	} else {
	  input = fopen(argv[1], "r");
	  if (input == NULL) {
	    printf("Couldn't open %s for reading.\r\n", argv[1]);
		return(1);
	  }
	  output = fopen(argv[2], "w");
	  if (output == NULL) {
		printf("Couldn't open %s for writing.\r\n", argv[2]);
		return(1);
	  }
	}
	
	while(fgets(inputString, sizeof(inputString), input)) {
		inputString[strlen(inputString) - 2] = '\0';
		conVal = strtol(inputString, NULL, 16);
		if (conVal > 0x7fff) {
			conVal -= 0x1000;
		}
		fprintf(output, "%i\r\n", conVal);
	}
	fclose(input);
	fclose(output);
	return(0);
}
