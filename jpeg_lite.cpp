#include "jpeg_lite.h"

/* JPEG markers, reference: https://www.disktuna.com/list-of-jpeg-markers/ */
/* JPEG specification uses "markers" to tell what type of data that is coming next, */
/* they always start with "0xFF" and the second byte indicates the data type. */
/*         Marker Value    Name                       Description                    */
const BYTE  SOF0 = 0xC0; /* Start of frame 0,          baseline DCT                  */
const BYTE  SOF1 = 0xC1; /* Start of frame 1,          extended sequential DCT       */
const BYTE  SOF2 = 0xC2; /* Start of frame 2,          progressive DCT               */
const BYTE  SOF3 = 0xC3; /* Start of frame 3,          loseless (sequential)         */
const BYTE   DHT = 0xC4; /* Define Huffman table */
const BYTE  SOF5 = 0xC5; /* Start of frame 5,          differential sequantial DCT   */
const BYTE  SOF6 = 0xC6; /* Start of frame 6,          differential progressive DCT  */
const BYTE  SOF7 = 0xC7; /* Start of frame 7,          differential loseless (sequential) */
const BYTE   JPG = 0xC8; /* JPEG extensions */
const BYTE  SOF9 = 0xC9; /* Start of frame 9,          extended sequential DCT, arithmetic coding */
const BYTE SOF10 = 0xCA; /* Start of frame 10,         progressive DCT, arithmetic coding */
const BYTE SOF11 = 0xCB; /* Start of frame 11,         lossless (sequential), arithmetic coding */
const BYTE   DAC = 0xCC; /* Define Arithmetic Coding                                 */
const BYTE SOF13 = 0xCD; /* Start of frame 13,         differential sequential DCT, arithmetic coding */
const BYTE SOF14 = 0xCE; /* Start of frame 14,         differential progressive DCT, arithmetic coding */
const BYTE SOF15 = 0xCF; /* Start of frame 15,         differential lossless (sequential), arithmetic coding */
const BYTE  RST0 = 0xD0; /* Restart Marker 0 */
const BYTE  RST1 = 0xD1; /* Restart Marker 1 */
const BYTE  RST2 = 0xD2; /* Restart Marker 2 */
const BYTE  RST3 = 0xD3; /* Restart Marker 3 */
const BYTE  RST4 = 0xD4; /* Restart Marker 4 */
const BYTE  RST5 = 0xD5; /* Restart Marker 5 */
const BYTE  RST6 = 0xD6; /* Restart Marker 6 */
const BYTE  RST7 = 0xD7; /* Restart Marker 7 */
const BYTE   SOI = 0xD8; /* Start of Image */
const BYTE   EOI = 0xD9; /* End of Image */
const BYTE   SOS = 0xDA; /* Start of Scan */
const BYTE   DQT = 0xDB; /* Define Quantization Table */
const BYTE   DNL = 0xDC; /* Define Number of Lines,     (Not common)                  */
const BYTE   DRI = 0xDD; /* Define Restart Interval */
const BYTE   DHP = 0xDE; /* Define Hierarchical Progression, (Not common)             */
const BYTE   EXP = 0xDF; /* Expand Reference Component, (Not common)                  */
const BYTE  APP0 = 0xE0; /* Application Segment 0,     JFIF - JFIF JPEG image
                                                      AVI1 - Motion JPEG (MJPG)     */
const BYTE  APP1 = 0xE1; /* Application Segment 1,     EXIF Metadata, TIFF IFD format,
                                                      JPEG Thumbnail (160¡Á120)
                                                      Adobe XMP                     */
const BYTE  APP2 = 0xE2; /* Application Segment 2,     ICC color profile,
                                                      FlashPix                      */
const BYTE  APP3 = 0xE3; /* Application Segment 3,     (Not common)
                                                      JPS Tag for Stereoscopic JPEG images */
const BYTE  APP4 = 0xE4; /* Application Segment 4,     (Not common)                   */
const BYTE  APP5 = 0xE5; /* Application Segment 5,     (Not common)                   */
const BYTE  APP6 = 0xE6; /* Application Segment 6,     (Not common)
                                                      NITF Lossles profile          */
const BYTE  APP7 = 0xE7; /* Application Segment 7,     (Not common)                   */
const BYTE  APP8 = 0xE8; /* Application Segment 8,     (Not common)                   */
const BYTE  APP9 = 0xE9; /* Application Segment 9,     (Not common)                   */
const BYTE APP10 = 0xEA; /* Application Segment 10,   (Not common)
                            PhoTags,                  ActiveObject (multimedia messages / captions) */
const BYTE APP11 = 0xEB; /* Application Segment 11,   (Not common)
                                                      HELIOS JPEG Resources (OPI Postscript) */
const BYTE APP12 = 0xEC; /* Application Segment 12,   Picture Info (older digicams),
                                                      Photoshop Save for Web: Ducky */
const BYTE APP13 = 0xED; /* Application Segment 13,   Photoshop Save As: IRB, 8BIM, IPTC */
const BYTE APP14 = 0xEE; /* Application Segment 14,   (Not common)                    */
const BYTE APP15 = 0xEF; /* Application Segment 15,   (Not common)                    */
const BYTE  JPG0 = 0xF0; /* JPEG Extension 0~6,       (Not common)                    */
const BYTE  JPG1 = 0xF1;
const BYTE  JPG2 = 0xF2;
const BYTE  JPG3 = 0xF3;
const BYTE  JPG4 = 0xF4;
const BYTE  JPG5 = 0xF5;
const BYTE  JPG6 = 0xF6;
const BYTE  JPG7 = 0xF7, SOF48 = 0xF7;
/*                          JPEG Extension 7,         Loseless JPEG
                            JPEG-LS                                                 */
const BYTE  JPG8 = 0xF8, LSE = 0xF8;
/*                          JPEG Extension 8,         Lossless JPEG Extension Parameters
                            JPEG-LS Extension                                       */
const BYTE  JPG9 = 0xF9; /* JPEG Extension 9,          (Not common)                  */
const BYTE JPG10 = 0xFA; /* JPEG Extension 10,        (Not common)                  */
const BYTE JPG11 = 0xFB; /* JPEG Extension 11,        (Not common)                  */
const BYTE JPG12 = 0xFC; /* JPEG Extension 12,        (Not common)                  */
const BYTE JPG13 = 0xFD; /* JPEG Extension 13,        (Not common)                  */
const BYTE   COM = 0xFE; /* Comment */
const BYTE   TEM = 0x01; /* One byte, just skip */

/* precomputed values for fast 2D DCT */

const REAL _DCT_S[] = {
    REAL(0.353553390593273762200422), REAL(0.254897789552079584470970),
    REAL(0.270598050073098492199862), REAL(0.300672443467522640271861),
    REAL(0.353553390593273762200422), REAL(0.449988111568207852319255),
    REAL(0.653281482438188263928322), REAL(1.281457723870753089398043),
};
const REAL _DCT_A[] = {
    REAL(0.0), /* never used */
    REAL(0.707106781186547524400844), REAL(0.541196100146196984399723),
    REAL(0.707106781186547524400844), REAL(1.306562964876376527856643),
    REAL(0.382683432365089771728460),
};

/*
JPEG chroma subsampling
----------------------
1. no subsampling (w8 x h8)                  : only Y0,Cb,Cr used
2. horizontal subsampling (w16 x h8)         : only Y0,Y1,Cb,Cr used
3. vertical subsampling (w8 x h16)           : only Y0,Y1,Cb,Cr used
4. horiz. and verti. subsampling (w16 x h16) : all of them are used
*/
struct JPEG_MCU {
    REAL_8x8 Y0, Y1, Y2, Y3, Cb, Cr;
};

/* structure used to store quantized coefficients */
struct JPEG_MCU_QCOEFF {
    INT_8x8 Y0, Y1, Y2, Y3, Cb, Cr;
    BYTE_8x8 Y0_bits, Y1_bits, Y2_bits, Y3_bits,
        Cb_bits, Cr_bits; /* how many bits are used in each quantized coefficient, used in progressive JPEGs */
    int Y0_diff, Y1_diff, Y2_diff, Y3_diff, Cb_diff, Cr_diff; /* used in calculating relative DC coefficient */
};

/* store triplet [(r,s),v] generated in run length encoding */
struct JPEG_RLE_TRIPLET {
    int nZ, l, v;
    Array<BYTE> symbols; /* parsed symbols. */
                         /* sometimes a triplet may resemble multiple symbols. */
};

INT_8x8 fill_int8x8(const char * s)
{
    INT_8x8 m;
    char buf[8];
    char* S = (char*)malloc(strlen(s) + 1);
    strcpy(S, s);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            getWord(S, buf, 8, " ");
            char* errptr = NULL;
            int val = (int)(strtol(buf, &errptr, 10));
            m.data[i][j] = val;
        }
    }
    free(S);
    return m;
}
REAL_8x8 fill_real8x8(const char * s)
{
    REAL_8x8 m;
    char buf[8];
    char* S = (char*)malloc(strlen(s) + 1);
    strcpy(S, s);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            getWord(S, buf, 8, " ");
            char* errptr = NULL;
            REAL val = (REAL)(strtod(buf, &errptr));
            m.data[i][j] = val;
        }
    }
    free(S);
    return m;
}

void print_int8x8(INT_8x8 * a)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d ", a->data[i][j]);
        }
        printf("\n");
    }
}
void print_real8x8(REAL_8x8 * a)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%.2f ", a->data[i][j]);
        }
        printf("\n");
    }
}

REAL_8x8 _jpeg_int8x8_to_real8x8(INT_8x8 * a)
{
    REAL_8x8 m;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            m.data[i][j] = (REAL)(a->data[i][j]);
        }
    }
    return m;
}
INT_8x8 _jpeg_real8x8_to_int8x8(REAL_8x8 * a)
{
    INT_8x8 m;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            m.data[i][j] = (int)Round(a->data[i][j]);
        }
    }
    return m;
}
/*
convert int8x8 to an array using zig-zag ordering
z must be an pre-allocated memory with 64 integers
storage space
*/
void _jpeg_zz_int8x8_to_intarr(INT_8x8 * a, int * z)
{
    z[0] = a->data[0][0]; z[1] = a->data[0][1]; z[5] = a->data[0][2]; z[6] = a->data[0][3];
    z[14] = a->data[0][4]; z[15] = a->data[0][5]; z[27] = a->data[0][6]; z[28] = a->data[0][7];
    z[2] = a->data[1][0]; z[4] = a->data[1][1]; z[7] = a->data[1][2]; z[13] = a->data[1][3];
    z[16] = a->data[1][4]; z[26] = a->data[1][5]; z[29] = a->data[1][6]; z[42] = a->data[1][7];
    z[3] = a->data[2][0]; z[8] = a->data[2][1]; z[12] = a->data[2][2]; z[17] = a->data[2][3];
    z[25] = a->data[2][4]; z[30] = a->data[2][5]; z[41] = a->data[2][6]; z[43] = a->data[2][7];
    z[9] = a->data[3][0]; z[11] = a->data[3][1]; z[18] = a->data[3][2]; z[24] = a->data[3][3];
    z[31] = a->data[3][4]; z[40] = a->data[3][5]; z[44] = a->data[3][6]; z[53] = a->data[3][7];
    z[10] = a->data[4][0]; z[19] = a->data[4][1]; z[23] = a->data[4][2]; z[32] = a->data[4][3];
    z[39] = a->data[4][4]; z[45] = a->data[4][5]; z[52] = a->data[4][6]; z[54] = a->data[4][7];
    z[20] = a->data[5][0]; z[22] = a->data[5][1]; z[33] = a->data[5][2]; z[38] = a->data[5][3];
    z[46] = a->data[5][4]; z[51] = a->data[5][5]; z[55] = a->data[5][6]; z[60] = a->data[5][7];
    z[21] = a->data[6][0]; z[34] = a->data[6][1]; z[37] = a->data[6][2]; z[47] = a->data[6][3];
    z[50] = a->data[6][4]; z[56] = a->data[6][5]; z[59] = a->data[6][6]; z[61] = a->data[6][7];
    z[35] = a->data[7][0]; z[36] = a->data[7][1]; z[48] = a->data[7][2]; z[49] = a->data[7][3];
    z[57] = a->data[7][4]; z[58] = a->data[7][5]; z[62] = a->data[7][6]; z[63] = a->data[7][7];
}
void _jpeg_zz_intarr_to_int8x8(int * z, INT_8x8 * a)
{
    a->data[0][0] = z[0]; a->data[0][1] = z[1]; a->data[0][2] = z[5]; a->data[0][3] = z[6];
    a->data[0][4] = z[14]; a->data[0][5] = z[15]; a->data[0][6] = z[27]; a->data[0][7] = z[28];
    a->data[1][0] = z[2]; a->data[1][1] = z[4]; a->data[1][2] = z[7]; a->data[1][3] = z[13];
    a->data[1][4] = z[16]; a->data[1][5] = z[26]; a->data[1][6] = z[29]; a->data[1][7] = z[42];
    a->data[2][0] = z[3]; a->data[2][1] = z[8]; a->data[2][2] = z[12]; a->data[2][3] = z[17];
    a->data[2][4] = z[25]; a->data[2][5] = z[30]; a->data[2][6] = z[41]; a->data[2][7] = z[43];
    a->data[3][0] = z[9]; a->data[3][1] = z[11]; a->data[3][2] = z[18]; a->data[3][3] = z[24];
    a->data[3][4] = z[31]; a->data[3][5] = z[40]; a->data[3][6] = z[44]; a->data[3][7] = z[53];
    a->data[4][0] = z[10]; a->data[4][1] = z[19]; a->data[4][2] = z[23]; a->data[4][3] = z[32];
    a->data[4][4] = z[39]; a->data[4][5] = z[45]; a->data[4][6] = z[52]; a->data[4][7] = z[54];
    a->data[5][0] = z[20]; a->data[5][1] = z[22]; a->data[5][2] = z[33]; a->data[5][3] = z[38];
    a->data[5][4] = z[46]; a->data[5][5] = z[51]; a->data[5][6] = z[55]; a->data[5][7] = z[60];
    a->data[6][0] = z[21]; a->data[6][1] = z[34]; a->data[6][2] = z[37]; a->data[6][3] = z[47];
    a->data[6][4] = z[50]; a->data[6][5] = z[56]; a->data[6][6] = z[59]; a->data[6][7] = z[61];
    a->data[7][0] = z[35]; a->data[7][1] = z[36]; a->data[7][2] = z[48]; a->data[7][3] = z[49];
    a->data[7][4] = z[57]; a->data[7][5] = z[58]; a->data[7][6] = z[62]; a->data[7][7] = z[63];
}
void _jpeg_zz_intarr_to_real8x8(int * z, REAL_8x8 * a)
{
    a->data[0][0] = REAL(z[0]); a->data[0][1] = REAL(z[1]); a->data[0][2] = REAL(z[5]); a->data[0][3] = REAL(z[6]);
    a->data[0][4] = REAL(z[14]); a->data[0][5] = REAL(z[15]); a->data[0][6] = REAL(z[27]); a->data[0][7] = REAL(z[28]);
    a->data[1][0] = REAL(z[2]); a->data[1][1] = REAL(z[4]); a->data[1][2] = REAL(z[7]); a->data[1][3] = REAL(z[13]);
    a->data[1][4] = REAL(z[16]); a->data[1][5] = REAL(z[26]); a->data[1][6] = REAL(z[29]); a->data[1][7] = REAL(z[42]);
    a->data[2][0] = REAL(z[3]); a->data[2][1] = REAL(z[8]); a->data[2][2] = REAL(z[12]); a->data[2][3] = REAL(z[17]);
    a->data[2][4] = REAL(z[25]); a->data[2][5] = REAL(z[30]); a->data[2][6] = REAL(z[41]); a->data[2][7] = REAL(z[43]);
    a->data[3][0] = REAL(z[9]); a->data[3][1] = REAL(z[11]); a->data[3][2] = REAL(z[18]); a->data[3][3] = REAL(z[24]);
    a->data[3][4] = REAL(z[31]); a->data[3][5] = REAL(z[40]); a->data[3][6] = REAL(z[44]); a->data[3][7] = REAL(z[53]);
    a->data[4][0] = REAL(z[10]); a->data[4][1] = REAL(z[19]); a->data[4][2] = REAL(z[23]); a->data[4][3] = REAL(z[32]);
    a->data[4][4] = REAL(z[39]); a->data[4][5] = REAL(z[45]); a->data[4][6] = REAL(z[52]); a->data[4][7] = REAL(z[54]);
    a->data[5][0] = REAL(z[20]); a->data[5][1] = REAL(z[22]); a->data[5][2] = REAL(z[33]); a->data[5][3] = REAL(z[38]);
    a->data[5][4] = REAL(z[46]); a->data[5][5] = REAL(z[51]); a->data[5][6] = REAL(z[55]); a->data[5][7] = REAL(z[60]);
    a->data[6][0] = REAL(z[21]); a->data[6][1] = REAL(z[34]); a->data[6][2] = REAL(z[37]); a->data[6][3] = REAL(z[47]);
    a->data[6][4] = REAL(z[50]); a->data[6][5] = REAL(z[56]); a->data[6][6] = REAL(z[59]); a->data[6][7] = REAL(z[61]);
    a->data[7][0] = REAL(z[35]); a->data[7][1] = REAL(z[36]); a->data[7][2] = REAL(z[48]); a->data[7][3] = REAL(z[49]);
    a->data[7][4] = REAL(z[57]); a->data[7][5] = REAL(z[58]); a->data[7][6] = REAL(z[62]); a->data[7][7] = REAL(z[63]);
}
INT_8x8 _jpeg_int8x8_mul_int8x8(INT_8x8 * a, INT_8x8 * b)
{
    INT_8x8 c;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            c.data[y][x] = a->data[y][x] * b->data[y][x];
        }
    }
    return c;
}
REAL_8x8 _jpeg_real8x8_mul_int8x8(REAL_8x8 * a, INT_8x8 * b)
{
    REAL_8x8 c;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            c.data[y][x] = a->data[y][x] * REAL(b->data[y][x]);
        }
    }
    return c;
}

