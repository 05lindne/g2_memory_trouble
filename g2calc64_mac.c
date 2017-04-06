/*
  Name: g2calc - a program to calculate the cross-correlation of two time ordered lists
  Copyright: 
  Author: Carlos Russo
  Date: 26-02-06 11:29
  Description: 
  Notes: compile using "-std=c99" (because of the restrict keyword)
*/

#define _FILE_OFFSET_BITS 64
#define __USE_LARGEFILE64
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE

#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>			/* declare the 'stat' structure	*/
#include <sys/types.h>	
// #include <conio.h> /* for getch(), non-portable */
#include <fcntl.h>

//-------------------------------------------------------------------------
//     Start of portability issues
//-------------------------------------------------------------------------

// I need a 64-bit integer
#ifdef _MSC_VER
   typedef __int64 int64_t	// Define it from MSVC's internal type
#else
   #include <stdint.h>		// Use the C99 official header
#endif

// I need aligned memory to be able to use SSE instructions
#include <malloc/malloc.h>
// _aligned_malloc and _aligned_free are not portable to Linux, we need to wrap them! 
inline void * _aligned_malloc( size_t size, size_t boundary) 
{ 
      return ((void *) memalign(boundary, size));
}
#define _aligned_free   free



//-------------------------------------------------------------------------
//     End of portability issues
//-------------------------------------------------------------------------


#define FLOAT2UINTCAST(f) (*((int *)(&f)))
//According to AMD, if f is no NaN
//(f<0.0f) is equivalent to (FLOAT2UINTCAST(f)> FLOAT2UINTCAST(bin_f) > 0x80000000U)


//-------------------------------------------------------------------------
//     Specific settings
//-------------------------------------------------------------------------
#define G2DEBUG (1==1)
#define MEM_ALIGNMENT 16

//-------------------------------------------------------------------------
//     End of specific settings
//-------------------------------------------------------------------------




int Correlate_float(unsigned int * restrict g2, const  int64_t * restrict cl1, const  int64_t * restrict cl2, const unsigned int sizeg2, const int64_t n1, const int64_t n2, const float scale);
int Correlate_float_fast(unsigned int * restrict g2, const  int64_t * restrict cl1, const  int64_t * restrict cl2, const unsigned int sizeg2, const unsigned int n1, const unsigned int n2, const float scale);
int Correlate_float_4fast(unsigned int * restrict g2, const  int64_t * restrict cl1, const  int64_t * restrict cl2, const unsigned int sizeg2, const unsigned int n1, const unsigned int n2, const float scale);

int Correlate_int  (unsigned int * restrict g2, const  int64_t * restrict cl1, const  int64_t * restrict cl2, const unsigned int sizeg2, const unsigned int n1, const unsigned int n2, const unsigned int bitshift);

