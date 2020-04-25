#include "stl_model.h"

#include<stdio.h>
#include<math.h>

#include"debug.h"

double det(double *a,double *b,double *c){
    double dd = 0.0;

    dd += a[0]*b[1]*c[2];
    dd += a[1]*b[2]*c[0];
    dd += a[2]*b[0]*c[1];

    dd -= a[2]*b[1]*c[0];
    dd -= a[1]*b[0]*c[2];
    dd -= a[0]*b[2]*c[1];

    return dd;
}
/*
bool one_part::test(void){
    double norm_len = 0.0;
    for(int i=0;i<3;i++)
        norm_len+=norm[i]*norm[i];
    norm_len=sqrt(norm_len);
    if((norm_len<0.999)||(norm_len>1.001))
        return false;

    double a[3],b[3],c[3];
    for(int i=0;i<3;i++){
        double mid = (vert1[i]+vert2[i]+vert3[i])/3.0;
        a[i] = vert1[i] - mid - norm[i];
        b[i] = vert2[i] - mid - norm[i];
        c[i] = vert3[i] - mid - norm[i];
    }

    double rez = det(a,b,c);

    return (rez<=0);
}
*/

bool tr_matr_t::rotate_xy(float x,float y){
    double phi = 0.005*sqrt(x*x+y*y);
    double theta = atan2(y,x);
    double c_phi = cos(phi),s_phi = sin(phi);
    double c_theta = cos(theta),s_theta = sin(theta);
    double dmat [3][3]={
        {   1+(c_phi-1)*c_theta*c_theta ,   (1-c_phi)*s_theta*c_theta ,  s_phi*c_theta   },
        {     (1-c_phi)*s_theta*c_theta , 1+(c_phi-1)*s_theta*s_theta , -s_phi*s_theta   },
        {        -s_phi*c_theta         ,       s_phi*s_theta         ,  c_phi           }
    };

    //multiply
    double prom[3][3];
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            prom[i][j]=0.0;
            for(int k=0;k<3;k++){
                prom[i][j]+=dmat[i][k]*data[k][j];
            }
        }
    }

    //copy
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            data[i][j]=prom[i][j];
        }
    }

    //multiply
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            prom[i][j]=0.0;
            for(int k=0;k<3;k++){
                prom[i][j]+=dmat[i][k]*edin[k][j];
            }
        }
    }

    //copy
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            edin[i][j]=prom[i][j];
        }
    }

    return true;
}
bool tr_matr_t::scale(int n){
    double sc_d = pow(2,n/12.0);
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            data[i][j]*=sc_d;
        }
    }
    return true;
}

tr_matr_t::tr_matr_t(void){
    data[0][0]= 1.0;data[0][1]=  0.0;data[0][2]= 0.0;
    data[1][0]= 0.0;data[1][1]=  0.0;data[1][2]= 1.0;
    data[2][0]= 0.0;data[2][1]= -1.0;data[2][2]= 0.0;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            edin[i][j]=data[i][j];
    return;
}
//------------------------------------
bool stl_model_t::have(const char *oper,float *dat,bool *eof,bool extreme_find){
    int backup_point = cur_p;
    unsigned char c=0;
    //debug_print(oper);
    //debug_print("\n");

    int sost = 0;//0 - before num,1 - before point,2-after point;

    if(oper[0]=='#'){//number
        *dat = 0.0;
        float drob=1.0;
        bool negative = false;
        int pok = 0;

        while(1){
            if(cur_p>=sizeof_text){*eof=true;return false;}
            c=text_buffer[cur_p++];
            //debug_print("%c",c);
            if(c<=' '){
                if(sost==0)continue;//Skip free symbols
                else{
                    if(negative)*dat = -*dat;
                    if(sost==3)(*dat) *= pow(10,pok);
                    if(sost==4)(*dat) *= pow(10,-pok);
                    return true;
                }
            }
            if(c=='.'){
                if(sost>=2)return false;//Error in facet
                else {sost=2;drob=1.0;continue;}
            }
            if((c=='e')||(c=='E')){
                if(sost<=2){ sost=3;continue;}
                else return false;
            }
            if(c=='-'){
                if(sost==0){
                    negative = true;
                    sost = 1;
                    continue;
                }else if(sost==3){
                    sost = 4;
                    continue;
                }else{
                    return false;//Error in facet
                }
            }
            if((c>='0')&&(c<='9')){
                if(sost==0)sost=1;
                if(sost==1)((*dat)*=10.0)+=(c-'0');
                if(sost==2)(*dat)+=(drob*=0.1)*(c-'0');
                if((sost==3)||(sost==4)){
                    (pok*=10)+=(c-'0');
                }
                continue;
            }
            //--- any other case
            cur_p=backup_point;//fseek(f,backup_point,SEEK_SET);
            debug_print("BAD number %c\n",c);
            return false;
        }
    }else{
        int uk=0;
        while(1){
            if(cur_p>=sizeof_text){*eof=true;return false;}
            c=text_buffer[cur_p++];
            //debug_print("%c",c);
            if(c<=' '){
                if(sost==0)continue;//Skip free symbols
            }
            if((c==oper[uk])||(c==oper[uk]-32)){
                //debug_print("+");
                sost=1;
                uk++;
                if(oper[uk]==0)return true;
                else continue;
            }
            //--- any other case
            //debug_print(oper);
            //debug_print(" -- BAD symbol %c\n",c);
            if(extreme_find){//facet
                if(c==oper[uk])cur_p-=1;//fseek(f,-1,SEEK_CUR);//use last symb
                uk=0;
                continue;
            }else{
                cur_p=backup_point;//fseek(f,backup_point,SEEK_SET);
                return false;
            }
        }
    }
    return true;
}

