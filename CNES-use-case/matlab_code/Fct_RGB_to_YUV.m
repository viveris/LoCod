% *** Code provided by CNES in the frame of the LoCod study ***

function YUV=Fct_RGB_to_YUV(RGB,plot_flag)
R = RGB(:,:,1);
G = RGB(:,:,2);
B = RGB(:,:,3);

YUV = zeros(size(RGB));

%Conversion Formula
YUV(:,:,1) = 0.299   * R + 0.587   * G + 0.114 * B;
YUV(:,:,2) = 128 - 0.168736 * R - 0.331264 * G + 0.5 * B;
YUV(:,:,3) = 128 + 0.5 * R - 0.418688 * G - 0.081312 * B;


end