int main(int argc, char *argv[])
{
  	int64_t *list1, *list2, list1_start, list2_start;
	unsigned int *g2;
	FILE *fpout;
	int fpin_list1, fpin_list2;
	int64_t list1_size, list2_size;
	int64_t result, tag;
	int i;
	int g2size;
	int bin_width_i;
	float bin_width_f, scale;
	time_t   start, finish;
	struct stat64 stat_list1, stat_list2;	
	double duration;
	
	time(&start);

	// Check if all parameters were entered
	if(argc!=7)
	{
		printf("\nUsage: g2calc method list1file list2file g2file g2size bin_width");
		printf("\nmethod can be:");
		printf("\n\t 1 : bin_width is a float in units of 4ps (time width of one bin is n x 4ps)");
		printf("\n\t 2 : bin_width is actually n, and 2^n x 4ps is the time width of one bin)");
//		printf("\n\t 3 : like method 2, but potentially faster (beta stage)");
//		printf("\n\t 10 : like method 1, process data in chunks (beta stage)");
//		printf("\n\t 20 : like method 2, process data in chunks (beta stage)");
		printf("\nlist1file and list2file are binary files spit by makelists.exe");
		printf("\ng2file will be the output, format ASCII, first column contains time of bin in ns, second column contains coincidences.");
		printf("\ng2size is the amount of desired time bins for the g2 function");
		printf("\nbin_width is the time width of each bin (units of 4ps)");
		printf("\n");

		return -1;
    }
	
	// Check validity for each parameters
	g2size = atoi(argv[5]);
	if (g2size<0)
	{
		printf("\nInvalid g2size, aborting");
		return -1;
	}


	//Find out the sizes of list1 and list2
	if ( -1 ==  stat64 (argv[2], &stat_list1))	
    {
      printf(" Error occurred attempting to stat %s\n", argv[2]);
      return -1;
    }
	list1_size = stat_list1.st_size/sizeof(int64_t);
    printf("list1 size is   %lld bytes --> %lld events\n", stat_list1.st_size, list1_size);
	if ( -1 ==  stat64 (argv[3], &stat_list2))	
    {
      printf(" Error occurred attempting to stat %s\n", argv[2]);
      return -1;
    }
	list2_size = stat_list2.st_size/sizeof(int64_t);
    printf("list2 size is   %lld bytes --> %lld events\n", stat_list2.st_size, list2_size);


	// Open files for input and output
	// if((fpin_list1=open(argv[2],O_RDONLY|O_BINARY))==-1)
	// {
	// 	printf("\ncannot open input list1file, aborting\n");
	// 	return -1;
	// }
	if(list1_size <= 0)
	{
		printf("\nlist1file has no valid size, aborting\n");
		close(fpin_list1);
		return -1;
	}
		
 //    if((fpin_list2=open(argv[3],O_RDONLY|O_BINARY))==-1)
	// {
	// 	printf("\ncannot open input list2file, aborting\n");
	// 	close(fpin_list1);
	// 	return -1;
	// }
	if(list2_size <= 0)
	{
		printf("\nlist2file has no valid size, aborting\n");
		close(fpin_list1);
		close(fpin_list2);
		return -1;
	}
	if((fpout=fopen(argv[4],"w"))==NULL)
	{
		printf("\ncannot open output file g2file, aborting\n");
		close(fpin_list1);
		close(fpin_list2);
		return -1;
	}


	//Allocating memory
	//list1 = (int64_t *) _aligned_malloc(list1_size*sizeof(int64_t), MEM_ALIGNMENT);
	list1 = (int64_t *) malloc(list1_size*sizeof(int64_t));
//	list1 = (int64_t *) memalign(sizeof(int64_t),list1_size);
	if (list1 == NULL)
	{
		printf("\nCould not allocate memory for list1, aborting");
		close(fpin_list1);
		close(fpin_list2);
		fclose(fpout);
		return -1;
	}

	//list2 = (int64_t *) _aligned_malloc(list2_size*sizeof(int64_t), MEM_ALIGNMENT);
	list2 = (int64_t *) malloc(list2_size*sizeof(int64_t));	
	if (list2 == NULL)
	{
		printf("\nCould not allocate memory for list2, aborting");
		free(list1);
		close(fpin_list1);
		close(fpin_list2);
		fclose(fpout);
		return -1;
	}
	
    //g2 = (unsigned int*) _aligned_malloc((2*g2size+1)*sizeof(unsigned int), MEM_ALIGNMENT);
    g2 = (unsigned int*) malloc((2*g2size+1)*sizeof(unsigned int));
    //g2 = (unsigned int*) memalign(sizeof(int64_t), 2*g2size+1);
    // It is convenient to maintain the 64bit alignment
	if(g2 == NULL)
	{
		printf("\nCould not allocate memory for g2, aborting");
		free(list1);
		free(list2);
		close(fpin_list1);
		close(fpin_list2);
		fclose(fpout);
		return -1;
	}
	memset(g2, 0, (2*g2size+1)*sizeof(unsigned int)); //set all entries of g2 to 0,
	printf("\nMemory allocated!");

	printf("\nReading input files into memory...\n");
	
	// printf("%s",stat_list1.st_size);
	list1_start=list1;
	for(i=0;i<stat_list1.st_size/2000000000;i++){
		printf("\nFor-loop 1");
		result = read(fpin_list1, list1, 2000000000);
	    if (result != 2000000000)
		{
			printf("\nerror reading list1file, aborted.");
			printf("\n(Read just %lld out of 2.000.000.000 bytes chunk)", result);
			free(list1);
			free(list2);
			free(g2);
			close(fpin_list1);
			close(fpin_list2);
			fclose(fpout);
			return -1;
		}
		list1+=250000000;
	}
	
	printf("\nrest 1");
	result = read(fpin_list1, list1, stat_list1.st_size%2000000000);
    if (result != stat_list1.st_size%2000000000)
	{
		printf("\nerror reading list1file, aborted.");
		printf("\n(Read just %lld out of %lld bytes chunk)", result, stat_list1.st_size%2000000000);
		free(list1);
		free(list2);
		free(g2);
		close(fpin_list1);
		close(fpin_list2);
		fclose(fpout);
		return -1;
	}	
	
	close(fpin_list1);
	list1=list1_start;

	list2_start=list2;
	for(i=0;i<stat_list2.st_size/2000000000;i++){
		printf("\nFor-loop 2");
		result = read(fpin_list2, list2, 2000000000);
	    if (result != 2000000000)
		{
			printf("\nerror reading list2file, aborted.");
			printf("\n(Read just %lld out of 2.000.000.000 bytes chunk)", result);
			free(list1);
			free(list2);
			free(g2);
			close(fpin_list1);
			close(fpin_list2);
			fclose(fpout);
			return -1;
		}
		list2+=250000000;
	}
	
	printf("\nrest 2");
	result = read(fpin_list2, list2, stat_list2.st_size%2000000000);
    if (result != stat_list2.st_size%2000000000)
	{
		printf("\nerror reading list2file, aborted.");
		printf("\n(Read just %lld out of %lld bytes chunk)", result, stat_list2.st_size%2000000000);
		free(list1);
		free(list2);
		free(g2);
		close(fpin_list1);
		close(fpin_list2);
		fclose(fpout);
		return -1;
	}	
	
	close(fpin_list2);
	list2=list2_start;
	printf("\ndone!");

	time(&finish);
	duration = difftime(finish, start);
    printf( "\nTime spent loading data:%f seconds\n", duration );
    
    printf("\nList 1 first tag: %lld",list1[0]);
    printf("\nList 1 last tag: %lld",list1[list1_size-1]);
    
    printf("\nList 2 first tag: %lld",list2[0]);
    printf("\nList 2 last tag: %lld",list2[list2_size-1]);

	printf("\nStarting g2 calculation...");
	time(&start);
	result = -1;
	switch(atoi(argv[1]))
	{
	case 1:  bin_width_f = atof(argv[6]);
			 if (bin_width_f == 0.0f) {printf("Error: division by zero!\n"); break;} //prevent division by zero
			 scale = 1.0f/bin_width_f;
			 result = Correlate_float(g2, list1, list2, g2size, list1_size, list2_size, scale);
			 break;
/*	case 2:  bin_width_i = atoi(argv[6]); // keep in mind this is a power of 2
	         if ((bin_width_i >63) || (bin_width_i<0)) break;
	         scale = 1.0f/pow(2, bin_width_i);
			 result = Correlate_int(g2, list1, list2, g2size, list1_size, list2_size, bin_width_i);
			 break;
	case 11: bin_width_f = atof(argv[6]);
			 if (bin_width_f == 0.0f) break; //prevent division by zero
			 scale = 1.0f/bin_width_f;
			 result = Correlate_float_fast(g2, list1, list2, g2size, list1_size, list2_size, scale);
			 break;
	case 3: bin_width_i = atoi(argv[6]); // keep in mind this is a power of 2
			result = Correlate_4int(g2, list1, list2, g2size, list1_size, list2_size, bin_width_i);
			break;
	case 10: bin_width_f = atof(argv[6]);
			if (bin_width_f == 0.0f) break;
			scale = 1.0f/bin_width_f;
			result = Correlate_float_chunk(g2, list1, list2, g2size, list1_size, list2_size, scale, 256*1024/8);
			break;
	case 20: bin_width_i = atoi(argv[6]);
			result = Correlate_int_chunk(g2, list1, list2, g2size, list1_size, list2_size, bin_width_i, 256*1024/8);
			break;
*/	
    default: printf("\nThis method does not exist!\n"); 
             break;
	}
	time(&finish);
	duration = difftime(finish, start);
    printf( "\nFinished! Time elapsed:%f seconds\n", duration );

	printf("\nWriting g2 to file...");
	for (i=0; i<2*g2size+1; i++)
			fprintf(fpout, "%.3f\t%d\n",0.078/scale*(i-g2size),g2[i]);
	printf("done!\n");

	//Housekeeping
	free(list1);
	free(list2);
	free(g2);
	fclose(fpout);
	
	return result;
}

