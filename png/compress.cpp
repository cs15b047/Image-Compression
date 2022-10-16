#include "compress.h"

using namespace std;

unsigned long file_size(char* filename) {
    FILE *pFile = fopen(filename, "rb");
    fseek (pFile, 0, SEEK_END);
    unsigned long size = ftell(pFile);
    fclose (pFile);
    return size;
}

void decompress(string filename) {
    string compressed_filename = filename + ".z";
    char* infilename = (char*)(compressed_filename).c_str();
    char* outfilename = (char*)filename.c_str();
    gzFile infile = gzopen(infilename, "rb");
    FILE *outfile = fopen(outfilename, "wb");

    char buffer[128];
    int num_read = 0;
    while ((num_read = gzread(infile, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, num_read, outfile);
    }

    gzclose(infile);
    fclose(outfile);
}

void compress(string filename) {
    string compressed_filename = filename + ".z";
    char* outfilename = (char*)(compressed_filename).c_str();
    char* infilename = (char*)filename.c_str();
    FILE *infile = fopen(infilename, "rb");
    gzFile outfile = gzopen(outfilename, "wb");
    char inbuffer[128];
    int num_read = 0;
    unsigned long total_read = 0, total_wrote = 0;
    while ((num_read = fread(inbuffer, 1, sizeof(inbuffer), infile)) > 0) {
        total_read += num_read;
        gzwrite(outfile, inbuffer, num_read);
    }
    fclose(infile);
    gzclose(outfile);

    printf("Read %ld bytes, Wrote %ld bytes, Compression factor %4.2f\n", total_read, file_size(outfilename), 
            (1.0-file_size(outfilename)*1.0/total_read)*100.0);
}