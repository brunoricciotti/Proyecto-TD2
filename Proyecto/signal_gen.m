fs = 20e3;
samples = 1024;

t = 0:1/fs:(samples-1)/fs;

x = uint16(2048*(1+ sin(2*pi*1000*t)));


fileID = fopen('muestras.txt','w');
fprintf(fileID,'uint32_t ADC_Buffer2[ADC_BUF_LEN] = {\n');

for i = 1:8:(length(x))
    fprintf(fileID,'%d,%d,%d,%d,%d,%d,%d,%d,\n',x(i),x(i+1),x(i+2),x(i+3),x(i+4),x(i+5),x(i+6),x(i+7));
end