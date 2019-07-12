//
// Created by shankarganesh on 12/9/15.
//


#include "Logger.h"
#include "PngFileManager.h"

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers;

uint8_t * PNGFileManager::read_png_file(const char *filename , int &texWidth , int &texHeight) {
    Logger::log(INFO , "Read png" , "Png read Started");
    uint8_t * outputData;

    //string file = "/data/data/com.smackall.animator/files/assets/white_texture.png";

    FILE *fp = fopen(filename, "rb");

    if(!fp)
     return NULL;

    printf("File Name %s ",filename);

    //Logger::log(INFO,"PngFileManager","Texture Name : %s" + file);
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png) return NULL;
    
    png_infop info = png_create_info_struct(png);
    if(!info) return NULL;
    
    if(setjmp(png_jmpbuf(png))) return NULL;
    
    png_init_io(png, fp);
    
    png_read_info(png, info);
    
    width      = png_get_image_width(png, info);
    height     = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth  = png_get_bit_depth(png, info);
    
    texWidth = width;
    texHeight = height;
    
    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt
    
    if(bit_depth == 16)
        png_set_strip_16(png);
    
    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    
    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    
    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    
    
    // These color_type don't have an alpha channel then fill it with 0xff.
    if(color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    
    
    if(color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);
    png_read_update_info(png, info);
    unsigned  long row_bytes = png_get_rowbytes(png,info);
    
    outputData = (uint8_t*) malloc(row_bytes * height * 4);
    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    
    for(int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }
    png_read_image(png, row_pointers);
    for(int i = 0; i < height; i++) {
        
        memcpy(outputData+(row_bytes * i), row_pointers[i], row_bytes);
    }
    png_destroy_info_struct(png,&info);
    
    for(int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);
    fclose(fp);
    Logger::log(INFO , "Read png" , "Png read success");
    return  outputData;
}

void PNGFileManager::write_png_file(char *filename , uint8_t * inputData , int imgWidth , int imgHeight) {
    Logger::log(INFO , "Read png" , "Png Write Started ");
    FILE *fp = fopen(filename, "wb");
    if(!fp) return;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) return;

    png_infop info = png_create_info_struct(png);
    if (!info) return;

    if (setjmp(png_jmpbuf(png))) return;

    png_init_io(png, fp);
    Logger::log(INFO , "Read png" , "Png init");
    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
            png,
            info,
            imgWidth, imgHeight,
            8,
            PNG_COLOR_TYPE_RGBA,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    
    unsigned  long row_bytes = 4 * imgWidth;
    
    png_bytep *write_row_pointers = (png_bytep*)malloc(row_bytes * imgHeight);
    for(int y = 0; y < imgHeight; y++) {
        write_row_pointers[y] = (png_byte*)malloc(row_bytes);
    }
    
    for(int i = 0; i < imgHeight; i++) {
        
        memcpy(write_row_pointers[i], inputData+(row_bytes * i), row_bytes);
    }

    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    //png_set_filler(png, 0, PNG_FILLER_AFTER);

    png_write_image(png, write_row_pointers);
    png_write_end(png, NULL);
    
    png_destroy_info_struct(png,&info);
    for(int y = 0; y < imgHeight; y++) {
        free(write_row_pointers[y]);
    }
    free(write_row_pointers);
    Logger::log(INFO, "IYAN3D.cpp", "Png Write Success");
    fclose(fp);
}

uint8_t* PNGFileManager::getImageDataFromVideo(string fileName, int frame, int &width, int &height){
    return NULL;
}


