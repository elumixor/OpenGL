//
// Created by vlado on 20-Jul-18.
//

#include <iostream>
#include <cstring>
#include <GL/glew.h>
#include "util.h"

/*
    unsigned myValAsInt = 827611204;

    char asChars[4] = {
            toChar(toInt(FOURCC_DXT1)),
            toChar(toInt(FOURCC_DXT1), 1),
            toChar(toInt(FOURCC_DXT1), 2),
            toChar(toInt(FOURCC_DXT1), 3)
    };

    std::cout << asChars[0] << " "
              << asChars[1] << " "
              << asChars[2] << " "
              << asChars[3] << " "
              << " as int: " << *(int *) &asChars << std::endl;


char toChar(int v, int offset = 0) {
    return *((char *) &v + offset);
}

int toInt(const char* data) {
    return *(unsigned *) data;
}

              */


/**
 *
 * @param imagePath
 * @return
 */
unsigned util::loadDDS(const char *imagePath) {
    unsigned char header[124];

    FILE *fp;

    /* try to open the file */
    fp = fopen(imagePath, "rb");
    if (fp == nullptr) {
        fprintf(stdout, "Couldn't open file.");
        return 0;
    }

    /* verify the type of file */
    char fileCode[4];
    fread(fileCode, 1, 4, fp);
    if (strncmp(fileCode, "DDS ", 4) != 0) {
        fclose(fp);
        fprintf(stdout, "Not a DDS file.");
        return 0;
    }

    /* get the surface desc */
    fread(&header, 124, 1, fp);

    unsigned int height = *(unsigned int *) &(header[8]);
    unsigned int width = *(unsigned int *) &(header[12]);
    unsigned int linearSize = *(unsigned int *) &(header[16]);
    unsigned int mipMapCount = *(unsigned int *) &(header[24]);
    unsigned int fourCC = *(unsigned int *) &(header[80]);

    unsigned char *buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char *) malloc(bufsize * sizeof(unsigned char));
    fread(buffer, 1, bufsize, fp);
    /* close the file pointer */
    fclose(fp);

    unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch (fourCC) {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            free(buffer);
            fprintf(stdout, "Not a vali DXT1 format.");
            return 0;
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
//    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) {
        unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
                               0, size, buffer + offset);

        offset += size;
        width /= 2;
        height /= 2;
    }
    free(buffer);

    return textureID;
}

unsigned util::loadBMP(const char *imagePath) {
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char *data;

    // Open the file
    FILE *file = fopen(imagePath, "rb");
    if (!file) {
        printf("Image could not be opened\n");
        return 0;
    }

    if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
        printf("Not a correct BMP file\n");
        return 0;
    }

    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a correct BMP file\n");
        return 0;
    }

    // Read ints from the byte array
    dataPos = *(int *) &(header[0x0A]);
    imageSize = *(int *) &(header[0x22]);
    width = *(int *) &(header[0x12]);
    height = *(int *) &(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0) imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0) dataPos = 54; // The BMP header is done that way

    // Create a buffer
    data = new unsigned char[imageSize];

    // Read the actual data from the file into the buffer
    fread(data, 1, imageSize, file);

    //Everything is in memory now, the file can be closed
    fclose(file);

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureID;
}

