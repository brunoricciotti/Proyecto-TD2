clear;

fs = 7e3;
fmin = 27.5;

cant_frecs = 48;


x = zeros(1,cant_frecs);
x(1) = fmin;

for i = 2:cant_frecs
  x(i) = uint16(((x(i-1)*2^(1/12))));#hay que dividirlo por fs
end

x(1) = uint16(fmin);


fileID = fopen('frecuencias.txt','w');
fprintf(fileID,'uint32_t frecs[47] = {\n');

for i = 1:5:cant_frecs-5
    fprintf(fileID,'%d,%d,%d,%d,%d,\n',x(i),x(i+1),x(i+2),x(i+3),x(i+4));
end

fprintf(fileID,'%d,%d};',x(47),x(48));

fclose(fileID);
