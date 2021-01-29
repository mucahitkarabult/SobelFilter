#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
unsigned char ** matrix_allocate(int rows,int cols);

void deallocetemat(unsigned char**mat,int rows);

void normalization(unsigned char **mat,short **tempM,int pixel,int max,int min,int rows,int cols);

int main(int argc, char *argv[]) {
	char filename[30],comment[100],p[3];
	int rows,cols,pixel,i,j;
	printf("filename: ");
	scanf("%s",filename);
	FILE *fp =fopen(filename,"rb");
	if(fp==NULL){
		printf("File cannot open");
		exit(0);
	}
	
	fgets(p,4,fp);  // skip type of file 
	while (fgetc(fp)=='#'){ // skip comment
		fgets(comment,100,fp);
	}
	fseek(fp,-1,SEEK_CUR); 
	fscanf(fp,"%d %d\n",&cols,&rows);
	fscanf(fp,"%d\n",&pixel);
	


	unsigned char **mat=matrix_allocate(rows,cols),
	**xgrad=matrix_allocate(rows,cols),
	**ygrad=matrix_allocate(rows,cols),
	**sumgrad=matrix_allocate(rows,cols); 
	for(i=0;i<rows;i++){
		fread(mat[i], cols , 1, fp);
	}	
	
	short max=-32767 ,min=32767; // initiliaze with corner value
	
	short **tempM;
	
	tempM=(short**)malloc(sizeof(short*)*rows);
	
	for(i=0;i<rows;i++)
	tempM[i]=(short*)malloc(sizeof(short)*cols);
	
	// sobel for x-gradiyent
	for(i=1;i<rows-1;i++){
		for(j=1;j<cols-1;j++){
			
			tempM[i][j]=(2*mat[i][j+1]+mat[i-1][j+1]+mat[i+1][j+1])-(2*mat[i][j-1] +mat[i-1][j-1]+mat[i+1][j-1]);
			
			
			
			if(tempM[i][j]>max){
				max=tempM[i][j];
			}
			else if(tempM[i][j]<min){
				min=tempM[i][j];
			}
		}
		
	}
	

	//max-min normalization
	normalization(xgrad,tempM,pixel,max,min,rows,cols);
	// take outer pattern from picture to x-y grand 
	for(j=0;j<cols;j++){

		    xgrad[0][j]=mat[0][j];
			ygrad[0][j]=mat[0][j];
			ygrad[rows-1][j]=mat[rows-1][j];					
			xgrad[rows-1][j]=mat[rows-1][j];				
	 					
			}
	for(j=0;j<rows;j++){
			xgrad[j][0]=mat[j][0];
			ygrad[j][0]=mat[j][0];
			xgrad[j][cols-1]=mat[j][cols-1];
			ygrad[j][cols-1]=mat[j][cols-1];
			}
	
	
	FILE *fp1;
	
	fp1=fopen("xgrad.pgm","wb");
	
	fprintf(fp1,"P5\n%d %d\n",cols,rows);
	fprintf(fp1,"%d\n",pixel);
	
	for(i=0;i<rows;i++){
        fwrite(xgrad[i],cols,1,fp1);
   	}
	min=32767;max=-32767;	
	fclose(fp1);
	// y gradyantý için sobel 
		for(i=1;i<rows-1;i++){
			for(j=1;j<cols-1;j++){
				tempM[i][j]=(2*mat[i+1][j]+mat[i+1][j+1]+mat[i+1][j-1])-(2*mat[i-1][j]+mat[i-1][j+1]+mat[i-1][j-1]);
				if(tempM[i][j]>max){
					max=tempM[i][j];
				}
				else if(tempM[i][j]<min){
					min=tempM[i][j];
			}
		}
	}
	//y gradyantý içib normalizasyon
	normalization(ygrad,tempM,pixel,max,min,rows,cols);
	fp1=fopen("ygrad.pgm","wb");
	
	fprintf(fp1,"P5\n%d %d\n",cols,rows);
	fprintf(fp1,"%d\n",pixel);
	

	for(i=0;i<rows;i++){
        fwrite(ygrad[i],1,cols,fp1);
   	}
   	//find sumgrad
   	fclose(fp1);
   	min=32767;max=-32767;
   	for(i=0;i<rows-1;i++){
        for(j=0;j<cols-1;j++){
           tempM[i][j] = sqrt((xgrad[i][j]*xgrad[i][j])+(ygrad[i][j]*ygrad[i][j]));
           	if(tempM[i][j]>max)
			max=tempM[i][j];
			else if(tempM[i][j]<min)
			min=tempM[i][j];
            
        }
 
    }
    
    i=rows-1;
    for(j=0;j<cols;j++)
    tempM[i][j] = sqrt((xgrad[i][j]*xgrad[i][j])+(ygrad[i][j]*ygrad[i][j]));

	normalization(sumgrad,tempM,pixel,max,min,rows,cols);
	
	fp1=fopen("sumgrad.pgm","wb");
	
	fprintf(fp1,"P5\n%d %d\n",cols,rows);
	fprintf(fp1,"%d\n",pixel);
	for(i=0;i<rows;i++){
        fwrite(sumgrad[i],1,cols,fp1);
   	}
	close(fp1);
	
	deallocetemat(sumgrad,rows);
	deallocetemat(xgrad,rows);
	deallocetemat(ygrad,rows);
	deallocetemat(mat,rows);
    deallocetematS(tempM,rows);

    

	 	
	
			
	return 0;
	
}

unsigned char ** matrix_allocate(int rows,int cols){
	int i;
	unsigned char ** mat=(unsigned char **)malloc(sizeof(unsigned char * )*rows);
	if(mat==NULL){
        printf("Malloc Error");
        return;
    }
	
	for(i=0;i<rows;i++){
		mat[i]=(unsigned char*)malloc(sizeof(unsigned char)*cols);
		if(mat[i]==NULL){
        printf("Malloc Error");
        return;
    }
	}
	return mat;
}

void normalization(unsigned char**mat,short **tempM,int pixel,int max,int min,int rows,int cols){
	int  i,m=max-min,j;
	   for(i=0;i<rows;i++){

		for(j=0;j<cols;j++){
					
			mat[i][j]=(unsigned char)(((tempM[i][j]-min)*pixel)/m);
				
					
	    }
	}
	
}

void deallocetemat(unsigned char**mat,int rows){
	int i;
    for(i=0;i<rows;i++){
        free(mat[i]);
    }
    free(mat);
}
void deallocetematS(short **mat,int rows){
	int i;
    for(i=0;i<rows;i++){
        free(mat[i]);
    }
    free(mat);
}