/* quantize coefficients a using quantization table q */
INT_8x8 _jpeg_quantize_real8x8(REAL_8x8 * a, INT_8x8 * q)
{
    INT_8x8 c;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            c.data[y][x] = int(Round(a->data[y][x] / q->data[y][x]));
        }
    }
    return c;
}

bool alloc_image(int w, int h, RAW_IMAGE ** image)
{
    if (w < 1 || h < 1) {
        *image = NULL;
        return false;
    }

    RAW_IMAGE* p = (RAW_IMAGE*)malloc(sizeof(RAW_IMAGE));
    if (p == NULL) return false;

    p->w = w;
    p->h = h;
    p->r = NULL;
    p->g = NULL;
    p->b = NULL;

    int channel_bytes = sizeof(BYTE) * w * h;
    p->r = (BYTE*)malloc(channel_bytes);
    p->g = (BYTE*)malloc(channel_bytes);
    p->b = (BYTE*)malloc(channel_bytes);

    if (p->r == NULL || p->g == NULL || p->b == NULL) {
        free_image(p);
        *image = NULL;
        return false;
    }
    else {
        /* success, clear trash data */
        memset(p->r, 0, channel_bytes);
        memset(p->g, 0, channel_bytes);
        memset(p->b, 0, channel_bytes);
        *image = p;
        return true;
    }
}

bool free_image(RAW_IMAGE * image)
{
    if (image == NULL)
        return false;
    if (image->r) {
        free(image->r);
        image->r = NULL;
    }
    if (image->g) {
        free(image->g);
        image->g = NULL;
    }
    if (image->b) {
        free(image->b);
        image->b = NULL;
    }
    free(image);
    return true;
}

bool save_PPM(RAW_IMAGE * data, const char * file)
{
    FILE* fp = NULL;
    fp = fopen(file, "w");
    if (fp == NULL) {
        return false;
    }

    const char* magic_number = "P3"; /* ASCII PortablePixMap (PPM) */
    char strbuffer[64];

    fwrite(magic_number, 1, 2, fp);
    fwrite(" ", 1, 1, fp);
    strcpy(strbuffer, "# ASCII PPM format\n");
    fwrite(strbuffer, 1, strlen(strbuffer), fp);

    char buffer[16];

    strcpy(strbuffer, "# image width and height\n");
    fwrite(strbuffer, 1, strlen(strbuffer), fp);

    /* write image width and height */
    itoa(data->w, buffer, 10);
    fwrite(buffer, 1, strlen(buffer), fp);
    fwrite(" ", 1, 1, fp);
    itoa(data->h, buffer, 10);
    fwrite(buffer, 1, strlen(buffer), fp);
    fwrite(" ", 1, 1, fp);

    strcpy(strbuffer, "\n# image bitdepth\n");
    fwrite(strbuffer, 1, strlen(strbuffer), fp);

    /* write image depth */
    fwrite("255", 1, 3, fp);

    /* image data */
    strcpy(strbuffer, "\n# image data (R G B)\n");
    fwrite(strbuffer, 1, strlen(strbuffer), fp);

    for (int y = 0; y < data->h; y++) {
        for (int x = 0; x < data->w; x++) {
            BYTE r = data->r[y*data->w + x];
            BYTE g = data->g[y*data->w + x];
            BYTE b = data->b[y*data->w + x];
            itoa(r, buffer, 10);
            fwrite(buffer, 1, strlen(buffer), fp);
            fwrite(" ", 1, 1, fp);
            itoa(g, buffer, 10);
            fwrite(buffer, 1, strlen(buffer), fp);
            fwrite(" ", 1, 1, fp);
            itoa(b, buffer, 10);
            fwrite(buffer, 1, strlen(buffer), fp);
            fwrite("  ", 1, 2, fp);
        }
        fwrite("\n", 1, 1, fp);
    }

    fclose(fp);

    return true;
}

REAL_8x8 DCT8x8(REAL_8x8 * f)
{
    REAL N = 8;
    REAL t1 = ONE / Sqrt(2);
    /* calculate C(u)C(v) */
    REAL_8x8 CuCv;
    for (int u = 0; u < 8; u++) {
        for (int v = 0; v < 8; v++) {
            CuCv.data[u][v] = ((u == 0 ? t1 : ONE) * (v == 0 ? t1 : ONE));
        }
    }
    /* calculate F(u,v) */
    REAL_8x8 F;
    REAL t2 = TWO / N;
    for (int u = 0; u < 8; u++) {
        for (int v = 0; v < 8; v++) {
            REAL sum = ZERO;
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {
                    sum += f->data[x][y] * Cos((PI*u / N)*(x + HALF)) * Cos((PI*v / N)*(y + HALF));
                }
            }
            F.data[u][v] = t2 * CuCv.data[u][v] * sum;
        }
    }
    return F;
}
void DCT8_fast(REAL v[8])
{
    const REAL v0 = v[0] + v[7];
    const REAL v1 = v[1] + v[6];
    const REAL v2 = v[2] + v[5];
    const REAL v3 = v[3] + v[4];
    const REAL v4 = v[3] - v[4];
    const REAL v5 = v[2] - v[5];
    const REAL v6 = v[1] - v[6];
    const REAL v7 = v[0] - v[7];

    const REAL v8 = v0 + v3;
    const REAL v9 = v1 + v2;
    const REAL v10 = v1 - v2;
    const REAL v11 = v0 - v3;
    const REAL v12 = -v4 - v5;
    const REAL v13 = (v5 + v6) * _DCT_A[3];
    const REAL v14 = v6 + v7;

    const REAL v15 = v8 + v9;
    const REAL v16 = v8 - v9;
    const REAL v17 = (v10 + v11) * _DCT_A[1];
    const REAL v18 = (v12 + v14) * _DCT_A[5];

    const REAL v19 = -v12 * _DCT_A[2] - v18;
    const REAL v20 = v14 * _DCT_A[4] - v18;

    const REAL v21 = v17 + v11;
    const REAL v22 = v11 - v17;
    const REAL v23 = v13 + v7;
    const REAL v24 = v7 - v13;

    const REAL v25 = v19 + v24;
    const REAL v26 = v23 + v20;
    const REAL v27 = v23 - v20;
    const REAL v28 = v24 - v19;

    v[0] = _DCT_S[0] * v15;
    v[1] = _DCT_S[1] * v26;
    v[2] = _DCT_S[2] * v21;
    v[3] = _DCT_S[3] * v28;
    v[4] = _DCT_S[4] * v16;
    v[5] = _DCT_S[5] * v25;
    v[6] = _DCT_S[6] * v22;
    v[7] = _DCT_S[7] * v27;
}
REAL_8x8 IDCT8x8(REAL_8x8 * F)
{
    REAL N = 8;
    REAL t1 = ONE / Sqrt(2);
    /* calculate C(u)C(v) */
    REAL_8x8 CuCv;
    for (int u = 0; u < 8; u++) {
        for (int v = 0; v < 8; v++) {
            CuCv.data[u][v] = ((u == 0 ? t1 : ONE) * (v == 0 ? t1 : ONE));
        }
    }
    /* calculate F(x,y) */
    REAL_8x8 f;
    REAL t2 = TWO / N;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            REAL sum = ZERO;
            for (int u = 0; u < 8; u++) {
                for (int v = 0; v < 8; v++) {
                    sum += CuCv.data[u][v] * F->data[u][v] * Cos((PI*u / N)*(x + HALF)) * Cos((PI*v / N)*(y + HALF));
                }
            }
            f.data[x][y] = t2 * sum;
        }
    }
    return f;
}
void IDCT8_fast(REAL v[8])
{
    const REAL v15 = v[0] / _DCT_S[0];
    const REAL v26 = v[1] / _DCT_S[1];
    const REAL v21 = v[2] / _DCT_S[2];
    const REAL v28 = v[3] / _DCT_S[3];
    const REAL v16 = v[4] / _DCT_S[4];
    const REAL v25 = v[5] / _DCT_S[5];
    const REAL v22 = v[6] / _DCT_S[6];
    const REAL v27 = v[7] / _DCT_S[7];

    const REAL v19 = (v25 - v28) / 2;
    const REAL v20 = (v26 - v27) / 2;
    const REAL v23 = (v26 + v27) / 2;
    const REAL v24 = (v25 + v28) / 2;

    const REAL v7 = (v23 + v24) / 2;
    const REAL v11 = (v21 + v22) / 2;
    const REAL v13 = (v23 - v24) / 2;
    const REAL v17 = (v21 - v22) / 2;

    const REAL v8 = (v15 + v16) / 2;
    const REAL v9 = (v15 - v16) / 2;

    const REAL v18 = (v19 - v20) * _DCT_A[5];  /* different from original */
    const REAL v12 = (v19 * _DCT_A[4] - v18) / (_DCT_A[2] * _DCT_A[5] - _DCT_A[2] * _DCT_A[4] - _DCT_A[4] * _DCT_A[5]);
    const REAL v14 = (v18 - v20 * _DCT_A[2]) / (_DCT_A[2] * _DCT_A[5] - _DCT_A[2] * _DCT_A[4] - _DCT_A[4] * _DCT_A[5]);

    const REAL v6 = v14 - v7;
    const REAL v5 = v13 / _DCT_A[3] - v6;
    const REAL v4 = -v5 - v12;
    const REAL v10 = v17 / _DCT_A[1] - v11;

    const REAL v0 = (v8 + v11) / 2;
    const REAL v1 = (v9 + v10) / 2;
    const REAL v2 = (v9 - v10) / 2;
    const REAL v3 = (v8 - v11) / 2;

    v[0] = (v0 + v7) / 2;
    v[1] = (v1 + v6) / 2;
    v[2] = (v2 + v5) / 2;
    v[3] = (v3 + v4) / 2;
    v[4] = (v3 - v4) / 2;
    v[5] = (v2 - v5) / 2;
    v[6] = (v1 - v6) / 2;
    v[7] = (v0 - v7) / 2;
}
REAL_8x8* DCT8x8_fast(REAL_8x8 * f)
{   /* DCT for each column */
    REAL x[8];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) x[j] = f->data[j][i];
        DCT8_fast(x);
        for (int j = 0; j < 8; j++) f->data[j][i] = x[j];
    }
    /* DCT for each row */
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) x[j] = f->data[i][j];
        DCT8_fast(x);
        for (int j = 0; j < 8; j++) f->data[i][j] = x[j];
    }
    return f;
}
REAL_8x8* IDCT8x8_fast(REAL_8x8 * F)
{   /* IDCT for each column */
    REAL x[8];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) x[j] = F->data[j][i];
        IDCT8_fast(x);
        for (int j = 0; j < 8; j++) F->data[j][i] = x[j];
    }
    /* IDCT for each row */
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) x[j] = F->data[i][j];
        IDCT8_fast(x);
        for (int j = 0; j < 8; j++) F->data[i][j] = x[j];
    }
    return F;
}

