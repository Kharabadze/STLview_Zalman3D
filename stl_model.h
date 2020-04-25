#pragma once
struct one_part{
    float norm[3];
    float vert[4][3];
    unsigned short attrib;
    //bool test(void);
};

struct tr_matr_t{
    double data[3][3];
    double edin[3][3];
    bool rotate_xy(float x,float y);
    bool scale(int n);
    tr_matr_t(void);
};
struct stl_model_t{
    unsigned char name[128];
    int number;
    one_part *triangles;

    //--- text Decoder
    unsigned char *text_buffer;
    int sizeof_text;
    int cur_p;
    bool have(const char *oper,float *dat,bool *eof,bool extreme_find=false);
    //--- ext
    bool open(char *filename);
    bool correct(void);
    bool new_empty(int number_of_vert);
    bool convert_from(stl_model_t *initial,tr_matr_t *conv);

    bool freemem(void);
};
