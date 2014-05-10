/*
*  FileIO.c
*  Reads input file and prints each word on a separate line of
*  the output file.
*/

#include<stdio.h>
#include<stdlib.h>

int main(int argc, char * argv[]){
   FILE *in, *out; /* handles for input and output files */
   char word[256]; /* char array to store words from input file */
   char *token;

   /* check command line for correct number of arguments */
   if( argc != 3 ){
      printf("Usage: %s inputFile outputFile\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   /* open input file for reading */
   in = fopen(argv[1], "r");
   if( in==NULL ){
      printf("Unable to read from file %s\n", argv[1]);
      exit(EXIT_FAILURE);
   }

   /* open output file for writing */
   out = fopen(argv[2], "w");
   if( out==NULL ){
      printf("Unable to write to file %s\n", argv[2]);
      exit(EXIT_FAILURE);
   }

   /* read each word in input file, print words on separate lines to output file */
   while(fscanf(in, "%s", word)!=EOF){
      fprintf(out, "%s\n", word);

      token = strtok (word, ","); 
      while (token != NULL)
      {
        printf("%i,",token);
        token = strtok (NULL, ",");
      }
      printf("\n");
   }

   /* close input and output files */
   fclose(in);
   fclose(out);

   return(EXIT_SUCCESS);
}

