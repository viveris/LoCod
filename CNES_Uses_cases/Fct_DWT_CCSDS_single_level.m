function [ X2 ] = Fct_DWT_CCSDS_single_level( X )
%FCT_DWT_CCSDS_SINGLE_LEVEL Summary of this function goes here
%   Detailed explanation goes here

%% Filters coef

LP_Filter = [0.037828455507, -0.023849465020, -0.110624404418, 0.377402855613, 0.852698679009, 0.377402855613, -0.110624404418, -0.023849465020, 0.037828455507];
HP_Filter = [-0.064538882629, 0.040689417609, 0.418092273222, -0.788485616406, 0.418092273222, 0.040689417609, -0.064538882629];

LP_InvFilter = [-0.064538882629, -0.040689417609, 0.418092273222, 0.788485616406, 0.418092273222, -0.040689417609, -0.064538882629 ];
HP_InvFilter = [-0.037828455507, -0.023849465020, 0.110624404418, 0.377402855613, -0.852698679009, 0.377402855613, 0.110624404418, -0.023849465020, -0.037828455507];


%% Cols filtering

% Initialize temp var
tempo_LP1 = zeros(size(X,1), size(X,2)/2);
tempo_HP1 = zeros(size(X,1), size(X,2)/2);

% compute symmetric on image edges for half filter size
X_sym_col   = [fliplr(X(:   ,2:5))  , X , fliplr(X(:            ,end-4:end-1))];

for i=5:2:size(X,2)+3 %+3 pour indexer ind en fct de i
 
ind              = (i+1-4)/2; %  var for temp var index scanning
    
tempo_LP1(:,ind) =  X_sym_col(:,i-4)*LP_Filter(1) + ...
                    X_sym_col(:,i-3)*LP_Filter(2) + ...
                    X_sym_col(:,i-2)*LP_Filter(3) + ...
                    X_sym_col(:,i-1)*LP_Filter(4) + ...
                    X_sym_col(:,i  )*LP_Filter(5) + ...
                    X_sym_col(:,i+1)*LP_Filter(6) + ...
                    X_sym_col(:,i+2)*LP_Filter(7) + ...
                    X_sym_col(:,i+3)*LP_Filter(8) + ...
                    X_sym_col(:,i+4)*LP_Filter(9);
             
             
tempo_HP1(:,ind) =  X_sym_col(:,i+1-3)*HP_Filter(1) + ...
                    X_sym_col(:,i+1-2)*HP_Filter(2) + ...
                    X_sym_col(:,i+1-1)*HP_Filter(3) + ...
                    X_sym_col(:,i+1  )*HP_Filter(4) + ...
                    X_sym_col(:,i+1+1)*HP_Filter(5) + ...
                    X_sym_col(:,i+1+2)*HP_Filter(6) + ...
                    X_sym_col(:,i+1+3)*HP_Filter(7);
             
end

X1           = [tempo_LP1, tempo_HP1];

% figure('color','k')
% image(X1)
% colormap(map) 


%% lines filtering

% compute symmetric on image edges for half filter size
X_sym_ligne     = [flipud(X1(2:5 ,:  ))  ; X1 ; flipud(X1(end-4:end-1  ,:          ))];

% Initialize temp var
tempo_LP2 = zeros(size(X,1)/2, size(X,2));
tempo_HP2 = zeros(size(X,1)/2, size(X,2));

for i=5:2:size(X,1)+3 %+3 pour indexer ind en fct de i
 
ind             = (i+1-4)/2;   % var for temp var index scanning
    
tempo_LP2(ind,:) =  X_sym_ligne(i-4,:)*LP_Filter(1) + ...
                    X_sym_ligne(i-3,:)*LP_Filter(2) + ...
                    X_sym_ligne(i-2,:)*LP_Filter(3) + ...
                    X_sym_ligne(i-1,:)*LP_Filter(4) + ...
                    X_sym_ligne(i  ,:)*LP_Filter(5) + ...
                    X_sym_ligne(i+1,:)*LP_Filter(6) + ...
                    X_sym_ligne(i+2,:)*LP_Filter(7) + ...
                    X_sym_ligne(i+3,:)*LP_Filter(8) + ...
                    X_sym_ligne(i+4,:)*LP_Filter(9);
             
             
tempo_HP2(ind,:) =  X_sym_ligne(i+1-3,:)*HP_Filter(1) + ...
                    X_sym_ligne(i+1-2,:)*HP_Filter(2) + ...
                    X_sym_ligne(i+1-1,:)*HP_Filter(3) + ...
                    X_sym_ligne(i+1  ,:)*HP_Filter(4) + ...
                    X_sym_ligne(i+1+1,:)*HP_Filter(5) + ...
                    X_sym_ligne(i+1+2,:)*HP_Filter(6) + ...
                    X_sym_ligne(i+1+3,:)*HP_Filter(7);
             
end

X2           = [tempo_LP2; tempo_HP2];

% figure(3)
% image(X2)
% colormap(map) 


end

