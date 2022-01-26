#ifndef _FIR_H_ 
#define _FIR_H_
//#include "ap_cint.h"
#define N	396
#define N2  N
#define SAMPLES N2+N+10 // just few more samples then number of taps
typedef long	coef_t;
typedef long	data_t;
typedef int	acc_t;
    
void fir_filter (data_t *y, data_t x);
void fir_filter2 (data_t *y, data_t x);

#endif