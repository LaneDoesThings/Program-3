#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MB 1000000

int ReadBinaryFile(const char *filename);
bool IsText(const unsigned char *buffer, int datasize);
bool IsTextAlt(const char *filename);

unsigned char *buffer;
size_t buffersize, datasize;

int main(int argc, char *argv[]) {
  int status;
  char *filename;
  bool isUnix;

  buffersize = 5 * MB;
  buffer = (unsigned char *)malloc(buffersize);

    size_t i;
  for (i = 1; i < argc; i++) {
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
        if (strstr((char *)buffer, "\r")) {
          isUnix = false;
        } else {
          isUnix = true;
        }
      }
    }
    /*printf("%s", isUnix ? "true\n" : "false\n");*/
  }
  return EXIT_SUCCESS;
}

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
  if (notText >= 50)
    return false;
  else
    return true;
}

bool IsTextAlt(const char *filename) { return 0; }