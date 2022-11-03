fs = 7e3;

fc = 27.5;
samples = 512;
RESOLUTION_ADC = 2048;

t = 0:1/fs:(samples-1)/fs;

x = uint16(RESOLUTION_ADC*(1+ sin(2*pi*fc*t)));


fileID = fopen('muestras.txt','w');
fprintf(fileID,'uint32_t ADC_Buffer2[ADC_BUF_LEN] = {\n');

for i = 1:8:(length(x))
    fprintf(fileID,'%d,%d,%d,%d,%d,%d,%d,%d,\n',x(i),x(i+1),x(i+2),x(i+3),x(i+4),x(i+5),x(i+6),x(i+7));
end

fclose(fileID)
