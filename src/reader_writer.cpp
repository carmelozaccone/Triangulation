#include <stdlib.h>
#include <stdio.h>

#include <iostream>

/********************************************************
 *  SOME MACRO FOR ERROR MESSAGES
 *  (maybe put that into some header, it's very usefull)
 ********************************************************/
#define MSH_ERROR(msg, ...) do{ \
    fprintf(stderr,"\e[1;31m\n== ERROR (%s -> %s:%d):\n\e[0m\t" msg \
        "\nExiting Program...\n\n",__func__,__FILE__,__LINE__,##__VA_ARGS__); \
    exit(EXIT_FAILURE); \
    } while(0)

#define MSH_WARN(msg,...) \
    fprintf(stderr,"\nwarning (%s -> %s:%d): " msg "\n",__func__,__FILE__,__LINE__,##__VA_ARGS__);

#ifdef DEBUG
#define MSH_ASSERT(exp,...) do{ \
        if(!(exp)) { \
            fprintf(stderr,"\e[1;31m\n== ERROR (%s -> %s:%d):\t assertion (" #exp ") Failed\n\e[0m\t" __VA_ARGS__ \
            "\nExiting Program...\n\n",__func__,__FILE__,__LINE__); \
            exit(EXIT_FAILURE); \
        }\
    } while(0)

#else
#define MSH_ASSERT(...)
#endif


/* gives an approximation for the number of lines in the file */
static unsigned lines_estim(FILE* file){
	/* read the first line */
	char str[2048];
	if(fgets(str,2048,file)==NULL || str[0]=='\0'){
		MSH_WARN("Empty file");
		return 0;
	}

	/* count the number of charachter at the beginning of the file*/
	int c;
	for (c=0; str[c+1]!='\0'; c++);

	fseek(file,0L,SEEK_END);
	unsigned lines_approx = ftell(file)/c;
	fseek(file,0L,SEEK_SET);
	return lines_approx;
}



unsigned read_nodes_txt(const char* filename, double** Data)
{
MSH_ASSERT(filename!=NULL)

	FILE* file = fopen(filename,"r");
	if(file==NULL)
		MSH_ERROR("Cannot open file %s",filename);

	unsigned nlines = lines_estim(file);
	if(nlines==0){
		Data=NULL;
		return 0;
	}
	unsigned length = 0;

	// we allocate much more to be sure (realloc a lesser size is fast)
	*Data = (double*) malloc(5*nlines*sizeof(double)); 
	
	unsigned line=0;


	while(!feof(file)){
		if(3*length>5*nlines) // should not happen if the first line is "normal"
			*Data = (double*) realloc(*Data, 5*length*sizeof(double));

		int i;

		for (i=0; i<3; i++){

			if(fscanf(file," %256lf ",*Data+length*3+i)!=1){
				if(i==0){ // the whole line is missing, we don't care
					break;
				}
				else{
					MSH_ERROR("File issue: missing column (line%d, col%d)"
					"(can be caused by an unwanted character in the file)",line,i);
				}
			}
		}

		if(i!=0)
			length++;
		line++;
	}
	fclose(file);
	*Data = (double*) realloc(*Data, 3*length*sizeof(double));
	return length;
}



void write_gmsh_txt(const char* filename, double* Data, unsigned dLength, unsigned* Triangles, unsigned tLength)
{
MSH_ASSERT(filename!=NULL);
MSH_ASSERT(Data!=NULL);
MSH_ASSERT(Triangles!=NULL);

	//FILE* file = fopen(filename,"C:/Users/Naomiel/Documents/UCL/GBIO2M/Option principale/Numerical Geometry/Projet/Projet/Projet/Out/triangle.out");
	FILE* file = fopen(filename,"w");
        if(file==NULL)
		MSH_ERROR("Cannot open file %s",filename);

	/* format for gmsh */
	fprintf(file,"$MeshFormat\n"
	             "2.2 0 %u\n"
	             "$EndMeshFormat\n"
	             "$Nodes\n"
	             "%u\n",(unsigned) sizeof(double),dLength);
	/* print the nodes */
	unsigned i;
	for (i=0; i<dLength; i++)
		fprintf(file,"%u %.10E %.10E 0.0\n",i+1,Data[3*i],Data[3*i+1]);

	/* print the elements */
	fprintf(file,"$EndNodes\n"
                 "$Elements\n"
                 "%u\n",tLength);
	for (i=0; i<tLength; i++)
		fprintf(file,"%u 2 0 %u %u %u\n",i+1,Triangles[3*i]+1,
			                               Triangles[3*i+1]+1,
			                               Triangles[3*i+2]+1);
	fputs("$EndElements\n",file);
	fprintf(file,"$NodeData\n"
		         "1\n\"Delaunay_triangulation\"\n"
		         "1\n0.0\n3\n0\n1\n%u\n",dLength);

	for (i=0; i<dLength; i++)
		fprintf(file,"%u %.10E\n",i+1, Data[3*i+2]);
	fputs("$EndNodeData",file);
	fclose(file);
}



void write_gmsh_bin(const char* filename, double* Data, unsigned dLength, unsigned* Triangles, unsigned tLength)
{
MSH_ASSERT(filename!=NULL);
MSH_ASSERT(Data!=NULL);
MSH_ASSERT(Triangles!=NULL);

	FILE* file = fopen(filename,"wb");
	if(file==NULL)
		MSH_ERROR("Cannot open file %s",filename);

	/* format for gmsh */
	fprintf(file,"$MeshFormat\n"
	             "2.2 1 %u\n",(unsigned) sizeof(double));
	int one = 1;
	fwrite(&one, sizeof(int), 1, file);

	fprintf(file,"\n$EndMeshFormat\n$Nodes\n%u\n",dLength);

	double zero = 0.0;
	/* print the nodes */
	unsigned i;
	for (i=1; i<=dLength; i++)
	{
		fwrite(&i, sizeof(unsigned), 1, file);
		fwrite(Data+3*i-3, sizeof(double), 2, file);
		fwrite(&zero, sizeof(double), 1, file);
	}

	/* print the elements */
	fprintf(file,"\n$EndNodes\n"
                 "$Elements\n"
                 "%u\n",tLength);

	unsigned header[3] = {2, tLength, 0};
	fwrite(header, sizeof(unsigned), 3, file);

	for (i=0; i<tLength; i++)
	{
		unsigned data[4] = {i+1,Triangles[3*i]+1,Triangles[3*i+1]+1,Triangles[3*i+2]+1};
  		fwrite(data, sizeof(unsigned), 4, file);
	}
	fputs("$EndElements\n",file);

	fprintf(file,"$NodeData\n"
	             "1\n\"Delaunay_triangulation\"\n"
	             "1\n0.0\n3\n0\n1\n%u\n",dLength);

	for (i=1; i<=dLength; i++)
	{
		fwrite(&i, sizeof(unsigned), 1, file);
		fwrite(Data+3*i-1,sizeof(double),1,file);
	}
	fputs("\n$EndNodeData",file);

	fclose(file);
}

