#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MB 1000000

int ReadBinaryFile(const char *filename);
bool IsText(const unsigned char *buffer, int datasize);
void ToUnix(const char *buffer, const char *filename);

unsigned char *buffer;
size_t buffersize, datasize;

int main(int argc, char *argv[]) {
  int status, optionIndex = -1;
  char *filename;
  bool isUnix, convert = false;

  buffersize = 5 * MB; /*5 megabytes*/
  buffer = (unsigned char *)malloc(buffersize);

  /*Loop through the input and if -u is given tell to convert and skip the input
    with the -u*/
  size_t i;
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-u") == 0) {
      convert = true;
      optionIndex = i;
    }
  }

  /*Loop through all the given files and perform the checks/conversions*/
  for (i = 1; i < argc; i++) {
    if (i == optionIndex)
      continue;
    filename = argv[i];
    status = ReadBinaryFile(filename);

    if (status > 0) {
      switch (status) {
      case 1:
        fprintf(stderr, "Could not open file!\n");
        break;
      case 2:
        fprintf(stderr, "File is zero length!\n");
        break;
      case 3:
        fprintf(stderr, "File is too big for the buffer!\n");
        break;
      case 4:
        fprintf(stderr, "Error reading file!\n");
      default:
      }
    } else {
      if (!IsText(buffer, datasize)) {
        fprintf(stderr, "%s, is not a text file!\n", filename);
      } else {

        printf("%s\n", filename);

        if (strstr((char *)buffer, "\r")) {
          isUnix = false;
          if (convert) {
            ToUnix(buffer, filename);
          }
        } else {
          isUnix = true;
        }
      }
    }
    /*printf("%s", isUnix ? "true\n" : "false\n");*/
  }
  return EXIT_SUCCESS;
}

/*Reads the file at filename and places it into buffer*/
int ReadBinaryFile(const char *filename) {
  int status = 0;
  int bytesInFile = 0;
  FILE *ifs = 0;

  ifs = fopen(filename, "rb");
  if (!ifs) {
    status = 1;
  } else { /* make sure file will fit into buffer */
    fseek(ifs, 0, SEEK_END);
    bytesInFile = (int)ftell(ifs);
    fseek(ifs, 0, SEEK_SET);

    if (bytesInFile == 0) {
      status = 2;
    } else {
      if (bytesInFile >= (buffersize - 1)) {
        status = 3;
      } else {
        datasize = (int)fread(buffer, 1, bytesInFile, ifs);
        if (datasize != bytesInFile) {
          status = 4;
        } else {
          /* if we read a text file then this */
          /* turns buffer into a "C" string */
          buffer[datasize] = '\0';
        }
      }
    }
  }
  if (ifs)
    fclose(ifs);
  return status;
}

/*Checks if the buffer contains text or a binary file*/
bool IsText(const unsigned char *buffer, int datasize) {
  size_t i, toCheck, notText = 0;
  char c;
  toCheck = datasize < 100 ? datasize : 100;

  for (i = 0; i < toCheck; i++) {
    c = buffer[i];
    if (!isprint(c) && !isspace(c))
      notText++;
    else {
      switch (c) {
      case '\f':
      case '\t':
      case '\r':
      case '\n':
      case '\a':
      case '\b':
      case '\v':
        notText++;
        break;
      }
    }
  }
  if (notText >= 50) /*If 50% or more of the checked characters aren't text the
                        file probably isn't text*/
    return false;
  else
    return true;
}

/*Converts windows text file to unix and writes it back to the file system*/
void ToUnix(const char *buffer, const char *filename) {
  FILE *ofs;
  ofs = fopen(filename, "wb");
  if (!ofs) {
    fprintf(stderr, "Cannot open %s for writing\n", filename);
    return;
  }

  size_t i;
  for (i = 0; i < datasize; i++) {
    if (buffer[i] == '\r') {
      continue;
    }
    fputc(buffer[i], ofs);
  }

  if (ofs)
    fclose(ofs);
}