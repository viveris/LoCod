clc; clear all; close all;

% Parameters
output_malvar_matlab_file_name = 'output_files/output_malvar_matlab.bin'
output_filter_matlab_file_name = 'output_files/output_filter_matlab.bin'
output_malvar_locod_file_name = 'output_files/output_malvar_locod.bin'
output_filter_locod_file_name = 'output_files/output_filter_locod.bin'

%-------------------------------------------------------------------------------
%                              Malvar outputs
%-------------------------------------------------------------------------------
% *** Matlab ***
fileID = fopen(output_malvar_matlab_file_name, 'r');
image = fread(fileID);
% Separate image layers, format in bin is [R0, G0, B0, R1, G1, B1, ... ]
R = image(1:3:end);
G = image(2:3:end);
B = image(3:3:end);
% Reshape layers
R = reshape(R, [1024, 1024]);
G = reshape(G, [1024, 1024]);
B = reshape(B, [1024, 1024]);
img_malvar_matlab = cat(3,R,G,B);

% *** LoCod ***
fileID = fopen(output_malvar_locod_file_name, 'r');
image = fread(fileID);
% Separate image layers, format in bin is [R0, G0, B0, R1, G1, B1, ... ]
R = image(1:3:end);
G = image(2:3:end);
B = image(3:3:end);
% Reshape layers
R = reshape(R, [1024, 1024]);
G = reshape(G, [1024, 1024]);
B = reshape(B, [1024, 1024]);
img_malvar_locod = cat(3,R,G,B);

% *** Rescale values ***
img_malvar_matlab = (img_malvar_matlab - 0) / (255 - 0);
img_malvar_locod = (img_malvar_locod - 0) / (255 - 0);

% *** Display Malvar outputs ***
figure('Name','Malvar outpus');
subplot(1,2,1);
imshow(img_malvar_matlab);
title('Malvar Matlab');
subplot(1,2,2);
imshow(img_malvar_locod);
title('Malvar Locod');


%-------------------------------------------------------------------------------
%                              Filter outputs
%-------------------------------------------------------------------------------
% *** Matlab ***
fileID = fopen(output_filter_matlab_file_name, 'r');
image = fread(fileID);
img_filter_matlab = reshape(image, [1024, 1024]);

% *** LoCod ***
fileID = fopen(output_filter_locod_file_name, 'r');
image = fread(fileID);
img_filter_locod = reshape(image, [1020, 1020]);

% *** Rescale values ***
img_filter_matlab = (img_filter_matlab - 0) / (255 - 0);
img_filter_locod = (img_filter_locod - 0) / (255 - 0);

% *** Display Filter outputs ***
figure('Name','Filter outpus');
subplot(1,2,1);
imshow(img_filter_matlab);
title('Filter Matlab');
subplot(1,2,2);
imshow(img_filter_locod);
title('Filter Locod');