bool _jpeg_read_fp(FILE* fp, int bytes, void * buffer)
{
    if (fp == NULL || bytes <= 0) {
        return false;
    }
    if (int(fread(buffer, 1, bytes, fp)) != bytes) {
        return false;
    }
    return true;
}

bool _jpeg_write_fp(FILE* fp, int bytes, void * buffer) {
    if (fp == NULL || bytes <= 0) return false;
    if (int(fwrite(buffer, 1, bytes, fp) != bytes)) {
        return false;
    }
    return true;
}

/* dump message when loading */
void _jpeg_dump_message(JPEG_FILE* jfile, const char* message) {
    /* concatenate message */
    int istart = strlen(jfile->message), iend = _JPEG_MSG_LEN - 1;
    int icur, j = 0;
    for (icur = istart; icur < iend; icur++, j++) {
        jfile->message[icur] = message[j];
    }
    jfile->message[icur + 1] = '\0';
}

/* JPEG marker read functions */
/* JPEG read unknown marker (do nothing) */
bool _jpeg_read_NONE(FILE* fp, JPEG_FILE* jfile) {
    BYTE buffer[16];
    /* assume big-endian */
    _jpeg_read_fp(fp, 1, buffer); /* higher 8 bits */
    _jpeg_read_fp(fp, 1, buffer + 1); /* lower 8 bits */
    int length = ((UINT(buffer[0]) << 8) | UINT(buffer[1])) - 2; /* size indicator is 2 bytes so we need to exclude marker length */
    /* we dont care what is in this marker so we just read it and do nothing. */
    for (int i = 0; i < length; i++) {
        _jpeg_read_fp(fp, 1, buffer);
    }
    return true;
}
bool _jpeg_read_APPN(FILE* fp, JPEG_FILE* jfile) {
    return _jpeg_read_NONE(fp, jfile);
}
/* JPEG read comment */
bool _jpeg_read_COM(FILE* fp, JPEG_FILE* jfile) {
    return _jpeg_read_NONE(fp, jfile);
}
/* JPEG read JPG0~13 */
bool _jpeg_read_JPGN(FILE* fp, JPEG_FILE* jfile) {
    return _jpeg_read_NONE(fp, jfile);
}
/* JPEG read quantization tables */
bool _jpeg_read_QTAB(FILE* fp, JPEG_FILE* jfile) {
    BYTE buffer[16];
    /* assume big-endian */
    _jpeg_read_fp(fp, 1, buffer); /* higher 8 bits */
    _jpeg_read_fp(fp, 1, buffer + 1); /* lower 8 bits */
    int length = ((UINT(buffer[0]) << 8) | UINT(buffer[1])) - 2; /* size indicator is 2 bytes so we need to exclude marker length */

    while (length > 0) {
        BYTE tabInfo;
        _jpeg_read_fp(fp, 1, &tabInfo);
        length -= 1;
        BYTE tabID, tabBits;
        tabID = (tabInfo & 0x0F);
        tabBits = ((tabInfo >> 4) > 0) ? 16 : 8;
        if (tabID > 3) {
            _jpeg_dump_message(jfile, "invalid quantization table ID.");
            return false;
        }
        int qtabz[64]; /* quantization table stored in zig-zag order */
        if (tabBits == 16) {
            /* read 16 bit quantization table */
            for (int u = 0; u < 8; u++) {
                for (int v = 0; v < 8; v++) {
                    _jpeg_read_fp(fp, 1, buffer); /* higher 8 bits */
                    _jpeg_read_fp(fp, 1, buffer + 1); /* lower 8 bits */
                    UINT value = ((UINT(buffer[0]) << 8) | UINT(buffer[1]));
                    qtabz[u * 8 + v] = int(value);
                }
            }
            length -= 128;
        }
        else if (tabBits == 8) {
            /* read 8 bit quantization table */
            for (int u = 0; u < 8; u++) {
                for (int v = 0; v < 8; v++) {
                    _jpeg_read_fp(fp, 1, buffer);
                    qtabz[u * 8 + v] = int(buffer[0]);
                }
            }
            length -= 64;
        }
        /* convert zig-zag quantization table into 8x8 matrix */
        _jpeg_zz_intarr_to_int8x8(qtabz, &(jfile->qtabs[tabID]));
    }

    //print_int8x8(jfile->qtabs);

    return true;

}
bool _jpeg_read_SOF0(FILE* fp, JPEG_FILE* jfile) {
    BYTE buffer[16];
    /* assume big-endian */
    _jpeg_read_fp(fp, 1, buffer); /* higher 8 bits */
    _jpeg_read_fp(fp, 1, buffer + 1); /* lower 8 bits */
    int length = ((UINT(buffer[0]) << 8) | UINT(buffer[1])) - 2; /* size indicator is 2 bytes so we need to exclude marker length */

    /*
    FF CX : C0~C15, marker
    XX XX : length (including this 2 bytes)
    XX    : precision, how many bits used in each color channel (must be 8)
    XX XX : image height
    XX XX : image width
    XX    : number of channels (1/3/4, 1: grayscale - luminance
                                       3: color data - Y Cb Cr
                                       4: CMYK JPEG, not supported)
    {
    XX    : channel ID 1/2/3/4 (standard), or rarely, 0/1/2/3
    XX    : sampling factors
    XX    : quantization table ID used for this channel
    } x N
    */

    /* read precision (must be 8) */
    _jpeg_read_fp(fp, 1, buffer);
    if (buffer[0] != 8) {
        _jpeg_dump_message(jfile, "invalid precision setting in JPEG file, precision must be 8 bits.");
        return false;
    }

    /* read image height */
    _jpeg_read_fp(fp, 1, buffer); /* higher 8 bits */
    _jpeg_read_fp(fp, 1, buffer + 1); /* lower 8 bits */
    int image_height = ((int(buffer[0]) << 8) | int(buffer[1]));

    /* read image width */
    _jpeg_read_fp(fp, 1, buffer); /* higher 8 bits */
    _jpeg_read_fp(fp, 1, buffer + 1); /* lower 8 bits */
    int image_width = ((int(buffer[0]) << 8) | int(buffer[1]));

    if (image_width <= 0 || image_height <= 0) {
        _jpeg_dump_message(jfile, "invalid image width and height.");
        return false;
    }

    /* read number of channels */
    _jpeg_read_fp(fp, 1, buffer);
    int num_channels = int(buffer[0]);
    if (num_channels == 4) {
        _jpeg_dump_message(jfile, "unsupported CMYK channel format.");
        return false;
    }
    else if (num_channels == 0 || num_channels > 4) {
        _jpeg_dump_message(jfile, "invalid number of channels.");
        return false;
    }

    jfile->num_channels = num_channels;
    jfile->image_width = image_width;
    jfile->image_height = image_height;

    /* guess channel start index, some JPEG image use channel 0 as start index,
       that is not correct but we still need to try our best to read the image */
    bool zero_start = false; /* default */
    int foffset = ftell(fp);
    for (int i = 0; i < num_channels; i++) {
        _jpeg_read_fp(fp, 1, buffer);
        int channelID = int(buffer[0]);
        if (channelID == 0) {
            zero_start = true;
            break; /* we found a channel with ID == 0 */
        }
        _jpeg_read_fp(fp, 2, buffer); /* skip 2 bytes */
    }

    jfile->zero_start = zero_start;

    /* restore file pointer */
    fseek(fp, foffset, SEEK_SET);

    /* read each channel */
    for (int i = 0; i < num_channels; i++) {
        /* read and check channel ID */
        _jpeg_read_fp(fp, 1, buffer);
        int channelID = int(buffer[0]);
        if (zero_start)
            channelID += 1; /* convert to valid channel ID that starts with 1 */
        if (channelID == 0 || channelID >= 5) {
            _jpeg_dump_message(jfile, "invalid channel ID.");
            return false;
        }
        if (channelID == 4) {
            _jpeg_dump_message(jfile, "unsupported 4th channel.");
        }

        /* read channel info */
        JPEG_CHANNEL* jchannel = &(jfile->channels[channelID - 1]);
        if (jchannel->is_used) {
            _jpeg_dump_message(jfile, "channel is already used.");
            return false;
        }
        jchannel->is_used = true;

        /* read sampling factors */
        _jpeg_read_fp(fp, 1, buffer);
        int horizontal_sampling_factor = int(buffer[0] >> 4);
        int vertical_sampling_factor = int(buffer[0] & 0x0F);

        /* read quantization table ID */
        _jpeg_read_fp(fp, 1, buffer);
        int qtab_id = int(buffer[0]);
        if (qtab_id > 3) {
            _jpeg_dump_message(jfile, "invalid quantization table ID.");
            return false;
        }

        jchannel->hsample = horizontal_sampling_factor;
        jchannel->vsample = vertical_sampling_factor;
        jchannel->qtab_id = qtab_id;

    }

    /* final check if length read is correct */
    if (length - 6 - num_channels * 3 != 0) {
        _jpeg_dump_message(jfile, "header size incorrect.");
        return false;
    }

    return true;
}
bool _jpeg_read_DRI(FILE* fp, JPEG_FILE* jfile) {
    BYTE buffer[16];
    /* assume big-endian */
    _jpeg_read_fp(fp, 1, buffer); /* higher 8 bits */
    _jpeg_read_fp(fp, 1, buffer + 1); /* lower 8 bits */
    int length = ((UINT(buffer[0]) << 8) | UINT(buffer[1])) - 2; /* size indicator is 2 bytes so we need to exclude marker length */
    if (length != 2) {
        _jpeg_dump_message(jfile, "header size incorrect.");
        return false;
    }
    _jpeg_read_fp(fp, 1, buffer); /* higher 8 bits */
    _jpeg_read_fp(fp, 1, buffer + 1); /* lower 8 bits */
    int restart_interval = ((UINT(buffer[0]) << 8) | UINT(buffer[1]));
    if (restart_interval < 0) {
        _jpeg_dump_message(jfile, "invalid DC coefficient restart interval.");
        return false;
    }
    jfile->restart_interval = restart_interval;
    return true;
}
bool _jpeg_read_DHT(FILE* fp, JPEG_FILE* jfile) {
    BYTE buffer[16];
    /* assume big-endian */
    _jpeg_read_fp(fp, 1, buffer); /* higher 8 bits */
    _jpeg_read_fp(fp, 1, buffer + 1); /* lower 8 bits */
    int length = ((UINT(buffer[0]) << 8) | UINT(buffer[1])) - 2; /* size indicator is 2 bytes so we need to exclude marker length */

    while (length > 0) {
        /* read table info */
        _jpeg_read_fp(fp, 1, buffer);
        BYTE htab_id = (buffer[0] & 0x0F);
        BYTE htab_type = (buffer[0] >> 4); /* 0: DC, 1: AC */
        if (htab_id > 3) {
            _jpeg_dump_message(jfile, "invalid Huffman table ID.");
            return false;
        }
        JPEG_HUFFMAN_TABLE* jhtab;
        if (htab_type == 0) {
            jhtab = &(jfile->dctabs[htab_id]);
        }
        else {
            jhtab = &(jfile->actabs[htab_id]);
        }

        /* fill in Huffman tables */
        jhtab->is_used = true;
        jhtab->offsets[0] = 0;
        int symbols_used = 0; /* total # of symbols used */
        for (int i = 1; i <= 16; i++) {
            _jpeg_read_fp(fp, 1, buffer);
            symbols_used += int(buffer[0]);
            jhtab->offsets[i] = symbols_used;
        }
        if (symbols_used > 176) {
            _jpeg_dump_message(jfile, "too many symbols used in Huffman table.");
            return false;
        }
        for (int i = 0; i < symbols_used; i++) {
            _jpeg_read_fp(fp, 1, buffer);
            jhtab->symbols[i] = buffer[0];
        }
        length = length - 1 - 16 - symbols_used;
    }
    if (length < 0) {
        _jpeg_dump_message(jfile, "header size incorrect.");
        return false;
    }
    return true;
}
bool _jpeg_read_SOS(FILE* fp, JPEG_FILE* jfile) {

    BYTE buffer[16];

    /* assume big-endian */
    _jpeg_read_fp(fp, 1, buffer); /* higher 8 bits */
    _jpeg_read_fp(fp, 1, buffer + 1); /* lower 8 bits */
    int length = ((UINT(buffer[0]) << 8) | UINT(buffer[1])) - 2; /* size indicator is 2 bytes so we need to exclude marker length */

    /* start of scan, defines the actual data in each MCU */

    for (int i = 0; i < jfile->num_channels; i++) {
        jfile->channels[i].is_used = false; /* set temporary flag to be false */
    }
    BYTE num_channels;
    _jpeg_read_fp(fp, 1, &num_channels);
    for (int i = 0; i < num_channels; i++) {
        BYTE channel_id;
        _jpeg_read_fp(fp, 1, &channel_id);
        if (jfile->zero_start) channel_id += 1; /* force starts with 1 */
        if (channel_id > jfile->num_channels) {
            _jpeg_dump_message(jfile, "invalid channel ID.");
            return false;
        }
        JPEG_CHANNEL* jchannel = &(jfile->channels[i]);
        if (jchannel->is_used) {
            _jpeg_dump_message(jfile, "set the same channel twice in a loop.");
            return false;
        }
        jchannel->is_used = true;
        _jpeg_read_fp(fp, 1, buffer);
        jchannel->dctab_id = buffer[0] >> 4;
        jchannel->actab_id = (buffer[0] & 0x0F);
        if (jchannel->dctab_id > 3 || jchannel->actab_id > 3) {
            _jpeg_dump_message(jfile, "too many Huffman DC/AC tables used (4 at maximum).");
            return false;
        }
    }
    _jpeg_read_fp(fp, 1, buffer);
    _jpeg_read_fp(fp, 1, buffer);
    _jpeg_read_fp(fp, 1, buffer);

    /* then read Huffman encoded bitstream */
    BYTE highbyte, lowbyte;
    if (!_jpeg_read_fp(fp, 1, &highbyte) || !_jpeg_read_fp(fp, 1, &lowbyte)) {
        _jpeg_dump_message(jfile, "unexpected end of file.");
        return false;
    }
    while (true) {
        /* checking if two bytes means a marker */
        if (highbyte == 0xFF) {
            if (lowbyte == EOI) { /* 0xFFD9 */
                return true; /* successfully read */
            }
            else if (lowbyte == 0x00) { /* 0xFF00 */
                /* put a 0xFF into bitstream and ignore 0x00 */
                BYTE data = 0xFF;
                jfile->hstream.append(data);
                if (!_jpeg_read_fp(fp, 1, &lowbyte)) {
                    _jpeg_dump_message(jfile, "unexpected end of file.");
                    return false;
                }
            }
            else if (lowbyte >= RST0 && lowbyte <= RST7) {
                /* skip restart marker as we already knew restart interval */
                if (!_jpeg_read_fp(fp, 1, &lowbyte)) {
                    _jpeg_dump_message(jfile, "unexpected end of file.");
                    return false;
                }
            }
            else if (lowbyte == 0xFF) { /* 0xFFFF */
                /* do nothing as multiple 0xFFs in a row means nothing */
                continue;
            }
            else {
                char buf[128];
                sprintf(buf, "invalid JPEG marker : '0xFF%02X'.\n", lowbyte);
                _jpeg_dump_message(jfile, buf);
                return false;
            }
        }
        else {
            jfile->hstream.append(highbyte);
        }
        /* read another byte from file */
        highbyte = lowbyte;
        if (!_jpeg_read_fp(fp, 1, &lowbyte)) {
            _jpeg_dump_message(jfile, "unexpected end of file.");
            return false;
        }
    }

    return true;
}

