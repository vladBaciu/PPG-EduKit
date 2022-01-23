                                                                                       /* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "fft_lib.h"

#define two(x)       (1 << (x))                  /* \(2\sp{x}\) by left-shifting */

int bitrev(n, B)
int n, B;
{
       int m, r;

       for (r=0, m=B-1; m>=0; m--)
          if ((n >> m) == 1) {                   /* if \(2\sp{m}\) term is present, then */
             r += two(B-1-m);                    /* add \(2\sp{B-1-m}\) to \(r\), and */
             n -= two(m);                        /* subtract \(2\sp{m}\) from \(n\) */
             }

       return(r);
}

void FFT(double complex x[], int n)
{

    //bit reverse

int s = log2(n);

for(int i = 0; i < n/2; i++){
    int h = bitrev(i, s);


    double complex temp = x[i];
    x[i] = x[h];
    x[h] = temp;
}

unsigned int Np = 2; //NUM POINTS IN EACH BLOCK. INITIALLY 2
unsigned int Bp = (n/2);
for(int i = 0; i < s; i++){
    int NpP = Np>>1; //num butterflies
    int BaseT = 0;
    for (int j = 0; j < Bp; j++){

        int BaseB = BaseT + NpP;

        for(int k = 0; k < NpP; k++){

            double complex top = x[BaseT + k];
            double complex bot = (ccos(2*M_PI*k/Np) - I*csin(2*M_PI*k/Np))*x[BaseB + k];
            x[BaseT + k] = top+bot;
            x[BaseB + k] = top-bot;
        }
        BaseT = BaseT + Np;
    }
    Bp =  Bp>>1;
    Np = Np<<1;
}
}
/* [] END OF FILE */