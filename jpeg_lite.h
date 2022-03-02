/*
jpeg_io.h: A simple JPEG IO utility.
Only supports Huffman encoded baseline DCT JPEGs.
Progressive, arithmetic JPEGs are not supported.
*/
#pragma once

#include "basedefs.h"
#include "linalg.h"
#include "huffman.h"

/*
8x8 data block storage: used in JPEG (JFIF) encoding/decoding process.
-------------------------------------
d[0][0]  d[0][1]    ...    d[0][7]
d[1][0]  d[1][1]    ...    d[1][7]
  ...      ...      ...      ...
d[7][0]  d[7][1]    ...    d[7][7]
-------------------------------------
d[0][0]: DC coefficient
d[0][1]~d[7][7]: AC coefficient
*/
struct REAL_8x8 { REAL data[8][8]; };
struct  INT_8x8 { int  data[8][8]; };
struct BYTE_8x8 { BYTE data[8][8]; };

INT_8x8 fill_int8x8(const char* s);
REAL_8x8 fill_real8x8(const char* s);
void print_int8x8(INT_8x8* a);
void print_real8x8(REAL_8x8* a);

/* image stored with uncompressed array, used as internal format */
struct RAW_IMAGE
{
    int w, h; /* image width and height */
    unsigned char* r; /* red channel */
    unsigned char* g; /* green channel */
    unsigned char* b; /* blue channel */
};

/* create an image with specified width and height */
bool alloc_image( /* in */ int w, int h, /* out */ RAW_IMAGE** image);

/* destroy an image */
bool free_image(RAW_IMAGE* image);

/* save raw image into PPM (for debugging purpose) */
bool save_PPM(RAW_IMAGE* data, const char* file);

/* * * * * * * * * * * * * * * * */
/* 2D discrete cosine transform  */
/* * * * * * * * * * * * * * * * */
/* https://arxiv.org/ftp/arxiv/papers/1405/1405.6147.pdf, Eq (5) */
REAL_8x8 DCT8x8(REAL_8x8* f);
/* https://www.nayuki.io/res/fast-discrete-cosine-transform-algorithms/fast-dct-8.c */
void DCT8_fast(REAL v[8]); /* inplace DCT-8 */
REAL_8x8* DCT8x8_fast(REAL_8x8 * f); /* inplace 8x8 fast 2D DCT, returns itself */

/* * * * * * * * * * * * * * * * * * * * */
/* inverse 2D discrete cosine transform  */
/* * * * * * * * * * * * * * * * * * * * */
/* https://arxiv.org/ftp/arxiv/papers/1405/1405.6147.pdf, Eq (6) */
REAL_8x8 IDCT8x8(REAL_8x8* F);
/* https://www.nayuki.io/page/fast-discrete-cosine-transform-algorithms */
/* https://www.nayuki.io/res/fast-discrete-cosine-transform-algorithms/fast-dct-8.c */
void IDCT8_fast(REAL v[8]); /* inplace IDCT-8 */
REAL_8x8* IDCT8x8_fast(REAL_8x8* F); /* inplace 8x8 fast 2D IDCT, returns itself */


/*
JPEG uses "markers" to tell what type of data that is coming next, they 
always start with "0xFF" and the second byte indicates the data type.
*/
/*         Marker Value    Name                       Description                   */
extern const BYTE  SOF0; /* Start of frame 0,          baseline DCT                  */
extern const BYTE  SOF1; /* Start of frame 1,          extended sequential DCT       */
extern const BYTE  SOF2; /* Start of frame 2,          progressive DCT               */
extern const BYTE  SOF3; /* Start of frame 3,          loseless (sequential)         */
extern const BYTE   DHT; /* Define Huffman table */
extern const BYTE  SOF5; /* Start of frame 5,          differential sequantial DCT   */
extern const BYTE  SOF6; /* Start of frame 6,          differential progressive DCT  */
extern const BYTE  SOF7; /* Start of frame 7,          differential loseless (sequential) */
extern const BYTE   JPG; /* JPEG extensions */
extern const BYTE  SOF9; /* Start of frame 9,          extended sequential DCT, arithmetic coding */
extern const BYTE SOF10; /* Start of frame 10,         progressive DCT, arithmetic coding */
extern const BYTE SOF11; /* Start of frame 11,         lossless (sequential), arithmetic coding */
extern const BYTE   DAC; /* Define Arithmetic Coding                                 */
extern const BYTE SOF13; /* Start of frame 13,         differential sequential DCT, arithmetic coding */
extern const BYTE SOF14; /* Start of frame 14,         differential progressive DCT, arithmetic coding */
extern const BYTE SOF15; /* Start of frame 15,         differential lossless (sequential), arithmetic coding */
extern const BYTE  RST0; /* Restart Marker 0 */
extern const BYTE  RST1; /* Restart Marker 1 */
extern const BYTE  RST2; /* Restart Marker 2 */
extern const BYTE  RST3; /* Restart Marker 3 */
extern const BYTE  RST4; /* Restart Marker 4 */
extern const BYTE  RST5; /* Restart Marker 5 */
extern const BYTE  RST6; /* Restart Marker 6 */
extern const BYTE  RST7; /* Restart Marker 7 */
extern const BYTE   SOI; /* Start of Image */
extern const BYTE   EOI; /* End of Image */
extern const BYTE   SOS; /* Start of Scan */
extern const BYTE   DQT; /* Define Quantization Table */
extern const BYTE   DNL; /* Define Number of Lines,     (Not common)                  */
extern const BYTE   DRI; /* Define Restart Interval */
extern const BYTE   DHP; /* Define Hierarchical Progression, (Not common)             */
extern const BYTE   EXP; /* Expand Reference Component, (Not common)                  */
extern const BYTE  APP0; /* Application Segment 0,     JFIF - JFIF JPEG image
                                                      AVI1 - Motion JPEG (MJPG)     */
