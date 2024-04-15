clc; clear all; close all;

% Image parameters
image_width = 1024
image_height = 1024
image_size = image_width * image_height
input_file_name = 'imagette.bin'


%% Main

% Load Image from binary file
fileID = fopen(input_file_name, 'r');
Imagette = fread(fileID, [image_height, image_width]);


% Motif bayer   |B G|
%               |G R|


%% Funtions

%% Malvar
Im_Malvar = Fct_Malvar(Imagette);
Im_Malvar(find(Im_Malvar<0)) = 0;

% Save Malvar result [R0, G0, B0, R1, G1, B1, ... ]
malvar_output = zeros(1,3*image_size);
malvar_output(1:3:end) = Im_Malvar(:,:,1);
malvar_output(2:3:end) = Im_Malvar(:,:,2);
malvar_output(3:3:end) = Im_Malvar(:,:,3);

fileID = fopen('output_malvar_matlab.bin', 'w');
fwrite(fileID, malvar_output, 'uint8');
fclose(fileID);


%% Anscombe (need signle of double input because of matlab sqrt)
Im_Ans = zeros(size(Im_Malvar));

for i=1:3
    Im_Ans(:,:,i) = Fct_Anscombe_Transform(double(Im_Malvar(:,:,i)),0.1,0.2);
end


%% RGB -> L,Ch1, Ch2
Im_LCh = Fct_RGB_to_YUV(Im_Ans,1);


%% Filtrage
Im_Fil = zeros(size(Im_LCh));

for j=1:3
    Im_Fil(:,:,j) = Fct_DWT_CCSDS_single_level (Im_LCh(:,:,j));
end

% Save final filtered YUV_1 output
fileID = fopen('output_filter_matlab.bin', 'w');
fwrite(fileID, Im_Fil(:,:,1), 'uint8');
fclose(fileID);

