#include<stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>

#define INPUT 784
#define HIDDEN 16
#define OUTPUT 10

int isFile(const char *path)
{
       struct stat path_stat;
       stat(path, &path_stat);
       return S_ISREG(path_stat.st_mode);
}

double sigmoid(double x){
  return(1.0 / (1.0 + exp(-x)));
}

void inputHidden(double input_layer[],double input_hidden[][HIDDEN],double hidden_layer[HIDDEN]){
double sum=0;

                for(int a=0;a<HIDDEN;a++){
                  sum=0;
                  for(int b=0;b<INPUT;b++){
                  sum+=input_layer[b]*input_hidden[b][a];
                  }
                  hidden_layer[a]=sigmoid(sum);
                }

}

void hiddenOutput(double hidden_layer[],double hidden_output[][OUTPUT],double output_layer[HIDDEN]){
double sum=0;
                  for(int a=0;a<OUTPUT;a++){
                    sum=0;
                    for(int b=0;b<HIDDEN;b++){
                    sum+=hidden_layer[b]*hidden_output[b][a];
                    }
                    output_layer[a]=sigmoid(sum);
                  }
}

int main () {
      double input_layer[INPUT];
      double hidden_layer[HIDDEN];
      double output_layer[OUTPUT];
      double input_hidden[INPUT][HIDDEN];
      double hidden_output[HIDDEN][OUTPUT];
      int count=0,k=0,d;
      double value;
      double output_layer_des[OUTPUT]={0};
      FILE* file = fopen ("input_hidden.txt", "r");
      float ip;
      fscanf(file,"%f",&ip);
      int row=0,col=0;
      while(!feof(file)){
        if(col==HIDDEN){
          row++;
          col=0;
        }
        input_hidden[row][col]=ip;
        fscanf(file,"%f",&ip);
        col++;
      }
      fclose(file);

          file = fopen ("hidden_output.txt", "r");
          fscanf(file,"%f",&ip);
          row=0,col=0;
          while(!feof(file)){
            if(col==OUTPUT){
              row++;
              col=0;
            }
            hidden_output[row][col]=ip;
            fscanf(file,"%f",&ip);
            col++;
          }
          fclose(file);

        int len;
        struct dirent *pDirent;
        DIR *pDir;

        char *v="testingMatrix";
        pDir = opendir(v);
        if (pDir == NULL) {
            printf ("Cannot open directory '%s'\n", v);
            return 1;
        }
        int hits=0, misses=0;
        char *root="testingMatrix/";

        while ((pDirent = readdir(pDir)) != NULL) {
          k=0;
          char * f = pDirent->d_name;
          char *filename=malloc(strlen(root)+strlen(f)+1);
          strcpy(filename,root);
          strcat(filename,f);
          if(isFile(filename)&&pDirent->d_name[0]!='.'){
          FILE* file = fopen (filename, "r");
          fscanf (file, "%lf", &value);
          while(!feof(file))  //Taking input form the Input file.
              {
                input_layer[k++]=value;
                fscanf (file, "%lf", &value);
              }
              for(int i=0;i<INPUT;i++)
              {
                input_layer[i]=(input_layer[i])/255;
              }
              count++;
              inputHidden(input_layer,input_hidden,hidden_layer);
              hiddenOutput(hidden_layer,hidden_output,output_layer);
              int des_output=pDirent->d_name[0]-'0';
              for(int i=0;i<OUTPUT;i++)
              output_layer_des[i]=0;
              output_layer_des[des_output]=1;
              // int max=output_layer[0];
             printf("%d\n",des_output);
             int max=0;
              for(int i=0;i<OUTPUT;i++){
                 printf("%f\t",output_layer[i]);
                 if(output_layer[i]>output_layer[max])
                 {
                  max=i;
                 }
               }

              if(max==des_output)
                hits++;
              else
                misses++;

              printf("\n\n\n");
              fclose(file);
              free(filename);
                }
              }

              printf("total%d\n",count);
              printf("Hits: %d\nMisses: %d\nAccuracy: %f\n",hits,misses,((float)hits/(float)(hits+misses))*100);

              closedir (pDir);
              return 0;
            }