extern const BYTE  APP1; /* Application Segment 1,     EXIF Metadata, TIFF IFD format,
                                                      JPEG Thumbnail (160¡Á120)
                                                      Adobe XMP                     */
extern const BYTE  APP2; /* Application Segment 2,     ICC color profile,
                                                      FlashPix                      */
extern const BYTE  APP3; /* Application Segment 3,     (Not common)
                                                      JPS Tag for Stereoscopic JPEG images */
extern const BYTE  APP4; /* Application Segment 4,     (Not common)                  */
extern const BYTE  APP5; /* Application Segment 5,     (Not common)                  */
extern const BYTE  APP6; /* Application Segment 6,     (Not common)
                                                      NITF Lossles profile          */
extern const BYTE  APP7; /* Application Segment 7,     (Not common)                  */
extern const BYTE  APP8; /* Application Segment 8,     (Not common)                  */
extern const BYTE  APP9; /* Application Segment 9,     (Not common)                  */
extern const BYTE APP10; /* Application Segment 10,   (Not common)
                            PhoTags,                  ActiveObject (multimedia messages / captions) */
extern const BYTE APP11; /* Application Segment 11,   (Not common)
                                                      HELIOS JPEG Resources (OPI Postscript) */
extern const BYTE APP12; /* Application Segment 12,   Picture Info (older digicams),
                                                      Photoshop Save for Web: Ducky */
extern const BYTE APP13; /* Application Segment 13,   Photoshop Save As: IRB, 8BIM, IPTC */
extern const BYTE APP14; /* Application Segment 14,   (Not common)                  */
extern const BYTE APP15; /* Application Segment 15,   (Not common)                  */
extern const BYTE  JPG0; /* JPEG Extension 0~6,       (Not common)                  */
extern const BYTE  JPG1;
extern const BYTE  JPG2;
extern const BYTE  JPG3;
extern const BYTE  JPG4;
extern const BYTE  JPG5;
extern const BYTE  JPG6;
extern const BYTE  JPG7, SOF48;
/*                          JPEG Extension 7,         Loseless JPEG
                            JPEG-LS                                                 */
extern const BYTE JPG8, LSE;
/*                          JPEG Extension 8,         Lossless JPEG Extension Parameters
                            JPEG-LS Extension                                       */
extern const BYTE  JPG9; /* JPEG Extension 9,          (Not common)                  */
extern const BYTE JPG10; /* JPEG Extension 10,        (Not common)                  */
extern const BYTE JPG11; /* JPEG Extension 11,        (Not common)                  */
extern const BYTE JPG12; /* JPEG Extension 12,        (Not common)                  */
extern const BYTE JPG13; /* JPEG Extension 13,        (Not common)                  */
extern const BYTE   COM; /* Comment */
extern const BYTE   TEM; /* One byte, just skip */

struct JPEG_CHANNEL {
    int hsample; /* horizontal sampling factor */
    int vsample; /* vertical sampling factor */
    int qtab_id; /* which quantization table will be used when encode/decode this channel */
    int dctab_id; /* which Huffman table will be used when encode/decode the DC coefficients of this channel */
    int actab_id; /* which Huffman table will be used when encode/decode the AC coefficients of this channel */
    bool is_used;
};

