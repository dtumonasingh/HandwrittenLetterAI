#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define INPUT 784
#define HIDDEN 16
#define OUTPUT 10

void swap(char **str1, char **str2)
{
  char * temp = *str1;
  *str1 = *str2;
  *str2 = temp;
}

void shuffle(char*files[],int n){
  int i;
    srand ( time(NULL) );
    for ( i = n-1; i > 0; i--){
        int j = rand() % (i+1);
        swap(&files[i], &files[j]);
    }
}

double sigmoid(double x)
{
    return (1.0/(1.0+exp(-x)));
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

void hiddenOutput(double hidden_layer[],double hidden_output[][OUTPUT],double output_layer[OUTPUT]){
                  double sum=0;
                  for(int a=0;a<OUTPUT;a++){
                    sum=0;
                    for(int b=0;b<HIDDEN;b++){
                    sum+=hidden_layer[b]*hidden_output[b][a];
                    }
                    output_layer[a]=sigmoid(sum);
                  }
                }
int isFile(const char *path)
{
       struct stat path_stat;
       stat(path, &path_stat);
       return S_ISREG(path_stat.st_mode);
}

int main () {
      double input_layer[INPUT];
      double hidden_layer[HIDDEN];
      double output_layer[OUTPUT];
      double input_hidden[INPUT][HIDDEN];
      double hidden_output[HIDDEN][OUTPUT];
      double deltaw[INPUT][HIDDEN];
      double deltav[HIDDEN][OUTPUT];
      double hn_delta[HIDDEN];
      double y_delta[OUTPUT];
      int count=0,k=0,error,errortemp;
      float value;
      double output_layer_des[OUTPUT]={0};
      double alpha=0.1, beta=0.1;

      for(int m=0;m<INPUT;m++){       //assigning random weights
        for(int n=0;n<HIDDEN;n++){
          input_hidden[m][n]=(float)rand()/RAND_MAX;
        }
      }

      for(int m=0;m<HIDDEN;m++){    //assigning random weights
        for(int n=0;n<OUTPUT;n++){
          hidden_output[m][n]=(float)rand()/RAND_MAX;
        }
      }
      //directory
        int len;
        struct dirent *pDirent;
        DIR *pDir;
        char *v="trainingMatrix";
        pDir = opendir (v);
        if (pDir == NULL) {
            printf ("Cannot open directory '%s'\n", v);
            return 1;
        }
          //counting the files in directory
        char *root="trainingMatrix/";
        int filecount=0;
        while ((pDirent = readdir(pDir)) != NULL) {
          char * f = pDirent->d_name;
          char *filename=malloc(strlen(root)+strlen(f)+1);
          strcpy(filename,root);
          strcat(filename,f);
          if(isFile(filename)&&pDirent->d_name[0]!='.'){
            filecount++;
          }
        }


        //saving file names
        int ch=0;
        char *files[filecount];
        pDir = opendir (v);
        while ((pDirent = readdir(pDir)) != NULL) {
        //  printf("%d\n",ch);
          char * f = pDirent->d_name;
          char *filename=malloc(strlen(root)+strlen(f)+1);
          strcpy(filename,root);
          strcat(filename,f);
          if(isFile(filename)&&pDirent->d_name[0]!='.'){
            files[ch++]=filename;
          }
      }
        closedir (pDir);
        shuffle(files,filecount);
        for(int loop=0;loop<filecount;loop++){
          k=0;
          printf("%d-",loop+1);
          printf("%s\n",files[loop]);
          FILE* file=fopen (files[loop], "r");
          fscanf (file, "%f", &value);
          while(!feof(file))  //Taking input form the Input file.
              {
                input_layer[k++]=value;
                fscanf (file, "%f", &value);
              }

              //printf("%d\n", count);
              for(int i=0;i<INPUT;i++)
              {
                input_layer[i]=(input_layer[i])/255;
              }


              inputHidden(input_layer,input_hidden,hidden_layer);
              hiddenOutput(hidden_layer,hidden_output,output_layer);

              // for(int i=0;i<OUTPUT;i++)
              // printf("%lf\t",output_layer[i]);
              // printf("\n\n");
              for(int i=0;i<OUTPUT;i++)
                  output_layer_des[i]=0;

              int des_output=files[loop][15]-'0';
              output_layer_des[des_output]=1;

              //Backpropagations
              for(int i=0; i<OUTPUT; i++){
                    errortemp=output_layer_des[i]-output_layer[i];
                        y_delta[i]=-errortemp*sigmoid(output_layer[i])*(1.0-sigmoid(output_layer[i]));
                         }

               for(int i=0; i<HIDDEN; i++){
                errortemp = 0.0;
                    for(int j=0; j<OUTPUT; j++)
                         errortemp +=y_delta[j]*hidden_output[i][j];
                     hn_delta[i] = errortemp*sigmoid(hidden_layer[i])*(1.0-sigmoid(hidden_layer[i]));
                }
                // Stochastic gradient descent
                for(int i=0; i<OUTPUT; i++){
                  for(int j=0; j<HIDDEN; j++){
                    deltav[j][i]=alpha*deltav[j][i]+beta*y_delta[i]*hidden_layer[j];
                    hidden_output[j][i]-=deltav[j][i];
                  }
                }
                for(int i=0; i<HIDDEN; i++){
                  for(int j=0; j<INPUT; j++){
                      deltaw[j][i]=alpha*deltaw[j][i]+beta*hn_delta[i]*input_layer[j];
                      input_hidden[j][i]-= deltaw[j][i];
                    }
                  }
                  
               fclose(file);

          count++;
                }


              FILE* file = fopen ("hidden_output.txt", "r");
              for(int i=0; i<HIDDEN; i++){
                for(int j=0; j<OUTPUT; j++){
                 printf("%f ",hidden_output[i][j]);
                  fprintf(file,"%f\t",hidden_output[i][j]);
                }

                fprintf(file,"\n");
              }
              //printf("\n\n");
              fclose(file);

              file=fopen ("input_hidden.txt", "r");
              for(int i=0; i<INPUT; i++){
                for(int j=0; j<HIDDEN; j++){
                printf("%f ",input_hidden[i][j]);
                  fprintf(file,"%f",input_hidden[i][j]);
                }
              //  printf("\n");
                fprintf(file,"\n");
              }
              fclose(file);
              printf("total%d\n",count);

              return 0;
            }