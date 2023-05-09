clc
clear all
close all


%% Main

% Load Image from binary file
fileID = fopen('imagette.bin', 'r');
Imagette = fread(fileID, [1024,1024]);


% Motif bayer   |B G|
%               |G R|


%% Funtions

%% Malvar
Im_Malvar_Temp = Fct_Malvar(Imagette);

R=Im_Malvar_Temp(:,:,1);
G=Im_Malvar_Temp(:,:,2);
B=Im_Malvar_Temp(:,:,3);

B(find(B<0))=0; %some malvar interpolation gives negative results, need to be verified
R(find(R<0))=0;
G(find(G<0))=0;

% Save RGB result
fileID = fopen('result_r.bin', 'w');
fwrite(fileID, R, 'uint8');
fclose(fileID);
fileID = fopen('result_g.bin', 'w');
fwrite(fileID, G, 'uint8');
fclose(fileID);
fileID = fopen('result_b.bin', 'w');
fwrite(fileID, B, 'uint8');
fclose(fileID);

Im_Malvar = cat(3,R,G,B);

figure
imshow(rescale(Im_Malvar))


%% Anscombe (need signle of double input because of matlab sqrt)
Im_Ans = zeros(size(Im_Malvar));

for i=1:3
    Im_Ans(:,:,i) = Fct_Anscombe_Transform(double(Im_Malvar(:,:,i)),0.1,0.2);
end

figure
imshow(rescale(Im_Ans))


%% RGB -> L,Ch1, Ch2
Im_LCh = Fct_RGB_to_YUV(Im_Ans,1);


%% Filtrage
Im_Fil = zeros(size(Im_LCh));

for j=1:3
    Im_Fil(:,:,j) = Fct_DWT_CCSDS_single_level (Im_LCh(:,:,j));
end

HalfSize = size(Im_Fil,1)/2;

% Plot
figure
Y_Hhf_Vhf = Im_Fil(HalfSize+1:end,HalfSize+1:end,1);
imagesc(Y_Hhf_Vhf)

figure
Y_BF = Im_Fil(1:1+512,1:1+512,1);
imagesc(Y_BF)