struct JPEG_HUFFMAN_TABLE {
    BYTE offsets[17];
    BYTE symbols[176];       /* Huffman symbols (nZ, length) */
    unsigned int codes[176]; /* binary Huffman codes */
    bool is_used;
};

#define _JPEG_MSG_LEN        256

struct JPEG_FILE {

    /* most useful information */
    bool is_valid;                 /* is JPEG file valid */
    int image_width, image_height; /* image width and height measured in pixels */
    int num_channels;              /* number of color channels */
    RAW_IMAGE* image_data;         /* decoded raw image data (RGB, 8 bits per channel) */
    char message[_JPEG_MSG_LEN];   /* JPEG loading message, stores error string */

    /* advanced information */
    bool zero_start;               /* is channel ID starts with 0 instead of 1 (default=false) */
    INT_8x8 qtabs[4];              /* quantization tables (4 at maximum) */
    JPEG_HUFFMAN_TABLE dctabs[4];  /* Huffman DC tables */
    JPEG_HUFFMAN_TABLE actabs[4];  /* Huffman AC tables */
    int restart_interval;          /* DC coefficient restart interval */
    JPEG_CHANNEL channels[4];      /* channel information */
    Array<BYTE> hstream;           /* Huffman bitstream */

};

#define JPEG_SAVE_PRESET_CUSTOM  0 /* use custom setting */
#define JPEG_SAVE_PRESET_HIGH    1
#define JPEG_SAVE_PRESET_MEDIUM  2
#define JPEG_SAVE_PRESET_LOW     3

struct JPEG_SAVE_OPTION {

    /* 1. specify save preset */
    int save_preset;

    /* 2. or manually set the quantization tables, in this case you need to */
    /* set save_preset = JPEG_SAVE_PRESET_CUSTOM */
    INT_8x8 qtab_Y;                /* quantization table for luminance */
    INT_8x8 qtab_CbCr;             /* quantization table for chrominance */

};

/* * * * * * * * * * * * * * * * * * * * * * * * */
/* here are the interface functions for JPEG IO  */
/* * * * * * * * * * * * * * * * * * * * * * * * */

#define JPEG_API

/*
jpeg_read: read and decode a JPEG image file.

* If loading success, "is_valid" will be set to true, and
"image_data" contains the decoded raw image data (RGB).

* now the program can only read baseline JPEGs, progressive JPEGs
  are not supported.

* example:

    JPEG_FILE* jfile = jpeg_read("example.jpg");

    if (jfile->is_valid == false) {
        printf("error when loading JPEG file: %s\n", jfile->message);
        ...
    }
    else {
        save_PPM(jfile->image_data, "example.ppm");
        ...
    }
*/
JPEG_API JPEG_FILE* jpeg_read(const char* file);
/*
jpeg_free: unload a JPEG file.
*/
JPEG_API void jpeg_free(JPEG_FILE* jfile);
/*
jpeg_save: save an image data as JPEG format.

* example:

    JPEG_SAVE_OPTION option;
    option.save_preset = JPEG_SAVE_PRESET_MEDIUM;  <= using medium quality preset
    jpeg_save(image, &option, "example.jpg");      <= saving image as "example.jpg"

  or you can define the quantization table yourself:

    JPEG_SAVE_OPTION option;
    option.save_preset = JPEG_SAVE_PRESET_CUSTOM;  <= using custom quality
    option.qtab_Y = fill_int8x8(                   <= then defines the quantization
        " 8  6  9  14 17 21 28 17 "                   table for luminance channel
        " 6  6  8  13 18 23 12 12 "
        " 9  8  11 17 23 12 12 12 "
        " 14 13 17 23 12 12 12 12 "
        " 17 18 23 12 12 12 12 12 "
        " 21 23 12 12 12 12 12 12 "
        " 28 12 12 12 12 12 12 12 "
        " 17 12 12 12 12 12 12 12 "
    );
    option.qtab_CbCr = fill_int8x8(                <= defines the quantization table
        " 9  9  11 18 20 20 17 17 "                   for chrominance channels
        " 9  10 11 14 14 12 12 12 "
        " 11 11 14 14 12 12 12 12 "
        " 18 14 14 12 12 12 12 12 "
        " 20 14 12 12 12 12 12 12 "
        " 20 12 12 12 12 12 12 12 "
        " 17 12 12 12 12 12 12 12 "
        " 17 12 12 12 12 12 12 12 "
    );
    jpeg_save(image, &option, "example.jpg");      <= saving image as "example.jpg"
*/
bool jpeg_save(RAW_IMAGE* image, JPEG_SAVE_OPTION* option, const char* file);
