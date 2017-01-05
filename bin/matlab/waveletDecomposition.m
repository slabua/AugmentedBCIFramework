function [ energy_alpha,  energy_beta,  energy_gamma,  energy_teta, ...
          entropy_alpha, entropy_beta, entropy_gamma, entropy_teta] = waveletDecomposition(signal)
    
    newmethod = false;
    
    % Wavelet Decomposition
    waveletFunction = 'db4';
%     [C, L] = wavedec(signal, 5, waveletFunction);
%     
%     % Calculation the Coefficient Vectors of every Band : 
%     cD1 = detcoef(C, L, 1);                   %NOISY 128 - 64
%     cD2 = detcoef(C, L, 2);                   %Gamma 64  - 32
%     cD3 = detcoef(C, L, 3);                   %Beta  32  - 16
%     cD4 = detcoef(C, L, 4);                   %Alpha 16  - 8
%     cD5 = detcoef(C, L, 5);                   %Theta 8   - 4
%     cA5 = appcoef(C, L, waveletFunction, 5);  %Delta 4   - 1
%     
%     % Calculation the Details Vectors of every Band : 
%     D1 = wrcoef('d', C, L, waveletFunction, 1); %NOISY 128 - 64
%     D2 = wrcoef('d', C, L, waveletFunction, 2); %Gamma 64  - 32
%     D3 = wrcoef('d', C, L, waveletFunction, 3); %Beta  32  - 16
%     D4 = wrcoef('d', C, L, waveletFunction, 4); %Alpha 16  - 8
%     D5 = wrcoef('d', C, L, waveletFunction, 5); %Theta 8   - 4
%     A5 = wrcoef('a', C, L, waveletFunction, 5); %Delta 4   - 1
    [C, L] = wavedec(signal, 5, waveletFunction);

    %Calculation The Coificients Vectors of every Band 
    cD1 = detcoef(C, L, 1); %NOISY 
    cD2 = detcoef(C, L, 2); %Gamma 
    cD3 = detcoef(C, L, 3); %Beta 
    cD4 = detcoef(C, L, 4); %Alpha 
    cD5 = detcoef(C, L, 5); %Theta 
    cA5 = appcoef(C, L, waveletFunction, 5); %Delta

    %Calculation the Details Vectors of every Band : 
    D1 = wrcoef('d', C, L, waveletFunction, 1); %NOISY 
    D2 = wrcoef('d', C, L, waveletFunction, 2); %Gamma 
    D3 = wrcoef('d', C, L, waveletFunction, 3); %Beta 
    D4 = wrcoef('d', C, L, waveletFunction, 4); %Alpha 
    D5 = wrcoef('d', C, L, waveletFunction, 5); %Theta 
    A5 = wrcoef('a', C, L, waveletFunction, 5); %Delta
    
%     figure;
%     Gamma = D2; 
%     subplot(5, 1, 1); plot(1 : 1 : length(Gamma), Gamma); title('GAMMA');
%     
%     Beta = D3; 
%     subplot(5, 1, 2); plot(1 : 1 : length(Beta),  Beta);  title('BETA');
%     
%     Alpha = D4; 
%     subplot(5, 1, 3); plot(1 : 1 : length(Alpha), Alpha); title('ALPHA'); 
%     
%     Theta = D5; 
%     subplot(5, 1, 4); plot(1 : 1 : length(Theta), Theta); title('THETA');
%     
%     Delta = A5; 
%     %figure, plot(0 : 1/fs : 1, Delta);
%     subplot(5, 1, 5); plot(1 : 1 : length(Delta), Delta); title('DELTA');
    
    [Ea, Ed] = wenergy(C, L);
    
    if (newmethod),
        % Energy
        energy_teta  = abs(sum(Ed(5).^2)); %%valore assoluto
        energy_alpha = abs(sum(Ed(4).^2));
        energy_beta  = abs(sum(Ed(3).^2));
        energy_gamma = abs(sum(Ed(2).^2));
        % Entropy
        entropy_gamma = entropy(D2); %entropia ?!?
        entropy_beta  = entropy(D3);
        entropy_alpha = entropy(D4);
        entropy_teta  = entropy(D5);
        
    else,
        % Energy
        energy_teta  = Ed(5); %%percentuale
        energy_alpha = Ed(4);
        energy_beta  = Ed(3);
        energy_gamma = Ed(2);
        % Entropy
        entropy_gamma = wentropy(D2, 'shannon'); %entropy shannon
        entropy_beta  = wentropy(D3, 'shannon');
        entropy_alpha = wentropy(D4, 'shannon');
        entropy_teta  = wentropy(D5, 'shannon');
        
    end;
    
    
% waveletFunction = 'db8';
%                 [C,L] = wavedec(signal,8,waveletFunction);
%        
%                 cD1 = detcoef(C,L,1); 
%                 cD2 = detcoef(C,L,2); 
%                 cD3 = detcoef(C,L,3); 
%                 cD4 = detcoef(C,L,4); 
%                 cD5 = detcoef(C,L,5); %GAMA
%                 cD6 = detcoef(C,L,6); %BETA
%                 cD7 = detcoef(C,L,7); %ALPHA
%                 cD8 = detcoef(C,L,8); %THETA
%                 cA8 = appcoef(C,L,waveletFunction,8); %DELTA 
%                 D1 = wrcoef('d',C,L,waveletFunction,1); 
%                 D2 = wrcoef('d',C,L,waveletFunction,2); 
%                 D3 = wrcoef('d',C,L,waveletFunction,3); 
%                 D4 = wrcoef('d',C,L,waveletFunction,4); 
%                 D5 = wrcoef('d',C,L,waveletFunction,5); %GAMMA
%                 D6 = wrcoef('d',C,L,waveletFunction,6); %BETA
%                 D7 = wrcoef('d',C,L,waveletFunction,7); %ALPHA
%                 D8 = wrcoef('d',C,L,waveletFunction,8); %THETA
%                 A8 = wrcoef('a',C,L,waveletFunction,8); %DELTA
%                
%                 %Gamma = D5; 
%                 %figure; subplot(5,1,1); plot(1:1:length(Gamma),Gamma);title('GAMMA');
%                
%                 %Beta = D6; 
%                 %subplot(5,1,2); plot(1:1:length(Beta), Beta); title('BETA');
%                 
%                 
%                 %Alpha = D7; 
%                 %subplot(5,1,3); plot(1:1:length(Alpha),Alpha); title('ALPHA'); 
%                 
%                 
%                 %Theta = D8; 
%                 %subplot(5,1,4); plot(1:1:length(Theta),Theta);title('THETA');
%                 
%                 
%                 %Delta = A8; 
%                 %figure, plot(0:1/fs:1,Delta); 
%                 %subplot(5,1,5);plot(1:1:length(Delta),Delta);title('DELTA');
% 
%                 
%                 [Ea, Ed] = wenergy(C, L);
%                 
%                 energy_teta  = Ed(8)
%                 energy_alpha = Ed(7)
%                 energy_beta  = Ed(6)
%                 energy_gamma = Ed(5)
%                 
%                 % Calculating wentropy
%                 entropy_gamma = entropy(D5);
%                 entropy_beta  = entropy(D6);
%                 entropy_alpha = entropy(D7);
%                 entropy_teta  = entropy(D8);
   
    
end