VEC3 _jpeg_YCbCr_to_RGB(VEC3 YCbCr) {
    REAL Y = YCbCr.x, Cb = YCbCr.y, Cr = YCbCr.z;
    REAL R, G, B;
    R = Y + REAL(1.402) * Cr + REAL(128);
    G = Y + REAL(-0.344) * Cb + REAL(-0.714) * Cr + REAL(128);
    B = Y + REAL(1.772) * Cb + REAL(128);
    return VEC3(R, G, B);
}
VEC3 _jpeg_RGB_to_YCbCr(VEC3 RGB) {
    REAL R = RGB.x - REAL(128), G = RGB.y - REAL(128), B = RGB.z - REAL(128);
    REAL Y, Cb, Cr;
    Y = REAL(0.299) * R + REAL(0.587) * G + REAL(0.114) * B;
    Cb = REAL(-0.1687) * R + REAL(-0.3313) * G + REAL(0.5) * B;
    Cr = REAL(0.5) * R + REAL(-0.4187) * G + REAL(-0.0813) * B;
    return VEC3(Y, Cb, Cr);
}

template <typename T>
BYTE _jpeg_byte_clamp(T value) {
    if (value <= T(0)) return BYTE(0);
    else if (value >= T(255)) return BYTE(255);
    else return BYTE(value);
}

INT_8x8 _jpeg_clamp_int8x8(INT_8x8 * a)
{
    INT_8x8 c;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int v = a->data[y][x];
            if (x == 0 && y == 0) { /* DC, 11 bits */
                if (v < -2048) v = -2048;
                if (v > 2047) v = 2047;
            }
            else { /* AC, 10 bits */
                if (v < -1024) v = -1024;
                if (v > 1023) v = 1023;
            }
            c.data[y][x] = v;
        }
    }
    return c;
}

/* generate code for each symbol in Huffman table */
void _jpeg_generate_huffman_codes(JPEG_HUFFMAN_TABLE* htable) {
    unsigned int code = 0;
    for (int i = 0; i < 16; i++) { /* codes are at most 16 bits long */
        for (int j = htable->offsets[i]; j < htable->offsets[i + 1]; j++) {
            htable->codes[j] = code;
            code++;
        }
        code <<= 1; /* append a zero to current code candidate */
    }
}

BYTE _jpeg_read_huffman_symbol(Bitstream* bs, JPEG_HUFFMAN_TABLE* htab) {
    unsigned int code = 0;
    for (int i = 0; i < 16; i++) {
        BYTE bit = bs->readBit();
        if (bit == 0xFF) 
            return 0xFF; /* bitstream empty */
        code = (code << 1) | bit;
        for (int j = htab->offsets[i]; j < htab->offsets[i + 1]; j++) {
            if (code == htab->codes[j]) {
                return htab->symbols[j];
            }
        }
    }
    return 0xFF; /* code not found */
}

bool _jpeg_decode_DCT_coeffs(JPEG_FILE* jfile, Bitstream* bit_reader, REAL_8x8* DCT_coeffs, int* prev_DC_coeff,
    JPEG_HUFFMAN_TABLE* dctab, JPEG_HUFFMAN_TABLE* actab) {

    if (DCT_coeffs == nullptr) return false;

    BYTE length = _jpeg_read_huffman_symbol(bit_reader, dctab);
    if (length > 11) {
        _jpeg_dump_message(jfile, "invalid Huffman table symbol.");
        return false; /* includes 0xFF */
    }

    int coeff = (int)(bit_reader->readBits(length));
    /* if length == 0, then coeff = 0, that is the only way we obtain 0 as coefficient. */
    if (coeff == -1) {
        _jpeg_dump_message(jfile, "invalid DC coefficient.");
        return false;
    }
    /* determine if the coefficient is negative */
    if (length != 0 && coeff < (1 << (length - 1))) {
        coeff -= (1 << length) - 1;
    }

    int coeffs[64];

    /* fill in DC coefficients */
    coeffs[0] = coeff + (*prev_DC_coeff); /* DC coefficients need to be relative to previous DC coefficients */
    (*prev_DC_coeff) = coeffs[0];

    /* fill 63 AC coefficients */
    int i = 1;
    while (i < 64) {
        BYTE symbol = _jpeg_read_huffman_symbol(bit_reader, actab);
        if (symbol == 0xFF) { /* invalid symbol */
            _jpeg_dump_message(jfile, "invalid Huffman table symbol.");
            return false;
        }
        else if (symbol == 0x00) { /* symbol 0x00 means all the rest components are 0 */
            for (int j = i; j < 64; j++)
                coeffs[j] = 0;
            _jpeg_zz_intarr_to_real8x8(coeffs, DCT_coeffs);
            return true;
        }
        else {
            BYTE nZ = (symbol >> 4);
            BYTE coeff_len = (symbol & 0x0F);
            /* symbol 0xF0 mean skip 16 zeros */
            if (symbol == 0xF0) {
                nZ = 16;
            }

            /* now we are going to add zeros to the end of the coefficients */
            if (i + nZ >= 64) {
                _jpeg_dump_message(jfile, "DCT coefficients is more than 64.");
                return false; /* out of range */
            }
            for (int j = 0; j < nZ; i++, j++) coeffs[i] = 0;

            if (coeff_len > 10) {
                _jpeg_dump_message(jfile, "AC coefficients can only have 10 bits length at maximum.");
                return false;
            }

            if (coeff_len != 0) { /* exclude 0xF0 */
                coeff = bit_reader->readBits(coeff_len);
                if (coeff == -1) {
                    _jpeg_dump_message(jfile, "invalid AC coefficient.");
                    return false;
                }
                if (coeff < (1 << (coeff_len - 1))) {
                    coeff -= (1 << coeff_len) - 1;
                }
                coeffs[i] = coeff;
                i++;
            }
        }
    }
    _jpeg_zz_intarr_to_real8x8(coeffs, DCT_coeffs);
    return true;
}

/* decode all the Huffman bitstream and fill them into all MCUs */
bool _jpeg_decode_MCUs(JPEG_FILE* jfile, int nW, int nH, JPEG_MCU* MCUs, int subsampling_type) {
    /* pointer to MCUs should be a valid contiguous memory space */

    /* decode MCU array */
    Bitstream bit_reader;
    bit_reader.load(jfile->hstream);
    int prev_DC_coeffs[4] = { 0 }; /* 4 channels at most */
    for (int i = 0; i < nW * nH; i++) { /* for each MCU in raster scan order */
        if (jfile->restart_interval != 0 && (i % jfile->restart_interval == 0)) {
            prev_DC_coeffs[0] = prev_DC_coeffs[1] = prev_DC_coeffs[2] = 0;
            bit_reader.alignRead();
        }
        if (subsampling_type == 1) { /* no subsampling */
            if (!_jpeg_decode_DCT_coeffs(jfile, &bit_reader, &(MCUs[i].Y0), &(prev_DC_coeffs[0]),
                &(jfile->dctabs[jfile->channels[0].dctab_id]), &(jfile->actabs[jfile->channels[0].actab_id])))
                return false;
        }
        else if (subsampling_type == 2 || subsampling_type == 3) { /* h/v subsampling */
            if (!_jpeg_decode_DCT_coeffs(jfile, &bit_reader, &(MCUs[i].Y0), &(prev_DC_coeffs[0]),
                &(jfile->dctabs[jfile->channels[0].dctab_id]), &(jfile->actabs[jfile->channels[0].actab_id])))
                return false;
            if (!_jpeg_decode_DCT_coeffs(jfile, &bit_reader, &(MCUs[i].Y1), &(prev_DC_coeffs[0]),
                &(jfile->dctabs[jfile->channels[0].dctab_id]), &(jfile->actabs[jfile->channels[0].actab_id])))
                return false;
        }
        else { /* h&v subsampling */
            if (!_jpeg_decode_DCT_coeffs(jfile, &bit_reader, &(MCUs[i].Y0), &(prev_DC_coeffs[0]),
                &(jfile->dctabs[jfile->channels[0].dctab_id]), &(jfile->actabs[jfile->channels[0].actab_id])))
                return false;
            if (!_jpeg_decode_DCT_coeffs(jfile, &bit_reader, &(MCUs[i].Y1), &(prev_DC_coeffs[0]),
                &(jfile->dctabs[jfile->channels[0].dctab_id]), &(jfile->actabs[jfile->channels[0].actab_id])))
                return false;
            if (!_jpeg_decode_DCT_coeffs(jfile, &bit_reader, &(MCUs[i].Y2), &(prev_DC_coeffs[0]),
                &(jfile->dctabs[jfile->channels[0].dctab_id]), &(jfile->actabs[jfile->channels[0].actab_id])))
                return false;
            if (!_jpeg_decode_DCT_coeffs(jfile, &bit_reader, &(MCUs[i].Y3), &(prev_DC_coeffs[0]),
                &(jfile->dctabs[jfile->channels[0].dctab_id]), &(jfile->actabs[jfile->channels[0].actab_id])))
                return false;
        }
        if (!_jpeg_decode_DCT_coeffs(jfile, &bit_reader, &(MCUs[i].Cb), &(prev_DC_coeffs[1]),
            &(jfile->dctabs[jfile->channels[1].dctab_id]), &(jfile->actabs[jfile->channels[1].actab_id])))
            return false;
        if (!_jpeg_decode_DCT_coeffs(jfile, &bit_reader, &(MCUs[i].Cr), &(prev_DC_coeffs[2]),
            &(jfile->dctabs[jfile->channels[2].dctab_id]), &(jfile->actabs[jfile->channels[2].actab_id])))
            return false;
    }
    return true;
}

bool _jpeg_dequantize_MCUs(JPEG_FILE* jfile, int nW, int nH, JPEG_MCU* MCUs, int subsampling_type) {
    for (int i = 0; i < nW * nH; i++) { /* for each MCU in raster scan order */
        if (subsampling_type >= 1) {
            MCUs[i].Y0 = _jpeg_real8x8_mul_int8x8(&(MCUs[i].Y0), &(jfile->qtabs[jfile->channels[0].qtab_id]));
        }
        if (subsampling_type >= 2) {
            MCUs[i].Y1 = _jpeg_real8x8_mul_int8x8(&(MCUs[i].Y1), &(jfile->qtabs[jfile->channels[0].qtab_id]));
        }
        if (subsampling_type >= 4) {
            MCUs[i].Y2 = _jpeg_real8x8_mul_int8x8(&(MCUs[i].Y2), &(jfile->qtabs[jfile->channels[0].qtab_id]));
            MCUs[i].Y3 = _jpeg_real8x8_mul_int8x8(&(MCUs[i].Y3), &(jfile->qtabs[jfile->channels[0].qtab_id]));
        }
        MCUs[i].Cb = _jpeg_real8x8_mul_int8x8(&(MCUs[i].Cb), &(jfile->qtabs[jfile->channels[1].qtab_id]));
        MCUs[i].Cr = _jpeg_real8x8_mul_int8x8(&(MCUs[i].Cr), &(jfile->qtabs[jfile->channels[2].qtab_id]));
    }
    return true;
}

/* apply IDCT to dequantized MCUs, to obtain the actual Y, Cb and Cr values */
bool _jpeg_IDCT_MCUs(int nW, int nH, JPEG_MCU* MCUs, int subsampling_type) {

    for (int i = 0; i < nW * nH; i++) { /* for each MCU in raster scan order */
        if (subsampling_type >= 1)
            IDCT8x8_fast(&(MCUs[i].Y0));
        if (subsampling_type >= 2)
            IDCT8x8_fast(&(MCUs[i].Y1));
        if (subsampling_type >= 4) {
            IDCT8x8_fast(&(MCUs[i].Y2));
            IDCT8x8_fast(&(MCUs[i].Y3));
        }
        IDCT8x8_fast(&(MCUs[i].Cb));
        IDCT8x8_fast(&(MCUs[i].Cr));
    }
    return true;
}

