clc
clear all
close all


%% Main

% Load Image

load('mandi_input_image');

% Motif bayer   |B G|
%               |G R|
Size = 48
Imagette   =   I(1+500:Size+500,1+1500:Size+1500); %crop image


fileID = fopen('imagette.bin', 'w');
fwrite(fileID, Imagette);

%

%% Funtions

%% malvar
%Im_Malvar = demosaic (Imagette, 'bggr');
Im_Malvar_Temp = Fct_Malvar(Imagette);

R=Im_Malvar_Temp(3:end-2,3:end-2,1);
G=Im_Malvar_Temp(3:end-2,3:end-2,2);
B=Im_Malvar_Temp(3:end-2,3:end-2,3);

B(find(B<0))=0; %some malvar interpolation gives negative results, need to be verified
R(find(R<0))=0;
G(find(G<0))=0;

Im_Malvar = cat(3,R,G,B);


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

HalfSize = size(Im_Ans,1)/2;

%plot
figure
Y_Hhf_Vhf = Im_Fil(HalfSize+1:end,HalfSize+1:end,1);
imagesc(Y_Hhf_Vhf)

figure
Y_BF = Im_Fil(1:((Size-4)/2),1:((Size-4)/2),1);
imagesc (Y_BF)