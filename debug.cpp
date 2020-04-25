#include "debug.h"
#include<stdio.h>

FILE *debug_start(void){
    FILE *f = fopen("debug.txt","rb");
    if(f==0)return 0;
    fclose(f);
    f=fopen("debug.txt","wt");
    return f;
}

FILE *debug_file=debug_start();

void debug_print(const char *data){
    if(debug_file){
        fprintf(debug_file,data);
        fflush(debug_file);
    }
    return;
}
void debug_print(const char *data,int data_i){
    if(debug_file){
        fprintf(debug_file,data,data_i);
        fflush(debug_file);
    }
    return;
}
void debug_print(const char *data,char data_c){
    if(debug_file){
        fprintf(debug_file,data,data_c);
        fflush(debug_file);
    }
    return;
}
