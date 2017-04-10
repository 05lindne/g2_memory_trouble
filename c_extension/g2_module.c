#include "g2_module.h"

int test(int* g2, int* cl1, const int scale)	{

    for (int i=0; i<scale;i++) {
    	g2[i] = g2[i]*2;
    }
		
	return 10;
}

// int Correlate_float(unsigned int & g2, const  int64_t & cl1, const float scale)
// {

// 	 int bin_i=0,i=0;
// 	 int64_t i1, i2, i2s=0;
// 	 unsigned int TwoTimesSizeg2 = 2*sizeg2;
// 	 double offset = sizeg2;
//      double bin_f;

//      for (i1=0; i1<n1; i1++)
// 		 for (i2=i2s; i2<n2 ; i2++)
//          {

//              bin_f = (cl2[i2]-cl1[i1])*scale + offset;
             
// 			 if(bin_f < 0.0f) 
// 				i2s=i2;     //mark at which position we stopped seeing garbage
// 			 else
// 			 {
// 				 bin_i = (int)bin_f;
// 				 if (bin_i<=TwoTimesSizeg2)
// 				 {

// 					 g2[bin_i]++;

// 				 }
// 				 else
// 					 break; 
			 				
// 			 }//outer if
//       }//inner and outer for

// 	return 0;
// }