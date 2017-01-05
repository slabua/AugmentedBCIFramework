% Extraction of signal features
function [signal_entropy, signal_energy, baseline_entropy, baseline_energy, stimuli_entropy, stimuli_energy, ...
    signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma,         ...
    signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma,             ...
    baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, ...
    stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma,     ...
    baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma,     ...
    stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma] = epy_egy(signal, channels, sampleRate, timeFlash, legacymode)

    % stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma] = epy_egy(fileName, rows, flashingMode, flash, channels, sampleRate, timeFlash, legacymode)
    
    newmethod = false;
    
    if (nargin < 5)
        legacymode = false;
    end;
    
    %[signal, signal_baseline, signal_stimuli] = base_stim(fileName, rows, flashingMode, flash, channels, sampleRate, timeFlash, legacymode);
    [signal, signal_baseline, signal_stimuli] = base_stim(signal, channels, sampleRate, timeFlash, legacymode);
    
    % stimuli = signal(:, end);
    signal  = signal(:, 1:channels);
    
    signal_baseline = signal_baseline(:, 1:channels);
    signal_stimuli  = signal_stimuli(:, 1:channels);
    
    % High-pass filter
    wn = 4/(sampleRate/2); % 4Hz in rad/s
    [b, a] = butter(4, wn, 'high');
    signal          = filtfilt(b, a, signal);
    signal_baseline = filtfilt(b, a, signal_baseline);
    signal_stimuli  = filtfilt(b, a, signal_stimuli);
    
    % Low-pass filter
    wn = 45/(sampleRate/2); % 45Hz in rad/s
    [b, a] = butter(4, wn, 'low');
    signal          = filtfilt(b, a, signal);
    signal_baseline = filtfilt(b, a, signal_baseline);
    signal_stimuli  = filtfilt(b, a, signal_stimuli);
    
    N_signal          = size(signal, 1);
    N_signal_baseline = size(signal_baseline, 1);
    N_signal_stimuli  = size(signal_stimuli, 1);
    
    % Notch filter
    signal          = notchFilter(signal, sampleRate, 50);
    signal_baseline = notchFilter(signal_baseline, sampleRate, 50);
    signal_stimuli  = notchFilter(signal_stimuli, sampleRate, 50);
    
    [signal_energy_alpha, signal_energy_beta, signal_energy_gamma, signal_energy_theta, ...
        signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma, signal_entropy_theta] = waveletDecomposition(signal);
    
    [baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma, baseline_energy_theta, ...
        baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, baseline_entropy_theta] = waveletDecomposition(signal_baseline);
    
    [stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma, stimuli_energy_theta, ...
        stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma, stimuli_entropy_theta] = waveletDecomposition(signal_stimuli);
    
    %IN ORDINE SEGNALE COMPLETO BASELINE STIMULI
    entropy_alpha = [signal_entropy_alpha, baseline_entropy_alpha, stimuli_entropy_alpha];
    entropy_beta  = [signal_entropy_beta,  baseline_entropy_beta,  stimuli_entropy_beta];
    entropy_gamma = [signal_entropy_gamma, baseline_entropy_gamma, stimuli_entropy_gamma];
    entropy_theta = [signal_entropy_theta, baseline_entropy_theta, stimuli_entropy_theta];
    
    energy_alpha  = [signal_energy_alpha, baseline_energy_alpha, stimuli_energy_alpha];
    energy_beta   = [signal_energy_beta,  baseline_energy_beta,  stimuli_energy_beta];
    energy_gamma  = [signal_energy_gamma, baseline_energy_gamma, stimuli_energy_gamma];
    energy_theta  = [signal_energy_theta, baseline_energy_theta, stimuli_energy_theta];
    
    %%DA SISTEMARE VERIFICARE FORMULE
    %[signal_energy, signal_entropy] = waveletDecomposition(signal);
    signal_energy    = sum(mean(signal ./ (sampleRate/channels), 2) .^ 2);
    %signal_energy    = std(mean(signal, 2))^2;
    signal_entropy   = wentropy(mean(signal, 2), 'shannon');
    
    if (newmethod),
        baseline_entropy = entropy(mean(signal_baseline, 2)); %%MODIFICATO IN ENTROPIA 
        stimuli_entropy  = entropy(mean(signal_stimuli, 2));
        baseline_energy  = sum(mean(signal_baseline ./ (sampleRate/channels), 2) .^ 2);
        stimuli_energy   = sum(mean(signal_stimuli  ./ (sampleRate/channels), 2) .^ 2);
    else,
        baseline_entropy = wentropy(mean(signal_baseline, 2), 'shannon');
        %baseline_entropy = baseline_entropy / log(N_signal_baseline);
        stimuli_entropy  = wentropy(mean(signal_stimuli, 2),  'shannon');
        baseline_energy  = sum(mean(signal_baseline ./ (sampleRate/channels), 2) .^ 2);
        stimuli_energy   = sum(mean(signal_stimuli  ./ (sampleRate/channels), 2) .^ 2);
    end;
    
end