bool _jpeg_decode_color(JPEG_FILE* jfile, int nW, int nH, JPEG_MCU* MCUs, int subsampling_type, RAW_IMAGE** image_ptr)
{
    FixedArray2D<BYTE> image_plane[3]; /* R,G,B */
    FixedArray2D<BYTE> MCU_plane[3]; /* R,G,B */

    int MCU_width, MCU_height;

    /* create image and MCU plane */

    if (subsampling_type == 1) { MCU_width = 8; MCU_height = 8; }
    else if (subsampling_type == 2) { MCU_width = 16; MCU_height = 8; }
    else if (subsampling_type == 3) { MCU_width = 8; MCU_height = 16; }
    else { MCU_width = 16; MCU_height = 16; }

    image_plane[0].create(jfile->image_width, jfile->image_height);
    image_plane[1].create(jfile->image_width, jfile->image_height);
    image_plane[2].create(jfile->image_width, jfile->image_height);
    MCU_plane[0].create(MCU_width, MCU_height);
    MCU_plane[1].create(MCU_width, MCU_height);
    MCU_plane[2].create(MCU_width, MCU_height);

    /* convert each MCU and paste to image plane */

    for (int j = 0; j < nH; j++) {
        for (int i = 0; i < nW; i++) {

            /* for each MCU, calculate RGB values based on subsampling type */

            if (subsampling_type == 1) {
                for (int y = 0; y < 8; y++) {
                    for (int x = 0; x < 8; x++) {
                        REAL Y = REAL(MCUs[j*nW + i].Y0.data[y][x]);
                        REAL Cb = REAL(MCUs[j*nW + i].Cb.data[y][x]);
                        REAL Cr = REAL(MCUs[j*nW + i].Cr.data[y][x]);
                        VEC3 RGB = _jpeg_YCbCr_to_RGB(VEC3(Y, Cb, Cr));
                        MCU_plane[0].at(x, y) = _jpeg_byte_clamp(RGB.x);
                        MCU_plane[1].at(x, y) = _jpeg_byte_clamp(RGB.y);
                        MCU_plane[2].at(x, y) = _jpeg_byte_clamp(RGB.z);
                    }
                }
            }
            else if (subsampling_type == 2) {
                /* horizontal subsampling (w16 x h8) */
                for (int y = 0; y < 8; y++) {
                    for (int x = 0; x < 16; x++) {
                        REAL Y;
                        if (x < 8) Y = REAL(MCUs[j*nW + i].Y0.data[y][x]);
                        else Y = REAL(MCUs[j*nW + i].Y1.data[y][x - 8]);
                        REAL Cb = REAL(MCUs[j*nW + i].Cb.data[y][x / 2]);
                        REAL Cr = REAL(MCUs[j*nW + i].Cr.data[y][x / 2]);
                        MCU_plane[0].at(x, y) = _jpeg_byte_clamp(Y + REAL(1.402) * Cr + 128);
                        MCU_plane[1].at(x, y) = _jpeg_byte_clamp(Y - REAL(0.344) * Cb - REAL(0.714) * Cr + 128);
                        MCU_plane[2].at(x, y) = _jpeg_byte_clamp(Y + REAL(1.772) * Cb + 128);
                    }
                }
            }
            else if (subsampling_type == 3) {
                /* vertical subsampling (w8 x h16) */
                for (int y = 0; y < 16; y++) {
                    for (int x = 0; x < 8; x++) {
                        REAL Y;
                        if (y < 8) Y = REAL(MCUs[j*nW + i].Y0.data[y][x]);
                        else Y = REAL(MCUs[j*nW + i].Y1.data[y - 8][x]);
                        REAL Cb = REAL(MCUs[j*nW + i].Cb.data[y / 2][x]);
                        REAL Cr = REAL(MCUs[j*nW + i].Cr.data[y / 2][x]);
                        MCU_plane[0].at(x, y) = _jpeg_byte_clamp(Y + REAL(1.402) * Cr + 128);
                        MCU_plane[1].at(x, y) = _jpeg_byte_clamp(Y - REAL(0.344) * Cb - REAL(0.714) * Cr + 128);
                        MCU_plane[2].at(x, y) = _jpeg_byte_clamp(Y + REAL(1.772) * Cb + 128);
                    }
                }
            }
            else {
                /* horizontal and vertical subsampling (w16 x h16) */
                for (int y = 0; y < 16; y++) {
                    for (int x = 0; x < 16; x++) {
                        REAL Y;
                        if (x < 8 && y < 8) Y = REAL(MCUs[j*nW + i].Y0.data[y][x]);
                        else if (x >= 8 && y < 8) Y = REAL(MCUs[j*nW + i].Y1.data[y][x - 8]);
                        else if (x < 8 && y >= 8) Y = REAL(MCUs[j*nW + i].Y2.data[y - 8][x]);
                        else Y = REAL(MCUs[j*nW + i].Y3.data[y - 8][x - 8]);
                        REAL Cb = REAL(MCUs[j*nW + i].Cb.data[y / 2][x / 2]);
                        REAL Cr = REAL(MCUs[j*nW + i].Cr.data[y / 2][x / 2]);
                        MCU_plane[0].at(x, y) = _jpeg_byte_clamp(Y + REAL(1.402) * Cr + 128);
                        MCU_plane[1].at(x, y) = _jpeg_byte_clamp(Y - REAL(0.344) * Cb - REAL(0.714) * Cr + 128);
                        MCU_plane[2].at(x, y) = _jpeg_byte_clamp(Y + REAL(1.772) * Cb + 128);
                    }
                }
            }

            /* paste the RGB values to the image plane */

            int xoffset = i * MCU_width, yoffset = j * MCU_height;
            MCU_plane[0].paste_to(image_plane[0], xoffset, yoffset);
            MCU_plane[1].paste_to(image_plane[1], xoffset, yoffset);
            MCU_plane[2].paste_to(image_plane[2], xoffset, yoffset);

        }
    }

    /* allocate image and dump data */

    if (!alloc_image(jfile->image_width, jfile->image_height, image_ptr)) {
        _jpeg_dump_message(jfile, "cannot allocate image storage space, maybe the image is too large.");
        return false;
    }

    int plane_bytes = sizeof(BYTE) * jfile->image_width * jfile->image_height;
    memcpy((*image_ptr)->r, image_plane[0].data(), plane_bytes);
    memcpy((*image_ptr)->g, image_plane[1].data(), plane_bytes);
    memcpy((*image_ptr)->b, image_plane[2].data(), plane_bytes);

    return true;
}

