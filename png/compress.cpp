#include "compress.h"
#include <vector>
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

void compress(vector<char> buffer, string filename) {
    int size = buffer.size();
    string compressed_filename = filename + ".z";
    char* outfilename = (char*)(compressed_filename).c_str();
    gzFile outfile = gzopen(outfilename, "wb");
    gzwrite(outfile, buffer.data(), size);
    gzclose(outfile);
}

vector<char> decompress1(string filename) {
    string compressed_filename = filename + ".z";
    char* infilename = (char*)(compressed_filename).c_str();
    gzFile infile = gzopen(infilename, "rb");

    int chunk_size = 5000000;
    char* tmp_buffer = (char*)malloc(chunk_size);
    int num_read = 0;
    vector<char> buffer;
    while ((num_read = gzread(infile, tmp_buffer, chunk_size)) > 0) {
        buffer.insert(buffer.end(), tmp_buffer, tmp_buffer + num_read);
    }
    
    gzclose(infile);
    free(tmp_buffer);

    return buffer;
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