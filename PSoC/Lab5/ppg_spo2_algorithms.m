
%% Start
clear all

%% Load sample data

uch_spo2_table =[ 95, 95, 95, 96, 96, 96, 97, 97, 97, 97, 97, 98, 98, 98, 98, 98, 99, 99, 99, 99, ...
                  99, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, ...
                  100, 100, 100, 100, 99, 99, 99, 99, 99, 99, 99, 99, 98, 98, 98, 98, 98, 98, 97, 97, ...
                  97, 97, 96, 96, 96, 96, 95, 95, 95, 94, 94, 94, 93, 93, 93, 92, 92, 92, 91, 91, ...
                  90, 90, 89, 89, 89, 88, 88, 87, 87, 86, 86, 85, 85, 84, 84, 83, 82, 82, 81, 81, ...
                  80, 80, 79, 78, 78, 77, 76, 76, 75, 74, 74, 73, 72, 72, 71, 70, 69, 69, 68, 67, ...
                  66, 66, 65, 64, 63, 62, 62, 61, 60, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50, ...
                  49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 31, 30, 29, ...
                  28, 27, 26, 25, 23, 22, 21, 20, 19, 17, 16, 15, 14, 12, 11, 10, 9, 7, 6, 5, ...
                  3, 2, 1] ;




[X1,X2]=textread('custom.log','%d %d');
window_size = 310;
window = 1;
data_length = 0;
overlapping_window_size  = 300;
SpO2_val = [];