bool stl_model_t::open(char *filename){
    //debug_print(filename);
    debug_print("-opening file...\n");
    FILE *f=fopen(filename,"rb");
    if(f==0){
        debug_print("Can't open the file");
        return false;
    }
    fseek(f,0,SEEK_END);
    int len = ftell(f);
    //printf("length_of_file = %i\n",len);

    fseek(f,0,SEEK_SET);
    if(len<80){
        fclose(f);
        return false;
    }
    fread(name,1,80,f);name[80]=0;len-=80;

    {//Test for text file format
        char var1[8]="solid";
        bool good = true;
        for(int i=0;i<5;i++)if((name[i]!=var1[i])&&(name[i]!=var1[i]-32))good=false;
        if(good){
            len+=80;//Correct it
            debug_print("Text header was detected\n");
            fseek(f,0,SEEK_SET);
            sizeof_text = len;
            text_buffer = new unsigned char[sizeof_text];
            if(fread(text_buffer,1,sizeof_text,f)!=sizeof_text){
                debug_print("Can't read the file\n");
                delete[]text_buffer;
                fclose(f);
                return false;
            }
            debug_print("File loaded to memory - %i bytes\n",sizeof_text);
            fclose(f);

            //--- Start procesing
            for(int step=0;step<2;step++){
                debug_print("Step %i of algorithm\n",step);
                cur_p = 0; //fseek(f,0,SEEK_SET);
                bool eof=false;
                float dat,facet[4][3];
                number = 0;
                while(1){
                    //debug_print("<");
                    if(eof)break;
                    if(have("endsolid",&dat,&eof))break;//
                    if(eof)break;
                    if(!have("facet",&dat,&eof,true))continue;
                    if(!have("normal",&dat,&eof))continue;
                    if(!have("#",&facet[3][0],&eof))continue;
                    if(!have("#",&facet[3][1],&eof))continue;
                    if(!have("#",&facet[3][2],&eof))continue;
                    if(!have("outer",&dat,&eof))continue;
                    if(!have("loop",&dat,&eof))continue;
                    if(!have("vertex",&dat,&eof))continue;
                    if(!have("#",&facet[0][0],&eof))continue;
                    if(!have("#",&facet[0][1],&eof))continue;
                    if(!have("#",&facet[0][2],&eof))continue;
                    if(!have("vertex",&dat,&eof))continue;
                    if(!have("#",&facet[1][0],&eof))continue;
                    if(!have("#",&facet[1][1],&eof))continue;
                    if(!have("#",&facet[1][2],&eof))continue;
                    if(!have("vertex",&dat,&eof))continue;
                    if(!have("#",&facet[2][0],&eof))continue;
                    if(!have("#",&facet[2][1],&eof))continue;
                    if(!have("#",&facet[2][2],&eof))continue;
                    if(!have("endloop",&dat,&eof))continue;
                    if(!have("endfacet",&dat,&eof))continue;
                    //debug_print("%i>",number);
                    //--- Processing...
                    if(step==1){
                        for(int qq=0;qq<3;qq++){
                            for(int qw=0;qw<3;qw++){
                                triangles[number].vert[qq][qw]=facet[qq][qw];
                            }
                        }
                        for(int qw=0;qw<3;qw++){
                            triangles[number].norm[qw]=facet[3][qw];
                        }
                        triangles[number].attrib = 0;
                    }
                    number++;
                }
                debug_print("Size = %i\n",number);
                if(number==0){
                    debug_print("No facets in file\n");
                    delete[]text_buffer;
                    return false;//bad file
                }else if(step==0){
                    triangles = new one_part[number];
                }
            }
            //---
            //debug_print("Tot Size = %i\n",number);
            debug_print("Text STL was finished\n");
            delete[]text_buffer;
            return true;
        }
    }


    //get number of triangles
    fread(&number,1,4,f);len-=4;
    debug_print("Number of triangles = %i\n",number);

    //test
    if((number*50)!=len){
        debug_print("Corrupted file (%i*50 !=",number);
        debug_print(" %i*50)\n",len);
        fclose(f);
        return false;
    }

    //get data from file
    triangles = new one_part[number];
    for(int i=0;i<number;i++){
        fread(triangles[i].norm,1,3*4,f);len-=12;
        fread(triangles[i].vert,1,3*3*4,f);len-=36;
        fread(&triangles[i].attrib,1,2,f);len-=2;
    }

    if(len!=0){
        debug_print("Corrupted file (%i bytes left)\n",len);
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}
bool stl_model_t::correct(void){
    double mins[3],maxs[3];
    for(int i=0;i<3;i++) mins[i] = maxs[i] = triangles[0].vert[0][i];

    for(int vn=0;vn<number;vn++){
        for(int pn=0;pn<3;pn++){
            for(int i=0;i<3;i++){
                if(mins[i]>triangles[vn].vert[pn][i]) mins[i] = triangles[vn].vert[pn][i];
                if(maxs[i]<triangles[vn].vert[pn][i]) maxs[i] = triangles[vn].vert[pn][i];
            }
        }
    }

    double max_fr = 0.0;
    for(int i=0;i<3;i++)if(max_fr<(maxs[i]-mins[i]))max_fr=(maxs[i]-mins[i]);

    for(int vn=0;vn<number;vn++){
        for(int pn=0;pn<3;pn++){
            for(int i=0;i<3;i++){
                triangles[vn].vert[pn][i]-=(maxs[i]+mins[i])*0.5;
                triangles[vn].vert[pn][i]*=500/max_fr;
            }
        }
    }

    //нормаль
    for(int vn=0;vn<number;vn++){
        double sve[3]={0.0,0.0,0.0};
        for(int i=0;i<3;i++){
            int x=i,y=(i+1)%3,z=(i+2)%3;
            sve[x]=0;
            for(int j=0;j<3;j++)
                sve[x]+=
                    triangles[vn].vert[j][y]*triangles[vn].vert[(j+1)%3][z]-
                    triangles[vn].vert[j][z]*triangles[vn].vert[(j+1)%3][y];
        }
        //=== normilize
        double sle =0;
        for(int i=0;i<3;i++)sle+=sve[i]*sve[i];
        sle = sqrt(sle);
        for(int i=0;i<3;i++)triangles[vn].vert[3][i]=sve[i]/sle;
    }

    return true;
}
bool stl_model_t::new_empty(int number_of_vert){
    number = number_of_vert;
    triangles = new one_part[number];
    return true;
}
bool stl_model_t::convert_from(stl_model_t *initial,tr_matr_t *conv){
    if(number!=initial->number)return false;
    for(int vn=0;vn<number;vn++){
        for(int pn=0;pn<3;pn++){
            for(int i=0;i<3;i++){
                triangles[vn].vert[pn][i]=0.0;
                for(int j=0;j<3;j++){
                    triangles[vn].vert[pn][i]+=
                        (float)conv->data[i][j]*
                        initial->triangles[vn].vert[pn][j];
                }
            }
        }
        //--- Нормаль
        for(int i=0;i<3;i++){
            triangles[vn].vert[3][i]=0.0;
            for(int j=0;j<3;j++){
                triangles[vn].vert[3][i]+=
                    (float)conv->edin[i][j]*
                    initial->triangles[vn].vert[3][j];
            }
        }
    }
    return true;
}
bool stl_model_t::freemem(void){
    if(number!=0){
        if(triangles)
            delete[]triangles;
        triangles=0;
        number=0;
    }
    return true;
}
