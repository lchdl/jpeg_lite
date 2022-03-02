#include "basedefs.h"

bool chInStr(char ch, const char* s) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (ch == s[i]) return true;
    }
    return false;
}
bool getWord(FILE * fp, char * buf, int bufLen, const char* wordDelim, const char commentChar)
{
    if (feof(fp)) return false;
    int i = 0;
    int write = 0, end = 0;
    while (!feof(fp) && !end) {
        int c = fgetc(fp);

        if (chInStr(char(c), wordDelim) || c == -1) {
            if (c == commentChar) { /* skip comment */
                while ((c = fgetc(fp)) != EOF)
                    if (c == '\n')
                        break;
            }
            else if (write == 1) { /* a word is finished because it meets a delimiter */
                write = 0;
                end = 1;
            }
        }
        else { /* word still not complete, write to buffer */
            write = 1;
        }

        if (write && i < bufLen - 1) {
            buf[i] = char(c);
            i++;
        }
    }
    buf[i] = '\0'; /* reserve last character as '\0' */
    if (i == 0) return false; /* if parsed word is empty string return false */
    else return true;
}
bool getWord(char * src, char * buf, int bufLen, const char * wordDelim, const char commentChar)
{
    if (!src) return false;
    int i = 0;
    int s = 0, l = strlen(src);
    int write = 0, end = 0;
    while (s < l && !end) {
        int c = src[s++];

        if (chInStr(char(c), wordDelim)) {
            if (c == commentChar) { /* skip comment */
                while ((c = src[s++]) && (s < l))
                    if (c == '\n')
                        break;
            }
            else if (write == 1) { /* a word is finished because it meets a delimiter */
                write = 0;
                end = 1;
            }
        }
        else { /* word still not complete, write to buffer */
            write = 1;
        }

        if (write && i < bufLen - 1) {
            buf[i] = char(c);
            i++;
        }
    }
    buf[i] = '\0'; /* reserve last character as '\0' */
    /* offset source string by s characters */
    int j = 0;
    for (; s + j < l; j++) {
        src[j] = src[s + j];
    }
    src[j] = '\0';
    /* if parsed word is empty string return false */
    if (i == 0) return false;
    else return true;
}
bool getReal(FILE * fp, REAL * v)
{
    char* errptr = NULL, buf[32];
    if (!getWord(fp, buf, 32)) return false;
    *v = (REAL)(strtod(buf, &errptr));
    if (*errptr != '\0') return false;
    else return true;
}
bool getInt(FILE * fp, int * v)
{
    char* errptr = NULL, buf[32];
    if (!getWord(fp, buf, 32)) return false;
    *v = (int)(strtol(buf, &errptr, 10));
    if (*errptr != '\0') return false;
    else return true;
}
bool getVec3(FILE * fp, VEC3 * v)
{
    if (!getReal(fp, &(v->e[X]))) return false;
    if (!getReal(fp, &(v->e[Y]))) return false;
    if (!getReal(fp, &(v->e[Z]))) return false;
    return true;
}
bool getInt3(FILE * fp, INT3 * v)
{
    if (!getInt(fp, &(v->e[X]))) return false;
    if (!getInt(fp, &(v->e[Y]))) return false;
    if (!getInt(fp, &(v->e[Z]))) return false;
    return true;
}

bool loadTextFile(char * file, char * buf, int bufLen) {
    FILE* fp = NULL;
    fp = fopen(file, "rb");
    if (fp == NULL) {
        return false;
    }
    else {
        /* get file size */
        fseek(fp, 0, SEEK_END);
        int flen = int(ftell(fp));
        fseek(fp, 0, SEEK_SET);
        /* load file into mem */
        int readlen = flen > (bufLen - 1) ? (bufLen - 1) : flen;
        bool success = true;
        if (buf) {
            memset(buf, 0, bufLen);
            int len = int(fread(buf, 1, readlen, fp));
            if (len != readlen) { success = false; }
            else { success = true; }
            /* add '\0' to the end of the string */
            buf[readlen + 1] = '\0';
        }
        else { success = false; }
        fclose(fp);
        return success;
    }
}

void printVec3(const char* prefix, VEC3 v)
{
    if (prefix) {
        printf("%s(%.2f, %.2f, %.2f)\n", prefix ,v.x, v.y, v.z);
    }
    else {
        printf("(%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);
    }
}

void printQuaternion(const char * prefix, QUATERNION q)
{
    if (prefix != NULL) {
        printf("%s", prefix);
    }
    printf("%.2fi + %.2fj + %.2fk + %.2fs\n", q.x, q.y, q.z, q.s);
}

void printMat3x3(const char * prefix, MAT3x3 m)
{
    if (prefix != NULL) {
        printf("%s\n", prefix);
    }
    printf("%.2f %.2f %.2f\n", m.xx, m.xy, m.xz);
    printf("%.2f %.2f %.2f\n", m.yx, m.yy, m.yz);
    printf("%.2f %.2f %.2f\n", m.zx, m.zy, m.zz);
}