for loop=1:(length(X1)/window_size) + 30
    if(window == 1)
        data_st = data_length + 1;
        data_length = (window * window_size); % Data length
    else
        data_st = (data_length + 1) - overlapping_window_size;
        data_length =  data_length + (window_size - overlapping_window_size); % Data length
    end
    fprintf('Test %i, %i \n', data_st, data_length);
    for i=data_st:data_length
        X(i,1)=X1(i); 
        X(i,2)=X2(i);
  
    end

    %% Data input for Heart rate and SpO2 calculation
    y1 = X(data_st:data_length,1); %RED
    y2 = X(data_st:data_length,2); %IR 

    fs=125; %sampling rate 25Hz
    NFFT=256; % FFT size

    % %% Beat count for heart rate
    % 
    % % Moving average filter
    % for i=1:(length(y2)-fs/5)
    %     local_sum=0;
    %     for j=1:fs/5
    %         local_sum=local_sum+y2(i+j);
    %     end
    %         y(i)=local_sum/(fs/5);
    % end
    % 
    % % Find peaks
    % leap=0;
    % pk_i=1;
    % 
    % while leap<=(length(y)-fs)
    %     
    % for i=1:fs
    %     yy(i) = y(i+leap);
    % end
    % 
    % local_i_max = 1;
    % local_max = yy(local_i_max);
    % 
    % for i=2:fs
    %     if local_max<yy(i)
    %         local_i_max = i;
    %         local_max=yy(i);
    %     end
    % end
    % 
    % pk(pk_i)=leap+local_i_max;
    % pk_i=pk_i+1;
    % leap =leap + fs;
    % end
    % 
    % beat = 0;
    % beat_i = 1;
    % for i=1:length(pk)-1
    %     if pk(i)<(pk(i+1)-10)    
    %         beat=beat+(fs/(pk(i+1)-pk(i)))*60;
    %         beat_i=beat_i+1;
    %     end
    % end
    % 
    % if beat_i>1
    %     HEART_RATE=beat/(beat_i-1)
    % end

    %% SpO2 level calculation AC/DC
    
    invertedY = max(y2) - y2;
    [peakValues, an_ir_valley_locs] = findpeaks(invertedY);
    n_exact_ir_valley_locs_count = length(an_ir_valley_locs);
    n_i_ratio_count = 1;
    for k=1:(n_exact_ir_valley_locs_count - 1)
        n_y_dc_max= -16777216 ; 
        n_x_dc_max= -16777216; 
        if (an_ir_valley_locs(k+1)-an_ir_valley_locs(k) >3)
            for i=an_ir_valley_locs(k):an_ir_valley_locs(k+1)
            
              if (y2(i)> n_x_dc_max) 
                  n_x_dc_max =y2(i); n_x_dc_max_idx=i;
              end
              if (y1(i)> n_y_dc_max) 
                  n_y_dc_max =y1(i); n_y_dc_max_idx=i;
              end
            end
          n_y_ac= (y1(an_ir_valley_locs(k+1)) - y1(an_ir_valley_locs(k))) *(n_y_dc_max_idx -an_ir_valley_locs(k));
          n_y_ac=  y1(an_ir_valley_locs(k)) + n_y_ac/ (an_ir_valley_locs(k+1) - an_ir_valley_locs(k))  ; 
          n_y_ac=  y1(n_y_dc_max_idx) - n_y_ac;    
          n_x_ac= (y2(an_ir_valley_locs(k+1)) - y2(an_ir_valley_locs(k) ) )*(n_x_dc_max_idx -an_ir_valley_locs(k)); 
          n_x_ac=  y2(an_ir_valley_locs(k)) + n_x_ac/ (an_ir_valley_locs(k+1) - an_ir_valley_locs(k)); 
          n_x_ac=  y2(n_y_dc_max_idx) - n_x_ac;     
          n_nume=( n_y_ac *n_x_dc_max);
          n_denom= ( n_x_ac *n_y_dc_max);
          if (n_denom>0  && n_i_ratio_count <5 &&  n_nume ~= 0)
            
            an_ratio(n_i_ratio_count)= (n_nume*100)/n_denom ; 
            n_i_ratio_count = n_i_ratio_count + 1;
          end
        end
    end
      
      an_ratio = sort(an_ratio,'ascend');
      n_middle_idx= int16(n_i_ratio_count/2);
       
      if (n_middle_idx >1)
        n_ratio_average =int16((an_ratio(n_middle_idx-1) +an_ratio(n_middle_idx))/2); 
      else
        n_ratio_average = an_ratio(n_middle_idx);
      end
      if(n_ratio_average>2 && n_ratio_average <184)
        n_spo2_calc= uch_spo2_table(n_ratio_average);
        
        SpO2_val_met1(window) = n_spo2_calc;
      end
  
    %% SpO2 level calculation

    %%FFT for RED signal
    Y1 = fft(y1,NFFT);

    figure(1)
    L = NFFT;
    hold on
    P2 = abs(Y1/L);
    P1 = P2(1:L/2+1);
    P1(2:end-1) = 2*P1(2:end-1);
    f = fs*(0:(L/2))/L;
    plot(f,P1) 
    title('Single-Sided Amplitude Spectrum of red channel')
    xlabel('f (Hz)')
    ylabel('|P1(f)|')



    %Find local maximum in RED spectrum
    st = 6;

    YY=abs(Y1(st:12));
    local_max_i=1;
    local_max=YY(1);
    for i=2:(length(YY)-1)
        if local_max<(YY(i))
            local_max_i=i;
            local_max=YY(i);
        end    
    end
    pk_RED_i=st-1+local_max_i;

    %%FFT for IR
    Y2 = fft(y2,NFFT);

    % Find local maximum in RED spectrum
    figure(2)    
    YY=abs(Y2(st:12));
    hold on
    L = NFFT;
    P2 = abs(Y2/L);
    P1 = P2(1:L/2+1);
    P1(2:end-1) = 2*P1(2:end-1);
    f = fs*(0:(L/2))/L;
    plot(f,P1) 
    title('Single-Sided Amplitude Spectrum of X(t)')
    xlabel('f (Hz)')
    ylabel('|P1(f)|')

    local_max_i=1;
    local_max=YY(1);
    for i=2:(length(YY)-1)
        if local_max<(YY(i))
            local_max_i=i;
            local_max=YY(i);
        end    
    end
    pk_IR_i=st-1+local_max_i;

    %%SpO2 
    R_RED = abs(Y1(pk_RED_i)/abs(Y1(1)));
    R_IR = abs(Y2(pk_IR_i)/abs(Y2(1)));
    R=R_RED/R_IR;
    if(104 - 28*R > 100)
        SpO2_val(window) = 100;
    else
        SpO2_val(window) = 104 - 28*R;
    end
    window = window + 1;
end

% NEXT









figure(3)
samples = 1:length(X1);
t = samples/fs;

ax2 = subplot(2,1,2)
samples = 1:length(SpO2_val);
t = samples/fs;
title("Oxygen saturation")
plot(t*(length(X1)/length(SpO2_val)),SpO2_val, 'green');
hold on
plot(t*(length(X1)/length(SpO2_val)),SpO2_val_met1,'blue');
ylim([90 110])
legend('FFT method', 'Time domain method');
%% END
