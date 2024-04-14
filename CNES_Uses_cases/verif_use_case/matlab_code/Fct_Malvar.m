function Image_Demosaic =Fct_Malvar(I)

% for bayer parten RGGB


%% kernels (figure 2 - ICASSP04 HIGH-QUALITY LINEAR INTERPOLATION FOR DEMOSAICING OF BAYER-PATTERNED COLOR IMAGES

G_at_RB = [0 0 -1 0 0; 0 0 2 0 0; -1 2 4 2 -1; 0 0 2 0 0; 0 0 -1 0 0];

R1 = [0 0 0.5 0 0; 0 -1 0 -1 0; -1 4 5 4 -1; 0 -1 0 -1 0; 0 0 0.5 0 0];
R2 = [0 0 -1 0 0; 0 -1 4 -1 0; 0.5 0 5 0 0.5; 0 -1 4 -1 0; 0 0 -1 0 0];
R3 = [0 0 -3/2 0 0; 0 2 0 2 0; -3/2 0 6 0 -3/2; 0 2 0 2 0; 0 0 -3/2 0 0];

B1 = [0 0 0.5 0 0; 0 -1 0 -1 0; -1 4 5 4 -1; 0 -1 0 -1 0; 0 0 0.5 0 0];
B2 = [0 0 -1 0 0; 0 -1 4 -1 0; 0.5 0 5 0 0.5; 0 -1 4 -1 0; 0 0 -1 0 0];
B3 = [0 0 -3/2 0 0; 0 2 0 2 0; -3/2 0 6 0 -3/2; 0 2 0 2 0; 0 0 -3/2 0 0];



%% extract chanel

Green_Channel   = zeros(size(I));
Red_Channel     = zeros(size(I));
Blue_Channel    = zeros(size(I));

for ind_line=0:2:size(I,1)-3 %on avance sur la ligne d'un motif de bayer entier, ici 2 pixel


    for ind_cols=0:2:size(I,2)-3 %on avance sur la ligne d'un motif de bayer entier, ici 2 pixel

        %Extraction
        Green_Channel   (ind_line+1,ind_cols+2) = I (ind_line+1,ind_cols+2);
        Green_Channel   (ind_line+2,ind_cols+1) = I (ind_line+2,ind_cols+1);
        Red_Channel     (ind_line+1,ind_cols+1) = I (ind_line+1,ind_cols+1);
        Blue_Channel    (ind_line+2,ind_cols+2) = I (ind_line+2,ind_cols+2);

        %Interpolation - No border management for simplification
        if ind_line+1 > 2 && ind_line+1 < size(I,1)- 2
            if ind_cols+1 > 2 && ind_cols+1 < size(I,2)- 2

                % Green Interp
                Green_Channel   (ind_line+1,ind_cols+1) = sum(sum( I (ind_line+1-2:ind_line+1+2,ind_cols+1-2:ind_cols+1+2).* G_at_RB))/8 ; % green at red location
                Green_Channel   (ind_line+2,ind_cols+2) = sum(sum( I (ind_line+2-2:ind_line+2+2,ind_cols+2-2:ind_cols+2+2).* G_at_RB))/8 ;


                % Red
                Red_Channel     (ind_line+1,ind_cols+2) = sum(sum( I (ind_line+1-2:ind_line+1+2,ind_cols+2-2:ind_cols+2+2).* R1))/8;
                Red_Channel     (ind_line+2,ind_cols+1) = sum(sum( I (ind_line+2-2:ind_line+2+2,ind_cols+1-2:ind_cols+1+2).* R2))/8;
                Red_Channel     (ind_line+2,ind_cols+2) = sum(sum( I (ind_line+2-2:ind_line+2+2,ind_cols+2-2:ind_cols+2+2).* R3))/8;


                %Blue
                Blue_Channel     (ind_line+1,ind_cols+1) = sum(sum( I (ind_line+1-2:ind_line+1+2,ind_cols+1-2:ind_cols+1+2).* B3))/8;
                Blue_Channel     (ind_line+1,ind_cols+2) = sum(sum( I (ind_line+1-2:ind_line+1+2,ind_cols+2-2:ind_cols+2+2).* B2))/8;
                Blue_Channel     (ind_line+2,ind_cols+1) = sum(sum( I (ind_line+2-2:ind_line+2+2,ind_cols+1-2:ind_cols+1+2).* B1))/8;

            end

        end

    end

end


Image_Demosaic = cat(3, Red_Channel, Green_Channel, Blue_Channel);


end