//-----------------------------------------------------------------------
int Correlate_float(unsigned int * restrict g2, const  int64_t * restrict cl1, const  int64_t * restrict cl2, const unsigned int sizeg2, const int64_t n1, const int64_t n2, const float scale)
//Calculate g2 function out of the list of clicks from detectors 1 and 2
{

	 int bin_i=0,i=0;
	 int64_t i1, i2, i2s=0;
	 unsigned int TwoTimesSizeg2 = 2*sizeg2;
	 double offset = sizeg2;
     double bin_f;
#if G2DEBUG
	 unsigned int x=0,y=0;
#endif

     for (i1=0; i1<n1; i1++)
		 for (i2=i2s; i2<n2 ; i2++)
         {
#if G2DEBUG
			 x++;
#endif
             bin_f = (cl2[i2]-cl1[i1])*scale + offset;
             //printf("%lld %lld %f\n",cl1[i1], cl2[i2], bin_f);
             //i++;
             //if(i%20==0){
             //	getch();
             //	i=0;
             // }
			 if(bin_f < 0.0f) //See note at the end of this routine
				i2s=i2;     //mark at which position we stopped seeing garbage
			 else
			 {
				 bin_i = (int)bin_f;
				 if (bin_i<=TwoTimesSizeg2)
				 {

					 g2[bin_i]++;
#if G2DEBUG
					 y++;
#endif
				 }
				 else
					 break; //assuming time ordered lists, the next element of cl2
			 				//will also not fulfill this inequality
			 }//outer if
      }//inner and outer for
#if G2DEBUG
	 printf("\nAnalysed %d pairs, %d of which within range", x,y);
#endif

// Note: you may question why did we compare (bin_f < 0.0f) instead of (bin_i < 0)
// Keep in mind all values between -0.99(9) and +0.(9) are truncated to 0
// Thus, this bin would be twice as thick as any other bin.
// Taking the comparison on the float avoids this problem

	 return 0;
}

