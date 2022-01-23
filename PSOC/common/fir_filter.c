#include "fir_filter.h" 

void fir_filter (data_t *y, data_t x) 
{
  const coef_t c[N+1] = 
{
    #include "band_pass_5_20.dat"
};
  

  static data_t shift_reg[N];
  acc_t acc;
  int i;
  
  acc=(acc_t)shift_reg[N-1]*(acc_t)c[N];
  loop: for (i=N-1;i!=0;i--) {
    acc+=(acc_t)shift_reg[i-1]*(acc_t)c[i];
    shift_reg[i]=shift_reg[i-1];
  }
  acc+=(acc_t)x*(acc_t)c[0];
  shift_reg[0]=x;
  *y = acc>>15;
}

void fir_filter2 (data_t *y, data_t x) 
{
    
  const coef_t c2[N2+1] = 
{
    #include "band_pass_5_20.dat"
};
  

  static data_t shift_reg2[N2];
  acc_t acc;
  int i;
  
  acc=(acc_t)shift_reg2[N2-1]*(acc_t)c2[N2];
  loop: for (i=N2-1;i!=0;i--) {
    acc+=(acc_t)shift_reg2[i-1]*(acc_t)c2[i];
    shift_reg2[i]=shift_reg2[i-1];
  }
  acc+=(acc_t)x*(acc_t)c2[0];
  shift_reg2[0]=x;
  *y = acc>>15;
}