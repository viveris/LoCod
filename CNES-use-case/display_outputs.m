clc; clear all; close all;

% Parameters
output_malvar_matlab_file_name = 'output_files/output_malvar_matlab.bin'
output_filter_matlab_file_name = 'output_files/output_filter_matlab.bin'
output_malvar_ccode_file_name = 'output_files/output_malvar_ccode.bin'
output_filter_ccode_file_name = 'output_files/output_filter_ccode.bin'
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

% *** CCode ***
fileID = fopen(output_malvar_ccode_file_name, 'r');
image = fread(fileID);
% Separate image layers, format in bin is [R0, G0, B0, R1, G1, B1, ... ]
R = image(1:3:end);
G = image(2:3:end);
B = image(3:3:end);
% Reshape layers
R = reshape(R, [1024, 1024]);
G = reshape(G, [1024, 1024]);
B = reshape(B, [1024, 1024]);
img_malvar_ccode = cat(3,R,G,B);

% *** Matlab ***
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

% *** Display Malvar outputs ***
figure('Name','Malvar outpus');
subplot(3,1,1);
imshow(rescale(img_malvar_matlab));
title('Malvar Matlab');
subplot(3,1,2);
imshow(rescale(img_malvar_ccode));
title('Malvar C Code');
subplot(3,1,3);
imshow(rescale(img_malvar_locod));
title('Malvar Locod');


%-------------------------------------------------------------------------------
%                              Filter outputs
%-------------------------------------------------------------------------------
% *** Matlab ***
fileID = fopen(output_filter_matlab_file_name, 'r');
image = fread(fileID);
img_filter_matlab = reshape(image, [1024, 1024]);

% *** CCode ***
fileID = fopen(output_filter_ccode_file_name, 'r');
image = fread(fileID);
img_filter_ccode = reshape(image, [1020, 1020]);

% *** Matlab ***
fileID = fopen(output_filter_locod_file_name, 'r');
image = fread(fileID);
img_filter_locod = reshape(image, [1020, 1020]);

% *** Display Filter outputs ***
figure('Name','Filter outpus');
subplot(3,1,1);
imshow(rescale(img_filter_matlab));
title('Filter Matlab');
subplot(3,1,2);
imshow(rescale(img_filter_ccode));
title('Filter C Code');
subplot(3,1,3);
imshow(rescale(img_filter_locod));
title('Filter Locod');