/*

//-----------------------------------------------------------------------
int Correlate_float_fast(unsigned int * restrict g2, const  int64_t * restrict cl1, const  int64_t * restrict cl2, const unsigned int sizeg2, const unsigned int n1, const unsigned int n2, const float scale)
//Calculate g2 function out of the list of clicks from detectors 1 and 2
//Evolution of Correlate_float
{

	 int64_t difference;
     int bin_i=0;
	 unsigned int i1, i2, i2s=0;
	 unsigned int TwoTimesSizeg2 = 2*sizeg2;
	 float offset = sizeg2;
     float bin_f;
#if G2DEBUG
	 unsigned int x=0,y=0;
#endif

     for (i1=0; i1<n1; i1++)
		 for (i2=i2s; i2<n2 ; i2++)
         {
#if G2DEBUG
			 x++;
#endif
             difference = cl2[i2]-cl1[i1];
             bin_f = (difference)*scale + offset;
//			 if(bin_f < 0.0f) //See note at the end of this routine
             if(FLOAT2UINTCAST(bin_f) > 0x80000000U)
				i2s=i2;     //mark at which position we stopped seeing garbage
			 else
			 {
				 bin_i = (int)bin_f;
				 if (bin_i<=TwoTimesSizeg2)
				 {

					 g2[bin_i]++;
#if G2DEBUG
					 y++;
#endif
				 }
				 else
					 break; //assuming time ordered lists, the next element of cl2
			 				//will also not fulfill this inequality
			 }//outer if
      }//inner and outer for
#if G2DEBUG
	 printf("\nAnalysed %d pairs, %d of which within range", x,y);
#endif

// Note: you may question why did we compare (bin_f < 0.0f) instead of (bin_i < 0)
// Keep in mind all values between -0.99(9) and +0.(9) are truncated to 0
// Thus, this bin would be twice as thick as any other bin.
// Taking the comparison on the float avoids this problem

	 return 0;
}


//-----------------------------------------------------------------------
int Correlate_float_4fast(unsigned int * restrict g2, const  int64_t * restrict cl1, const  int64_t * restrict cl2, const unsigned int sizeg2, const unsigned int n1, const unsigned int n2, const float scale)
//Calculate g2 function out of the list of clicks from detectors 1 and 2
//Evolution of Correlate_float_fast
{

	 int64_t difference;
     int bin_i=0;
	 unsigned int i1, i2, i2s=0;
	 unsigned int TwoTimesSizeg2 = 2*sizeg2;
	 float offset = sizeg2;
     float bin_f;

     for (i1=0; i1<n1; i1++)
		 for (i2=i2s; i2<n2 ; i2++)
         {
             difference = cl2[i2]-cl1[i1];
             bin_f = (difference)*scale + offset;
//			 if(bin_f < 0.0f) //See note at the end of this routine
             if(FLOAT2UINTCAST(bin_f) > 0x80000000U)
				i2s=i2;     //mark at which position we stopped seeing garbage
			 else
			 {
				 bin_i = (int)bin_f;
				 if (bin_i<=TwoTimesSizeg2)
				 {
					 g2[bin_i]++;
				 }
				 else
					 break; //assuming time ordered lists, the next element of cl2
			 				//will also not fulfill this inequality
			 }//outer if
      }//inner and outer for

// Note: you may question why did we compare (bin_f < 0.0f) instead of (bin_i < 0)
// Keep in mind all values between -0.99(9) and +0.(9) are truncated to 0
// Thus, this bin would be twice as thick as any other bin.
// Taking the comparison on the float avoids this problem

	 return 0;
}


//-----------------------------------------------------------------------
int Correlate_int  (unsigned int * restrict g2, const  int64_t * restrict cl1, const  int64_t * restrict cl2, const unsigned int sizeg2, const unsigned int n1, const unsigned int n2, const unsigned int bitshift)
//Calculate g2 function out of the list of clicks from detectors 1 and 2
{

	 unsigned int bin;
	 unsigned int i1, i2, i2s=0;
	 int64_t TwoTimesSizeg2 = 2*sizeg2;
	 int64_t offset = sizeg2;
     int64_t bin64;
#if G2DEBUG
	 unsigned int x=0,y=0;
#endif
     for (i1=0; i1<n1; i1++)
     {
         for (i2=i2s; i2<n2; i2++)
         {
             bin64 = ((cl2[i2]-cl1[i1]) >> bitshift) + offset;
#if G2DEBUG
			 x++;
#endif
             if (bin64<0)
				i2s=i2;
			 else
			 {
				 if (bin64<=TwoTimesSizeg2)
				 {
	  				bin = (unsigned int)bin64;  //convert int64 to int32
					g2[bin]++;
#if G2DEBUG
					y++;
#endif
				 }
				 else
					 break;
			 }
         }
     }
#if G2DEBUG
	 printf("\nAnalysed %d pairs, %d of which within range", x,y);
#endif
	 return 0;
}



//-----------------------------------------------------------------------
int Correlate_4int  (unsigned int * restrict g2, const  int64_t * restrict cl1, const  int64_t * restrict cl2, const unsigned int sizeg2, const unsigned int n1, const unsigned int n2, const unsigned int bitshift)
//Calculate g2 function out of the list of clicks from detectors 1 and 2
//Evolution from Correlate_int
{
#if 0
	 __declspec(align(16)) int64_t bin64_0, bin64_1, bin64_2, bin64_3;
	 __declspec(align(16)) const int offset = sizeg2;
	 __declspec(align(16)) const int TwoTimesSizeg2 = 2*sizeg2;
	 __declspec(align(16)) unsigned int i1, i2, i2s=0;
	 __declspec(align(16)) unsigned int bin0, bin1, bin2, bin3;
	 __declspec(align(16)) int flags, flag0, flag1, flag2, flag3;
#endif
	 int64_t bin64_0, bin64_1, bin64_2, bin64_3;
	 const int offset = sizeg2;
	 const int TwoTimesSizeg2 = 2*sizeg2;
	 unsigned int i1, i2, i2s=0;
	 unsigned int bin0, bin1, bin2, bin3;
	 int flags, flag0, flag1, flag2, flag3;

#if G2DEBUG
	 unsigned int x=0,y=0;
#endif
     for (i1=0; i1<n1; i1++)
     {
          for (i2=i2s; i2<n2 ; i2+=4)
          {
             bin64_0 = ((cl2[i2+0]-cl1[i1]) >> bitshift) + offset;
             bin64_1 = ((cl2[i2+1]-cl1[i1]) >> bitshift) + offset;
             bin64_2 = ((cl2[i2+2]-cl1[i1]) >> bitshift) + offset;
             bin64_3 = ((cl2[i2+3]-cl1[i1]) >> bitshift) + offset;

			 flag0 = (bin64_0 >= 0);
			 flag1 = (bin64_1 >= 0);
			 flag2 = (bin64_2 >= 0);
			 flag3 = (bin64_3 >= 0);
#if G2DEBUG
			 x+=4;
#endif

 		     flags = flag0 || flag1 || flag2 || flag3; 
			 if (flags)										 // if any is non-negative
			 {
				 flag0 = (flag0 && (bin64_0 <= TwoTimesSizeg2)); // true = 1, false = 0;
				 flag1 = (flag1 && (bin64_1 <= TwoTimesSizeg2));
				 flag2 = (flag2 && (bin64_2 <= TwoTimesSizeg2));
				 flag3 = (flag3 && (bin64_3 <= TwoTimesSizeg2));
 
				 bin0 = (unsigned int) (bin64_0);
				 bin1 = (unsigned int) (bin64_1);
				 bin2 = (unsigned int) (bin64_2);
				 bin3 = (unsigned int) (bin64_3);

#if G2DEBUG
				 if(flag0) {g2[bin0]++; y++;}
				 if(flag1) {g2[bin1]++; y++;}
				 if(flag2) {g2[bin2]++; y++;}
				 if(flag3) {g2[bin3]++; y++;}

#else			 
				 if(flag0) g2[bin0]++;
				 if(flag1) g2[bin1]++;
				 if(flag2) g2[bin2]++;
				 if(flag3) g2[bin3]++;
#endif
				 flags = !(flag0 || flag1 || flag2 || flag3); //are all out of range?
				 if (flags) break;

			 } else
				 i2s=i2;
          }// end for i2        
     }// end for i1
#if G2DEBUG
 	 printf("\nAnalysed %d pairs, %d of which within range", x,y);
#endif

	 return 0;
}
*/


