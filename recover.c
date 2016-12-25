/****************************************************************************
 * recover.c
 *
 *
 * Recovers JPEGs from a forensic image.
 ***************************************************************************/
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdint.h>
 #include <stdbool.h>
 #include <string.h>
 
 typedef uint8_t BYTE;
 
 #define BLOCKSIZE 512
 
 int main(int argc, char* argv[])
 {
     // check the command
     if (argc != 1)
     {
         printf("Usage: no command line arguements needed\n");
         return 1;
     }
     
     FILE* fp = fopen("card.raw", "r");
     
     // check the file availability
     if (fp == NULL)
     {
         printf("%s cannot open\n", argv[1]);
         return 1;
     }
     
     // define a buffer with 512 bytes
     BYTE buffer[BLOCKSIZE];
     
     // define a temp output file
     FILE* outfile = NULL;
     
     // define the jpg count and jpg status
     int pcount = 0;
     bool keep_search = true;
     
     // check through the file
     while (keep_search)
     {
         // read the fp into buffer
         for (int i = 0; i < BLOCKSIZE; i++)
         {
             if (feof(fp))
             {
                 fclose(fp);
                 
                 if (outfile != NULL)
                     fclose(outfile);
                     
                 keep_search = false;
                 
                 // summary of the recover process
                 printf("JPG found: %d\n", pcount);
                 
                 return 0;
             }
             
             fread(&buffer[i], sizeof(BYTE), 1, fp);
         }
         
         // start of a new jpg 0xff 0xd8 0xff 0xe0 || 0xff 0xd8 0xff 0xe1         
         if ((buffer[0] == 0xff) && (buffer[1] == 0xd8) && (buffer[2] == 0xff) && (buffer[3] == 0xe0 || buffer[3] == 0xe1))
         {
             // record the picture found
             pcount++;
             
             // check if there's jpg found before
             if (outfile != NULL)
                 fclose(outfile);
             
             // creat a new jpg file with count, count from 0
             char title[4];
             sprintf(title, "%03d.jpg", pcount - 1);
              
             // point the outfile to the newly ceated file
             outfile = fopen(title, "w");
                 
             // check if the file can be written
             if(outfile == NULL)
             {
                 printf("Could not write the image.\n");
                 return 1;
             }
             
             // write the buffer into the file
             fwrite(&buffer[0], BLOCKSIZE * sizeof(BYTE), 1, outfile);
         }
         else if (outfile != NULL)
         {                 
             // copy the continued data in buffer
             fwrite(&buffer[0], BLOCKSIZE * sizeof(BYTE), 1, outfile);
         }
     }
 }
