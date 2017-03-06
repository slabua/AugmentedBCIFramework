function [energy_alpha,  energy_beta,  energy_gamma,  energy_teta, ...
          entropy_alpha, entropy_beta, entropy_gamma, entropy_teta] = waveletDecomposition(signal)
    
    newmethod = true;
    
    % Wavelet Decomposition
    waveletFunction = 'db4';
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
    
    [Ea, Ed] = wenergy(C, L);
    
    if (newmethod),
        % Energy
        energy_teta  = abs(sum(Ed(5).^2)); %%valore assoluto
        energy_alpha = abs(sum(Ed(4).^2));
        energy_beta  = abs(sum(Ed(3).^2));
        energy_gamma = abs(sum(Ed(2).^2));
        % Entropy
        % entropy_gamma = entropy(D2);
        % entropy_beta  = entropy(D3);
        % entropy_alpha = entropy(D4);
        % entropy_teta  = entropy(D5);
        entropy_gamma = EntropyS(D2);
        entropy_beta  = EntropyS(D3);
        entropy_alpha = EntropyS(D4);
        entropy_teta  = EntropyS(D5);
        
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
    
    
function entropy = EntropyS(signal)

    dim       = size(signal, 1);
    log_dim   = log(dim);

    h         = hist(signal)';
    hist_size = size(h, 1);

    for i = 1 : hist_size,
        p(i) = h(i) / dim;  
        H(i) = - p(i) * log(p(i)) / log_dim;
    end;

    entropy = sum(H, 2);