/* the JPEG main reading function */
bool _jpeg_read_file(JPEG_FILE* jfile, FILE* fp) {

    /* initialize header information */
    jfile->image_data = NULL;
    jfile->is_valid = false;
    jfile->zero_start = false; /* default */
    jfile->image_width = 0;
    jfile->image_height = 0;
    memset(jfile->message, 0, _JPEG_MSG_LEN);
    for (int i = 0; i < 4; i++) {
        jfile->channels[i].is_used = false;
    }
    jfile->restart_interval = 0;
    for (int i = 0; i < 4; i++) {
        jfile->actabs[i].is_used = false;
        jfile->dctabs[i].is_used = false;
        for (int j = 0; j < 17; j++) {
            jfile->actabs[i].offsets[j] = 0;
            jfile->dctabs[i].offsets[j] = 0;
        }
        for (int j = 0; j < 162; j++) {
            jfile->actabs[i].symbols[j] = 0;
            jfile->dctabs[i].symbols[j] = 0;
        }
    }

    BYTE marker[2];
    /* JPEG markers are two bytes long */
    _jpeg_read_fp(fp, 2, marker); /* from fp read 2 bytes to marker */

    if (marker[0] != 0xFF || marker[1] != SOI) {
        _jpeg_dump_message(jfile, "invalid JPEG image marker.");
        return false;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    /* read file in a while loop until EOI marker or EOF is reached  */
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    bool success = false;
    while (true) {
        if (_jpeg_read_fp(fp, 2, marker) == false) {
            _jpeg_dump_message(jfile, "unexpected end of file.");
            return false;
        }
        /* now marker is read, check if it is a valid marker */
        if (marker[0] != 0xFF) {
            _jpeg_dump_message(jfile, "invalid JPEG image marker.");
            return false;
        }
        if (marker[1] == 0xFF) {
            /* any number of 0xFF in a row is allowed and should be ignored */
            while (marker[1] == 0xFF) {
                if (!_jpeg_read_fp(fp, 1, &(marker[1]))) {
                    _jpeg_dump_message(jfile, "invalid JPEG image marker.");
                    return false;
                }
            }
            /* read until non 0xFF values are met and file is not ended */
        }
        /* parse markers */
        if (marker[1] >= APP0 && marker[1] <= APP15) {
            printf("JPEG DEBUG: reading APPN.\n");
            if (!_jpeg_read_APPN(fp, jfile)) {
                _jpeg_dump_message(jfile, "invalid JPEG image marker.");
                return false;
            }
        }
        else if (marker[1] >= JPG0 && marker[1] <= JPG13) {
            printf("JPEG DEBUG: read JPGN marker.\n");
            if (!_jpeg_read_JPGN(fp, jfile)) {
                _jpeg_dump_message(jfile, "invalid JPEG image marker.");
                return false;
            }
        }
        else if (marker[1] == DNL || marker[1] == DHP || marker[1] == EXP) {
            printf("JPEG DEBUG: read misc marker.\n");
            if (!_jpeg_read_NONE(fp, jfile)) {
                _jpeg_dump_message(jfile, "invalid JPEG image marker.");
                return false;
            }
        }
        else if (marker[1] == TEM) {
            continue;
        }
        else if (marker[1] == DAC) {
            _jpeg_dump_message(jfile, "arithmetic encoding is not supported.");
            return false;
        }
        else if (marker[1] == SOI) {
            _jpeg_dump_message(jfile, "embedded JPEG file format is not supported.");
            return false;
        }
        else if (marker[1] == DQT) {
            printf("JPEG DEBUG: reading quantization tables.\n");
            /* FF DB
               XX XX
               [YY ZZ ... ZZ] x N
            */
            /* XX XX: length including the size indicator */
            /* YY: table info
                   upper 4 bit : 0 - 8 bit quantization table
                                 1 - 16 bit quantization table
                   lower 4 bit : 0~3 - table ID
               ZZ: quantization table data
               64 bytes for 8 bit QT and 128 bytes for 16 bit QT
            */
            if (!_jpeg_read_QTAB(fp, jfile)) {
                _jpeg_dump_message(jfile, "invalid quantization table.");
                return false;
            }
        }
        else if (marker[1] == SOF0) {
            printf("JPEG DEBUG: reading start of frame.\n");
            /*
            FF CX : C0~C15, marker
            XX XX : length (including this 2 bytes)
            XX    : precision, how many bits used in each color channel (must be 8)
            XX XX : image height
            XX XX : image width
            XX    : number of channels (1/3/4, 1: grayscale - luminance
                                               3: color data - Y Cb Cr
                                               4: CMYK JPEG)
            {
            XX    : channel ID (1/2/3/4)
            XX    : sampling factors
            XX    : quantization table ID used for this channel
            } x N
            */
            if (!_jpeg_read_SOF0(fp, jfile)) {
                _jpeg_dump_message(jfile, "corrupted JPEG SOF0 marker.");
                return false;
            }
        }
        else if (marker[1] == SOF2) {
            _jpeg_dump_message(jfile, "progressive JPEGs are not supported.");
            return false;
        }
        else if (marker[1] == SOF1 || marker[1] == SOF3 || (marker[1] >= SOF5 && marker[1] <= SOF7) ||
            (marker[1] >= SOF9 && marker[1] <= SOF11) || (marker[1] >= SOF13 && marker[1] <= SOF15)) {
            _jpeg_dump_message(jfile, "unsupported JPEG frame type.");
            return false;
        }
        else if (marker[1] == DRI) {
            /* define restart interval for the DC coefficient in the MCU */
            printf("JPEG DEBUG: define restart interval.\n");
            /*
            FF DD
            00 04 : length = 4
            XX XX : restart interval
            */
            if (!_jpeg_read_DRI(fp, jfile)) {
                _jpeg_dump_message(jfile, "invalid JPEG image marker.");
                return false;
            }
        }
        else if (marker[1] == DHT) {
            printf("JPEG DEBUG: define Huffman tables.\n");
            /*
            FF C4 : marker
            XX XX : length
            ------------
            Huffman symbols: decoding 8x8 MCU coefficients
            each symbol (8bit) "AB": 'A' - how many zeros are preceding the current coefficient
                                           (1~15)
                                     'B' - length of the coefficients that are trying to decode
                                           (number of bits, 1~10 for AC coefficients and 1~11 for
                                           DC coefficients)
            special case: "F0" -> just skip 16 zeros
            special case: "00" -> end of MCU coefficients
                                  (all zeros for all subsequent coefficients)
            ------------
            {
            XY    : table info , X - 0/1 DC/AC Huffman table
                                 Y - 0~3 table ID
            [16 bytes] : # of codes of each length
            [X bytes]  : actual symbols
            } x N
            */
            if (!_jpeg_read_DHT(fp, jfile)) {
                _jpeg_dump_message(jfile, "invalid JPEG image marker.");
                return false;
            }
        }
        else if (marker[1] == SOS) {
            /* start of scan, defines the actual data in each MCU */
            /*
            FF DA
            XX XX : length
            XX    : # of channels
            {
            XX    : channel ID
            XY    : X/Y - DC/AC Huffman table ID
            } x N, N = # of channels

            * below is used in progressive jpegs only, should be the same
            with all baseline jpegs
            -----------------------------------------
            XX    : start of selection (baseline=0)
            XX    : end of selection (baseline=63)
            XY    : successive approximation (baseline X=0,Y=0)

            * then, the Huffman encoded bitstream
            [...] : Huffman encoded bitstream
            NOTE: markers can show up in bitstream
            such as RST0~RST7, just skip them
            */
            if (!_jpeg_read_SOS(fp, jfile)) {
                _jpeg_dump_message(jfile, "invalid JPEG image marker.");
                return false;
            }
            return true;
        }
        else if (marker[1] == EOI) {
            _jpeg_dump_message(jfile, "unexpected end of image (EOI) marker.");
            return false;
        }
        else {
            printf("JPEG warning: unhandled marker '0xFF%02X'.\n", marker[1]);
            if (!_jpeg_read_NONE(fp, jfile)) {
                _jpeg_dump_message(jfile, "invalid JPEG image marker.");
                return false;
            }
        }
    }

    return false; /* this line should not be reached */
}


/* encode JPEG MCU coefficients to JPEG bitstream */
void _jpeg_encode_integer(int i, Bitstream* _bitstream) {
    /*
    encode   6 to bitstream "110"
    encode  -6 to bitstream "001"
    encode  17 to bitstream "10001"
    encode -17 to bitstream "01110"
    */

    if (i == 0) {
        /* encode DC coefficient "0" */
        _bitstream->appendBit(0);
        return;
    }

    bool negative = (i < 0);
    unsigned int t = (unsigned int)(negative ? -i : i);
    /* guess how many bits are needed in order to represent this integer */
    unsigned int bits = 0;
    while (t != 0) {
        t >>= 1;
        bits++;
    }
    /* convert to bitstream */
    unsigned int mask = ((1 << bits) - 1); /* 17 -> 00010001 -> mask = 00011111 */
    t = (unsigned int)(negative ? -i : i);
    if (negative)
        t = ((~t) & mask);
    _bitstream->appendBits(t, bits);
}

/* run forward JPEG encoding */
void _jpeg_forward_compression(RAW_IMAGE* image, JPEG_SAVE_OPTION* option, int restart_interval,
    FixedArray2D<JPEG_MCU_QCOEFF>* qCoeffs) {

    /* assume 2x2 subsampling for chrominance channels, 8 bit depth, baseline JPEG encoding */

    /* pad image */
    int padded_width = (image->w + 15) / 16 * 16;
    int padded_height = (image->h + 15) / 16 * 16;
    FixedArray2D<REAL> Y, Cb, Cr; /* use REAL to avoid rounding error */
    Y.create(padded_width, padded_height);
    Cb.create(padded_width, padded_height);
    Cr.create(padded_width, padded_height);

    /* convert RGB to YCbCr */
    for (int y = 0; y < padded_height; y++) {
        for (int x = 0; x < padded_width; x++) {
            VEC3 rgb;
            if (x >= image->w || y >= image->h)
                rgb = VEC3(0, 0, 0); /* set padded pixels to black */
            else
                rgb = VEC3(
                    REAL(image->r[y*image->w + x]),
                    REAL(image->g[y*image->w + x]),
                    REAL(image->b[y*image->w + x]));
            VEC3 ycbcr = _jpeg_RGB_to_YCbCr(rgb);
            Y.at(x, y) = ycbcr.x; Cb.at(x, y) = ycbcr.y; Cr.at(x, y) = ycbcr.z;
        }
    }

    /* generate and fill MCUs (chroma subsampling) */
    int nW = padded_width / 16;
    int nH = padded_height / 16;
    FixedArray2D<JPEG_MCU> all_MCUs;
    all_MCUs.create(nW, nH);
    for (int mcu_y = 0; mcu_y < nH; mcu_y++) {
        for (int mcu_x = 0; mcu_x < nW; mcu_x++) {
            int x_start = mcu_x * 16; int x_end = (mcu_x + 1) * 16;
            int y_start = mcu_y * 16; int y_end = (mcu_y + 1) * 16;
            for (int y = 0; y < 16; y++) {
                for (int x = 0; x < 16; x++) {
                    int x_cur = x_start + x;
                    int y_cur = y_start + y;
                    if (x < 8 && y < 8) {
                        all_MCUs.at(mcu_x, mcu_y).Y0.data[y][x] = Y.at(x_cur, y_cur);
                        all_MCUs.at(mcu_x, mcu_y).Cb.data[y][x] = Cb.at(x_start + 2 * x, y_start + 2 * y);
                        all_MCUs.at(mcu_x, mcu_y).Cr.data[y][x] = Cr.at(x_start + 2 * x, y_start + 2 * y);
                    }
                    else if (x >= 8 && y < 8)
                        all_MCUs.at(mcu_x, mcu_y).Y1.data[y][x - 8] = Y.at(x_cur, y_cur);
                    else if (x < 8 && y >= 8)
                        all_MCUs.at(mcu_x, mcu_y).Y2.data[y - 8][x] = Y.at(x_cur, y_cur);
                    else
                        all_MCUs.at(mcu_x, mcu_y).Y3.data[y - 8][x - 8] = Y.at(x_cur, y_cur);
                }
            }
        }
    }

    /* DCT, quantization, run length encoding */
    qCoeffs->create(nW, nH);
    for (int mcu_y = 0; mcu_y < nH; mcu_y++) {
        for (int mcu_x = 0; mcu_x < nW; mcu_x++) {
            qCoeffs->at(mcu_x, mcu_y).Y0 = _jpeg_quantize_real8x8(DCT8x8_fast(&(all_MCUs.at(mcu_x, mcu_y).Y0)), &(option->qtab_Y));
            qCoeffs->at(mcu_x, mcu_y).Y1 = _jpeg_quantize_real8x8(DCT8x8_fast(&(all_MCUs.at(mcu_x, mcu_y).Y1)), &(option->qtab_Y));
            qCoeffs->at(mcu_x, mcu_y).Y2 = _jpeg_quantize_real8x8(DCT8x8_fast(&(all_MCUs.at(mcu_x, mcu_y).Y2)), &(option->qtab_Y));
            qCoeffs->at(mcu_x, mcu_y).Y3 = _jpeg_quantize_real8x8(DCT8x8_fast(&(all_MCUs.at(mcu_x, mcu_y).Y3)), &(option->qtab_Y));
            qCoeffs->at(mcu_x, mcu_y).Cb = _jpeg_quantize_real8x8(DCT8x8_fast(&(all_MCUs.at(mcu_x, mcu_y).Cb)), &(option->qtab_CbCr));
            qCoeffs->at(mcu_x, mcu_y).Cr = _jpeg_quantize_real8x8(DCT8x8_fast(&(all_MCUs.at(mcu_x, mcu_y).Cr)), &(option->qtab_CbCr));
            qCoeffs->at(mcu_x, mcu_y).Y0 = _jpeg_clamp_int8x8(&(qCoeffs->at(mcu_x, mcu_y).Y0));
            qCoeffs->at(mcu_x, mcu_y).Y1 = _jpeg_clamp_int8x8(&(qCoeffs->at(mcu_x, mcu_y).Y1));
            qCoeffs->at(mcu_x, mcu_y).Y2 = _jpeg_clamp_int8x8(&(qCoeffs->at(mcu_x, mcu_y).Y2));
            qCoeffs->at(mcu_x, mcu_y).Y3 = _jpeg_clamp_int8x8(&(qCoeffs->at(mcu_x, mcu_y).Y3));
            qCoeffs->at(mcu_x, mcu_y).Cb = _jpeg_clamp_int8x8(&(qCoeffs->at(mcu_x, mcu_y).Cb));
            qCoeffs->at(mcu_x, mcu_y).Cr = _jpeg_clamp_int8x8(&(qCoeffs->at(mcu_x, mcu_y).Cr));
        }
    }

    /* remember the DC coefficient is relative */
    for (int i = 0; i < nW*nH; i++) {
        if (i%restart_interval != 0)
            qCoeffs->data()[i].Y0_diff = qCoeffs->data()[i - 1].Y3.data[0][0];
        else
            qCoeffs->data()[i].Y0_diff = 0;
        qCoeffs->data()[i].Y1_diff = qCoeffs->data()[i].Y0.data[0][0];
        qCoeffs->data()[i].Y2_diff = qCoeffs->data()[i].Y1.data[0][0];
        qCoeffs->data()[i].Y3_diff = qCoeffs->data()[i].Y2.data[0][0];
        if (i%restart_interval != 0) { /* not restart, calculate relative DC value change */
            qCoeffs->data()[i].Cb_diff = qCoeffs->data()[i - 1].Cb.data[0][0];
            qCoeffs->data()[i].Cr_diff = qCoeffs->data()[i - 1].Cr.data[0][0];
        }
        else {
            qCoeffs->data()[i].Cb_diff = 0;
            qCoeffs->data()[i].Cr_diff = 0;
        }
    }
    for (int i = 0; i < nW*nH; i++) {
        qCoeffs->data()[i].Y0.data[0][0] -= qCoeffs->data()[i].Y0_diff;
        qCoeffs->data()[i].Y1.data[0][0] -= qCoeffs->data()[i].Y1_diff;
        qCoeffs->data()[i].Y2.data[0][0] -= qCoeffs->data()[i].Y2_diff;
        qCoeffs->data()[i].Y3.data[0][0] -= qCoeffs->data()[i].Y3_diff;
        qCoeffs->data()[i].Cb.data[0][0] -= qCoeffs->data()[i].Cb_diff;
        qCoeffs->data()[i].Cr.data[0][0] -= qCoeffs->data()[i].Cr_diff;
    }

}

/* some RLE triples may represent multiple symbols, we use this function */
/* to split them into symbols that are allowed in JPEG encoding standard */
void _jpeg_RLE_parse_AC_triplets(Array<JPEG_RLE_TRIPLET>* triplets)
{
    for (int i = 0; i < triplets->size(); i++) {
        //printf("(nZ=%d, L=%d, v=%d)\n", AC_RLE_pairs[i].nZ, AC_RLE_pairs[i].l, AC_RLE_pairs[i].v);
        BYTE AC_symbol;
        int nZ = triplets->at(i).nZ,
            l = triplets->at(i).l,
            v = triplets->at(i).v;
        if (v == 0) {
            AC_symbol = 0x00;
            triplets->at(i).symbols.append(AC_symbol);
            //printf("AC_symbol newly added: 0x%02X\n", AC_symbol);
        }
        else if (nZ >= 16) {
            while (nZ >= 16) {
                AC_symbol = 0xF0; /* split multiple 0xF0's */
                triplets->at(i).symbols.append(AC_symbol);
                //printf("AC_symbol newly added: 0x%02X\n", AC_symbol);
                nZ -= 16;
            }
            AC_symbol = ((nZ & 0x0F) << 4) | (l & 0x0F);
            triplets->at(i).symbols.append(AC_symbol);
            //printf("AC_symbol newly added: 0x%02X\n", AC_symbol);

        }
        else {
            AC_symbol = ((nZ & 0x0F) << 4) | (l & 0x0F);
            triplets->at(i).symbols.append(AC_symbol);
            //printf("AC_symbol newly added: 0x%02X\n", AC_symbol);
        }

        if (triplets->at(i).symbols.size() == 0)
            printf("assertion error! AC symbol is empty.\n");
    }
}

bool _jpeg_RLE_collect_AC_triplets(int coeffs[64], int* index, Array<JPEG_RLE_TRIPLET>* triplets) {
    /* returns the number of zeros */

    int _d_coeffs[64];
    memcpy(_d_coeffs, coeffs, 64 * 4);

    JPEG_RLE_TRIPLET triplet;

    int nZ = 0;
    int current_coeff = coeffs[*index];
    while (current_coeff == 0) {
        nZ++;
        (*index)++;
        if (*index >= 64) {
            triplet.nZ = nZ;
            triplet.l = triplet.v = 0;
            triplets->append(triplet);
            return false;
        }
        current_coeff = coeffs[*index];
    }
    (*index)++;

    Bitstream bs;
    _jpeg_encode_integer(current_coeff, &bs);
    triplet.nZ = nZ;
    triplet.l = bs.size();
    triplet.v = current_coeff;
    triplets->append(triplet);
    if (*index >= 64)
        return false; /* finished parsing */
    else
        return true;
}

/* collect huffman symbols from run length encoded coefficients */
void _jpeg_RLE_collect_symbols(int coeffs[64],
    Array<BYTE>* DC_symbols, Array<BYTE>* AC_symbols) {

    //printf("=====================\n");

    /* fill DC */
    Bitstream _bs;
    BYTE DC_symbol;
    if (coeffs[0] == 0)
        DC_symbol = 0x00;
    else {
        _jpeg_encode_integer(coeffs[0], &_bs);
        DC_symbol = (0x0F & (_bs.size()));
    }
    DC_symbols->append(DC_symbol);
    //printf("DC_symbol newly added: 0x%02X\n", DC_symbol);

    /* fill AC */
    int index = 1;
    Array<JPEG_RLE_TRIPLET> AC_RLE_triplets;
    while (_jpeg_RLE_collect_AC_triplets(coeffs, &index, &AC_RLE_triplets));
    _jpeg_RLE_parse_AC_triplets(&AC_RLE_triplets);
    for (int i = 0; i < AC_RLE_triplets.size(); i++) {
        for (int j = 0; j < AC_RLE_triplets[i].symbols.size(); j++) {
            BYTE AC_symbol = AC_RLE_triplets[i].symbols[j];
            AC_symbols->append(AC_symbol);
        }
    }

}

/* convert coefficients to bitstream using RLE and Huffman encoding */
void _jpeg_RLE_as_bitstream(int coeffs[64],
    Array<BYTE>* DC_symbols, Array<Bitstream>* DC_codes, /* DC huffman table */
    Array<BYTE>* AC_symbols, Array<Bitstream>* AC_codes, /* AC huffman table */
    Bitstream* bs) {

    /* enocde DC */
    Bitstream DC_bs;
    if (coeffs[0] == 0) {
        bool found = false;
        for (int i = 0; i < DC_symbols->size(); i++) {
            if (DC_symbols->at(i) == 0x00) {
                bs->appendBitstream(DC_codes->at(i), DC_codes->at(i).size());
                //if (_DEBUG) bs->_print(), printf("\n");
                found = true;
                break;
            }
        }
        if (!found)
            printf("assertion error! cannot find bitstring for DC symbol 0x00.\n");
    }
    else {
        _jpeg_encode_integer(coeffs[0], &DC_bs);
        BYTE DC_symbol = (0x0F & DC_bs.size());
        bool found = false;
        for (int i = 0; i < DC_symbols->size(); i++) {
            if (DC_symbols->at(i) == DC_symbol) {
                bs->appendBitstream(DC_codes->at(i), DC_codes->at(i).size());
                //if (_DEBUG) bs->_print(), printf("\n");
                bs->appendBitstream(DC_bs, DC_bs.size());
                //if (_DEBUG) bs->_print(), printf("\n");
                found = true;
                break;
            }
        }
        if (!found)
            printf("assertion error! cannot find bitstring for DC symbol 0x%02X.\n", DC_symbol);
    }

    /* encode AC */
    int index = 1;
    Array<JPEG_RLE_TRIPLET> AC_RLE_triplets;
    while (_jpeg_RLE_collect_AC_triplets(coeffs, &index, &AC_RLE_triplets));
    _jpeg_RLE_parse_AC_triplets(&AC_RLE_triplets);
    for (int i = 0; i < AC_RLE_triplets.size(); i++) {
        int nZ = AC_RLE_triplets[i].nZ, l = AC_RLE_triplets[i].l, v = AC_RLE_triplets[i].v;
        for (int j = 0; j < AC_RLE_triplets[i].symbols.size(); j++) {
            BYTE AC_symbol = AC_RLE_triplets[i].symbols[j];
            //if (AC_symbol == 0x05) __debugbreak();
            bool found = false;
            for (int k = 0; k < AC_symbols->size(); k++) {
                if (AC_symbol == AC_symbols->at(k)) { /* found code */
                    bs->appendBitstream(AC_codes->at(k), AC_codes->at(k).size());
                    //if (_DEBUG) bs->_print(), printf("\n");
                    found = true;
                    break;
                }
            }
            if (!found)
                printf("assertion error! cannot find bitstring for AC symbol 0x%02X.\n", AC_symbol);
            if (AC_symbol != 0x00 && AC_symbol != 0xF0) {
                /* encode coefficient if AC symbol is not special */
                Bitstream v_bs;
                _jpeg_encode_integer(v, &v_bs);
                bs->appendBitstream(v_bs, v_bs.size());
                //if (_DEBUG) bs->_print(), printf("\n");
            }
        }
    }
}

/* adjust the number of codes at each code length so that all */
/* Huffman code length is not longer than 16 bits. */
/* Reference: */
/* ITU-T T.81 (1992) | ISO/IEC 10918-1:1994 , Figure K.3 */
/* https://www.w3.org/Graphics/JPEG/itu-t81.pdf, page 147 */
void _jpeg_adjust_bins(Array<int>* bins) {
    int I = bins->size() - 1, J;
    while (true) {
        if (bins->at(I) > 0) {
            J = I - 1;
            do {
                J--;
            } while (bins->at(J) == 0);
            bins->at(I) -= 2;
            bins->at(I - 1) += 1;
            bins->at(J + 1) += 2;
            bins->at(J) -= 1;
            continue;
        }
        else {
            I--;
            if (I != 16)
                continue;
            else {
                while (bins->at(I) == 0)
                    I--;
                bins->at(I)--;
                return;
            }
        }
    }
}

/* recalculate mappings. This applies to some special cases */
/* where you slightly adjusted the tree structure and still need */
/* a valid mapping. */
void _jpeg_reassign_huffman_code(Array<BYTE>* symbols, Array<int> bins, Array<Bitstream>* codes) {
    codes->clear();
    unsigned int current_code = 0;
    int bits = 1;
    int clen;
    for (int i = 1; i < bins.size(); i++) {
        if (bins[i] != 0) {
            clen = i;
            break;
        }
        else
            bits++;
    }
    for (int i = clen; i <= 16; i++) {
        while (bins[i]) {
            Bitstream bs;
            bs.appendBits(current_code, bits);
            codes->append(bs);
            bins[i]--;
            current_code++;
        }
        current_code <<= 1;
        bits++;
    }
}

/* generate Huffman DC table during forward compression */
void _jpeg_generate_huffman_tables(FixedArray2D<JPEG_MCU_QCOEFF>* qCoeffs,
    Array<BYTE>* _DC_symbols, Array<Bitstream>* _DC_codes, Array<int>* _DC_bins,
    Array<BYTE>* _AC_symbols, Array<Bitstream>* _AC_codes, Array<int>* _AC_bins) {

    int nW = qCoeffs->sizeX();
    int nH = qCoeffs->sizeY();

    /* do run length encoding and collect all symbols */
    HuffmanTree<BYTE> DC_htree, AC_htree;
    Array<BYTE>       DC_symbols, AC_symbols;
    Array<Bitstream>  DC_codes, AC_codes;
    for (int i = 0; i < nW * nH; i++) { /* collect all DC/AC Huffman symbols */
        int coeffs[64];
        _jpeg_zz_int8x8_to_intarr(&(qCoeffs->data()[i].Y0), coeffs);
        _jpeg_RLE_collect_symbols(coeffs, &DC_symbols, &AC_symbols);
        _jpeg_zz_int8x8_to_intarr(&(qCoeffs->data()[i].Y1), coeffs);
        _jpeg_RLE_collect_symbols(coeffs, &DC_symbols, &AC_symbols);
        _jpeg_zz_int8x8_to_intarr(&(qCoeffs->data()[i].Y2), coeffs);
        _jpeg_RLE_collect_symbols(coeffs, &DC_symbols, &AC_symbols);
        _jpeg_zz_int8x8_to_intarr(&(qCoeffs->data()[i].Y3), coeffs);
        _jpeg_RLE_collect_symbols(coeffs, &DC_symbols, &AC_symbols);
        _jpeg_zz_int8x8_to_intarr(&(qCoeffs->data()[i].Cb), coeffs);
        _jpeg_RLE_collect_symbols(coeffs, &DC_symbols, &AC_symbols);
        _jpeg_zz_int8x8_to_intarr(&(qCoeffs->data()[i].Cr), coeffs);
        _jpeg_RLE_collect_symbols(coeffs, &DC_symbols, &AC_symbols);
    }
    /* build Huffman trees from all symbols */
    for (int i = 0; i < DC_symbols.size(); i++) {
        DC_htree.recordSymbol(DC_symbols[i]);
        //printf("0x%02X ", DC_symbols[i]);
    }
    for (int i = 0; i < AC_symbols.size(); i++) {
        AC_htree.recordSymbol(AC_symbols[i]);
        //printf("0x%02X ", AC_symbols[i]);
    }
    /* NOTE 1: JPEG standard does not allow symbol with all "1" code (such as "11111", "111") */
    /* so that padding bits added at the end of a compressed segment can't look like a valid code */
    /* we need to add one symbol that is never used normally and set its frequency to 1 */
    /* After building the Huffman tree we can manually delete this symbol */
    BYTE dummy = 0xFF;
    DC_htree.recordSymbol(dummy);
    AC_htree.recordSymbol(dummy);
    /* after inserted the reserved symbol, build trees */
    DC_htree.buildTree();
    AC_htree.buildTree();
    DC_symbols.clear(); AC_symbols.clear(); /* clear lists for further use */
    /* retrieve the symbol mappings */
    DC_htree.enumSymbols(&DC_symbols, &DC_codes);
    AC_htree.enumSymbols(&AC_symbols, &AC_codes);
    /* remap, to ensure the 0xFF symbol code is all "1" */
    for (int i = 0; i < DC_symbols.size(); i++) {
        if (DC_symbols[i] == 0xFF) {
            BYTE last_symbol = DC_symbols[DC_symbols.size() - 1];
            DC_symbols[DC_symbols.size() - 1] = DC_symbols[i];
            DC_symbols[i] = last_symbol;
        }
    }
    for (int i = 0; i < AC_symbols.size(); i++) {
        if (AC_symbols[i] == 0xFF) {
            BYTE last_symbol = AC_symbols[AC_symbols.size() - 1];
            AC_symbols[AC_symbols.size() - 1] = AC_symbols[i];
            AC_symbols[i] = last_symbol;
        }
    }
    /* NOTE 2: JPEG standard does not allow symbol code length longer than 16 bits, */
    /* here we adjust the bins to ensure no code length exceeds 16. */
    Array<int> DC_bins, AC_bins;
    for (int i = 0; i < 256; i++) {
        int j = 0;
        DC_bins.append(j);
        AC_bins.append(j);
    }
    for (int i = 0; i < DC_symbols.size(); i++)
        DC_bins[DC_codes[i].size()]++;
    for (int i = 0; i < AC_symbols.size(); i++)
        AC_bins[AC_codes[i].size()]++;
    _jpeg_adjust_bins(&DC_bins);
    _jpeg_adjust_bins(&AC_bins);
    /* delete last symbol (0xFF) */
    Array<BYTE> _symbols;
    for (int i = 0; i < DC_symbols.size() - 1; i++) _symbols.append(DC_symbols[i]);
    DC_symbols = _symbols; _symbols.clear();
    for (int i = 0; i < AC_symbols.size() - 1; i++) _symbols.append(AC_symbols[i]);
    AC_symbols = _symbols; _symbols.clear();
    /* rebuild Huffman tree based on adjusted symbols and bins */
    _jpeg_reassign_huffman_code(&DC_symbols, DC_bins, &DC_codes);
    _jpeg_reassign_huffman_code(&AC_symbols, AC_bins, &AC_codes);
    /* fill in symbols and codes */
    *_DC_symbols = DC_symbols;
    *_AC_symbols = AC_symbols;
    *_DC_codes = DC_codes;
    *_AC_codes = AC_codes;
    *_DC_bins = DC_bins;
    *_AC_bins = AC_bins;

    //if (_DEBUG) {
    //    for (int i = 0; i < DC_codes.size(); i++) {
    //        printf("0x%02X : ", DC_symbols[i]);
    //        DC_codes[i]._print(); printf("\n");
    //    }
    //    printf("\n");
    //    for (int i = 0; i < AC_codes.size(); i++) {
    //        printf("0x%02X : ", AC_symbols[i]);
    //        AC_codes[i]._print(); printf("\n");
    //    }
    //}
}

void _jpeg_generate_huffman_bitstream(FixedArray2D<JPEG_MCU_QCOEFF>* qCoeffs,
    Array<BYTE>* DC_symbols, Array<Bitstream>* DC_codes,
    Array<BYTE>* AC_symbols, Array<Bitstream>* AC_codes,
    int restart_interval, Bitstream* bs) {

    int nW = qCoeffs->sizeX();
    int nH = qCoeffs->sizeY();
    int coeffs[64];

    int restart = 0; /* RST marker ID (0~7) */
    int MCU_remains_before_RST = restart_interval; /* how many MCU remains before next RST* marker */

    Bitstream MCU_bs; /* bitstream generated for MCUs */
    for (int i = 0; i < nW * nH; i++) {
        _jpeg_zz_int8x8_to_intarr(&(qCoeffs->data()[i].Y0), coeffs);
        _jpeg_RLE_as_bitstream(coeffs, DC_symbols, DC_codes, AC_symbols, AC_codes, &MCU_bs);
        _jpeg_zz_int8x8_to_intarr(&(qCoeffs->data()[i].Y1), coeffs);
        _jpeg_RLE_as_bitstream(coeffs, DC_symbols, DC_codes, AC_symbols, AC_codes, &MCU_bs);
        _jpeg_zz_int8x8_to_intarr(&(qCoeffs->data()[i].Y2), coeffs);
        _jpeg_RLE_as_bitstream(coeffs, DC_symbols, DC_codes, AC_symbols, AC_codes, &MCU_bs);
        _jpeg_zz_int8x8_to_intarr(&(qCoeffs->data()[i].Y3), coeffs);
        _jpeg_RLE_as_bitstream(coeffs, DC_symbols, DC_codes, AC_symbols, AC_codes, &MCU_bs);
        _jpeg_zz_int8x8_to_intarr(&(qCoeffs->data()[i].Cb), coeffs);
        _jpeg_RLE_as_bitstream(coeffs, DC_symbols, DC_codes, AC_symbols, AC_codes, &MCU_bs);
        _jpeg_zz_int8x8_to_intarr(&(qCoeffs->data()[i].Cr), coeffs);
        _jpeg_RLE_as_bitstream(coeffs, DC_symbols, DC_codes, AC_symbols, AC_codes, &MCU_bs);
        /* MCU encoding complete */
        MCU_remains_before_RST--;
        if (MCU_remains_before_RST == 0) { /* need to insert RST* marker */
            MCU_remains_before_RST = restart_interval;
            /* pad bit "1" to achieve byte alignment if we are going to insert the RST marker */
            MCU_bs.alignWrite(1);
            /* NOTE 3: be aware that the encoded bitstream can accidentally form a 0xFF byte, */
            /* which is considered as a marker prefix, in this case we need to manually insert */
            /* 0x00 to each 0xFF byte */
            Bitstream MCU_bs_0;
            Array<BYTE> packed_bits = MCU_bs.pack(1);
            for (int i = 0; i < packed_bits.size(); i++) {
                MCU_bs_0.appendBits(packed_bits[i], 8);
                if (packed_bits[i] == 0xFF)
                    MCU_bs_0.appendBits(0x00, 8);
            }
            /* don't need to insert RST marker if this MCU is the last MCU */
            if (i != nW * nH - 1) {
                MCU_bs_0.appendBits(0xFF, 8);
                MCU_bs_0.appendBits(RST0 + restart, 8);
                restart++;
                restart %= 8;
            }
            /* flush bitstream */
            bs->appendBitstream(MCU_bs_0, MCU_bs_0.size());
            MCU_bs.clear();
        }
    }
    if (MCU_bs.size() > 0) {
        /* flush the remaining bits */
        MCU_bs.alignWrite(1);
        Bitstream MCU_bs_0;
        Array<BYTE> packed_bits = MCU_bs.pack(1);
        for (int i = 0; i < packed_bits.size(); i++) {
            MCU_bs_0.appendBits(packed_bits[i], 8);
            if (packed_bits[i] == 0xFF)
                MCU_bs_0.appendBits(0x00, 8);
        }
        bs->appendBitstream(MCU_bs_0, MCU_bs_0.size());
        MCU_bs.clear();
    }

}

/* * * * * * * * * * * * * * * * * * * * * * * * */
/* here are the interface functions for JPEG IO  */
/* * * * * * * * * * * * * * * * * * * * * * * * */

/*
jpeg_read: read and decode a JPEG image file.

* If loading success, "is_valid" will be set to true, and
  "image_data" contains the decoded raw image data (RGB).
* Return NULL pointer if file does not exist or out of memory.
* now the program can only read baseline JPEGs.
* example:

    JPEG_FILE* jfile = jpeg_read("example.jpg");
    if (jfile == NULL){
        printf("error, file not exist or out of memory.\n");
    }
    else if (jfile->is_valid == false) {
        printf("error when loading JPEG file: %s\n", jfile->message);
        ...
    }
    else {
        save_PPM(jfile->image_data, "example.ppm");
        ...
    }
*/
JPEG_API JPEG_FILE* jpeg_read(const char * file)
{
    FILE* fp = NULL;
    if ((fp = fopen(file, "rb")) == NULL) {
        return NULL;
    }
    /* read JPEG file header */
    JPEG_FILE * jfile = new JPEG_FILE();
    if (jfile == NULL) {
        fclose(fp);
        return NULL; /* memory is full */
    }

    if (!_jpeg_read_file(jfile, fp)) {
        goto jpeg_read_error;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * */
    /* decode Huffman bitstream and fill MCU array */
    /* * * * * * * * * * * * * * * * * * * * * * * */

    /* generate code for every Huffman symbol */
    for (int i = 0; i < 4; i++) {
        if (jfile->dctabs[i].is_used) {
            _jpeg_generate_huffman_codes(&(jfile->dctabs[i]));
        }
        if (jfile->actabs[i].is_used) {
            _jpeg_generate_huffman_codes(&(jfile->actabs[i]));
        }
    }

    /* guess chroma subsampling type */
    int hsample = 0, vsample = 0;
    for (int ch = 0; ch < 4; ch++) {
        if (jfile->channels[ch].is_used) {
            if (hsample < jfile->channels[ch].hsample)
                hsample = jfile->channels[ch].hsample;
            if (vsample < jfile->channels[ch].vsample)
                vsample = jfile->channels[ch].vsample;
        }
    }
    if (hsample != 1 && hsample != 2)
        goto jpeg_read_error;
    if (vsample != 1 && vsample != 2)
        goto jpeg_read_error;
    int subsampling_type = 0;
    if (hsample == 1 && vsample == 1)
        subsampling_type = 1; /* no subsampling */
    else if (hsample == 2 && vsample == 1)
        subsampling_type = 2; /* horizontal subsampling */
    else if (hsample == 1 && vsample == 2)
        subsampling_type = 3; /* vertical subsampling */
    else if (hsample == 2 && vsample == 2)
        subsampling_type = 4; /* horizontal and vertical subsampling */

    /* allocate, decode and dequantize all MCUs */
    int nW, nH;
    if (hsample == 1) nW = (jfile->image_width + 7) / 8;
    else nW = (jfile->image_width + 15) / 16;
    if (vsample == 1) nH = (jfile->image_height + 7) / 8;
    else nH = (jfile->image_height + 15) / 16;
    JPEG_MCU* all_MCUs = (JPEG_MCU*)malloc(sizeof(JPEG_MCU) * nW * nH);
    if (all_MCUs == NULL) { /* fatal memory error */
        goto jpeg_read_error;
    }
    if (!_jpeg_decode_MCUs(jfile, nW, nH, all_MCUs, subsampling_type)) {
        free(all_MCUs);
        goto jpeg_read_error;
    }





    if (!_jpeg_dequantize_MCUs(jfile, nW, nH, all_MCUs, subsampling_type)) {
        free(all_MCUs);
        goto jpeg_read_error;
    }
    if (!_jpeg_IDCT_MCUs(nW, nH, all_MCUs, subsampling_type)) {
        free(all_MCUs);
        goto jpeg_read_error;
    }
    if (!_jpeg_decode_color(jfile, nW, nH, all_MCUs, subsampling_type, &(jfile->image_data))) {
        free(all_MCUs);
        goto jpeg_read_error;
    }

    /* clean up */
    free(all_MCUs);

    jfile->is_valid = true;
    fclose(fp);

    _jpeg_dump_message(jfile, "JPEG file successfully read.");
    return jfile;


jpeg_read_error:


    jfile->is_valid = false;
    fclose(fp);
    _jpeg_dump_message(jfile, "error when loading JPEG image file.");
    return jfile;
}
/*
jpeg_free: unload a JPEG file, free all resources allocated.
*/
JPEG_API void jpeg_free(JPEG_FILE * jfile)
{
    free_image(jfile->image_data);
    delete jfile;
}
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
JPEG_API bool jpeg_save(RAW_IMAGE* image, JPEG_SAVE_OPTION* option, const char* file) {

    if (option->save_preset != JPEG_SAVE_PRESET_CUSTOM) {

        /* predefined JPEG quantization tables: */
        /* https://www.impulseadventure.com/photo/jpeg-quantization.html */

        if (option->save_preset == JPEG_SAVE_PRESET_HIGH) {
            option->qtab_Y = fill_int8x8(
                " 1 1 1 1 1 1 1 2 "
                " 1 1 1 1 1 1 1 2 "
                " 1 1 1 1 1 1 2 2 "
                " 1 1 1 1 1 2 2 3 "
                " 1 1 1 1 2 2 3 3 "
                " 1 1 1 2 2 3 3 3 "
                " 1 1 2 2 3 3 3 3 "
                " 2 2 2 3 3 3 3 3 "
            );
            option->qtab_CbCr = fill_int8x8(
                " 1 1 1 2 3 3 3 3 "
                " 1 1 1 2 3 3 3 3 "
                " 1 1 2 3 3 3 3 3 "
                " 2 2 3 3 3 3 3 3 "
                " 3 3 3 3 3 3 3 3 "
                " 3 3 3 3 3 3 3 3 "
                " 3 3 3 3 3 3 3 3 "
                " 3 3 3 3 3 3 3 3 "
            );
        }
        else if (option->save_preset == JPEG_SAVE_PRESET_MEDIUM) {
            option->qtab_Y = fill_int8x8(
                " 8  6  9  14 17 21 28 17 "
                " 6  6  8  13 18 23 12 12 "
                " 9  8  11 17 23 12 12 12 "
                " 14 13 17 23 12 12 12 12 "
                " 17 18 23 12 12 12 12 12 "
                " 21 23 12 12 12 12 12 12 "
                " 28 12 12 12 12 12 12 12 "
                " 17 12 12 12 12 12 12 12 "
            );
            option->qtab_CbCr = fill_int8x8(
                " 9  9  11 18 20 20 17 17 "
                " 9  10 11 14 14 12 12 12 "
                " 11 11 14 14 12 12 12 12 "
                " 18 14 14 12 12 12 12 12 "
                " 20 14 12 12 12 12 12 12 "
                " 20 12 12 12 12 12 12 12 "
                " 17 12 12 12 12 12 12 12 "
                " 17 12 12 12 12 12 12 12 "
            );
        }
        else if (option->save_preset == JPEG_SAVE_PRESET_LOW) {
            option->qtab_Y = fill_int8x8(
                " 32 33 51 81 66 39 34 17 "
                " 33 36 48 47 28 23 12 12 "
                " 51 48 47 28 23 12 12 12 "
                " 81 47 28 23 12 12 12 12 "
                " 66 28 23 12 12 12 12 12 "
                " 39 23 12 12 12 12 12 12 "
                " 34 12 12 12 12 12 12 12 "
                " 17 12 12 12 12 12 12 12 "
            );
            option->qtab_CbCr = fill_int8x8(
                " 34 51 52 34 20 20 17 17 "
                " 51 38 24 14 14 12 12 12 "
                " 52 24 14 14 12 12 12 12 "
                " 34 14 14 12 12 12 12 12 "
                " 20 14 12 12 12 12 12 12 "
                " 20 12 12 12 12 12 12 12 "
                " 17 12 12 12 12 12 12 12 "
                " 17 12 12 12 12 12 12 12 "
            );
        }
        else {
            return false;
        }
    }

    Bitstream jpeg;

    /* write image start marker */
    jpeg.appendBits(0xFF, 8); jpeg.appendBits(SOI, 8);

    /* write quantization tables */
    jpeg.appendBits(0xFF, 8); jpeg.appendBits(DQT, 8);
    jpeg.appendBits(0x0084, 16); /* header size in bytes (132) */
    {
        /* Y, qtab id = 0 */
        jpeg.appendBits(0x00, 8);
        int qtab_coeffs[64];
        _jpeg_zz_int8x8_to_intarr(&option->qtab_Y, qtab_coeffs);
        for (int i = 0; i < 64; i++) {
            jpeg.appendBits(BYTE(qtab_coeffs[i]), 8);
        }
    }
    {
        /* Y, qtab id = 1 */
        jpeg.appendBits(0x01, 8);
        int qtab_coeffs[64];
        _jpeg_zz_int8x8_to_intarr(&option->qtab_CbCr, qtab_coeffs);
        for (int i = 0; i < 64; i++) {
            jpeg.appendBits(BYTE(qtab_coeffs[i]), 8);
        }
    }

    /* define restart interval */
    int restart_interval = 16;
    jpeg.appendBits(0xFF, 8); jpeg.appendBits(DRI, 8);
    jpeg.appendBits(0x0004, 16);
    jpeg.appendBits(0x00, 8); jpeg.appendBits(BYTE(restart_interval), 8);

    /* define start of frame */
    jpeg.appendBits(0xFF, 8); jpeg.appendBits(SOF0, 8);
    jpeg.appendBits(0x00, 8); jpeg.appendBits(17, 8);
    jpeg.appendBits(8, 8);
    jpeg.appendBits(image->h, 16);
    jpeg.appendBits(image->w, 16);
    jpeg.appendBits(3, 8); /* 3 channels */
    for (int i = 1; i <= 3; i++) {
        jpeg.appendBits(i, 8); /* channel ID */
        if (i == 1) {  /* Y */
            jpeg.appendBits(0x22, 8); /* sampling factors (h/v) */
            jpeg.appendBits(0x00, 8); /* quantization table ID */
        }
        else { /* Cb/Cr */
            jpeg.appendBits(0x11, 8); /* sampling factors (h/v) */
            jpeg.appendBits(0x01, 8); /* quantization table ID */
        }
    }

    /* start forward compression, to obtain quantized DCT coefficients */
    FixedArray2D<JPEG_MCU_QCOEFF> qCoeffs;
    _jpeg_forward_compression(image, option, restart_interval, &qCoeffs);

    /* define Huffman tables */
    JPEG_HUFFMAN_TABLE dctab, actab;
    Array<BYTE>      DC_symbols, AC_symbols;
    Array<Bitstream> DC_codes, AC_codes;
    Array<int>       DC_bins, AC_bins;
    _jpeg_generate_huffman_tables(&qCoeffs,
        &DC_symbols, &DC_codes, &DC_bins,
        &AC_symbols, &AC_codes, &AC_bins);
    /* fill the dc and ac table */
    dctab.offsets[0] = 0; actab.offsets[0] = 0;
    for (int i = 1; i < 17; i++)
        dctab.offsets[i] = dctab.offsets[i - 1] + DC_bins[i];
    for (int i = 1; i < 17; i++)
        actab.offsets[i] = actab.offsets[i - 1] + AC_bins[i];
    for (int i = 0; i < DC_symbols.size(); i++)
        dctab.symbols[i] = DC_symbols[i];
    for (int i = 0; i < AC_symbols.size(); i++)
        actab.symbols[i] = AC_symbols[i];
    /* write to bitstream */
    jpeg.appendBits(0xFF, 8); jpeg.appendBits(DHT, 8);
    int tab_len = 2 + (1 + 16 + dctab.offsets[16]) +
        (1 + 16 + actab.offsets[16]); /* 2 tables */
    jpeg.appendBits(WORD(tab_len), 16);
    /* DC huffman table */
    jpeg.appendBits(0, 4); /* DC Huffman table */
    jpeg.appendBits(0, 4); /* table ID = 0 */
    for (int i = 0; i < 16; i++)
        jpeg.appendBits(BYTE(dctab.offsets[i + 1] - dctab.offsets[i]), 8);
    for (int i = 0; i < dctab.offsets[16]; i++)
        jpeg.appendBits(dctab.symbols[i], 8);
    /* AC huffman table */
    jpeg.appendBits(1, 4); /* AC Huffman table */
    jpeg.appendBits(0, 4); /* table ID = 0 */
    for (int i = 0; i < 16; i++)
        jpeg.appendBits(BYTE(actab.offsets[i + 1] - actab.offsets[i]), 8);
    for (int i = 0; i < actab.offsets[16]; i++)
        jpeg.appendBits(actab.symbols[i], 8);

    /* define start of scan */
    jpeg.appendBits(0xFF, 8); jpeg.appendBits(SOS, 8);
    /* generate huffman bitstream */
    int length = 3 + 2 * 3 + 3;
    jpeg.appendBits(length, 16);
    jpeg.appendBits(3, 8);
    for (int i = 1; i <= 3; i++) {
        jpeg.appendBits(i, 8); /* channel ID */
        jpeg.appendBits(0, 4); /* DC huffman table ID */
        jpeg.appendBits(0, 4); /* AC huffman table ID */
    }
    jpeg.appendBits(0, 8);  /* start of selection */
    jpeg.appendBits(63, 8); /* end of selection */
    jpeg.appendBits(0, 8);  /* successive approximation (H/L) */
    /* write huffman bitstream */
    Bitstream bs;
    _jpeg_generate_huffman_bitstream(&qCoeffs,
        &DC_symbols, &DC_codes, &AC_symbols, &AC_codes, restart_interval, &bs);
    jpeg.appendBitstream(bs, bs.size());
    jpeg.alignWrite();

    /* end of image (EOI) marker */
    jpeg.appendBits(0xFF, 8); jpeg.appendBits(EOI, 8);

    FILE* fp = fopen(file, "wb");
    if (fp == NULL)
        return false;

    Array<BYTE> packedBitstream = jpeg.pack();
    if (!_jpeg_write_fp(fp, packedBitstream.size(), packedBitstream.data())) {
        fclose(fp);
        return false;
    }
    else {
        fclose(fp);
        return true;
    }

    return true;
}
