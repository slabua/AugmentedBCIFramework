% Error using filtfilt>getCoeffsAndInitialConditions (line 181)
% Data length must be larger than 12 samples.
% 
% Error in filtfilt (line 96)
% [b,a,zi,nfact,L] = getCoeffsAndInitialConditions(b,a,Npts);
% 
% Error in epy_egy (line 28)
%     signal          = filtfilt(b, a, signal);
% 
% Error in ext_feat>split_signal (line 115)
%         [~, ~,               ...
% 
% Error in ext_feat (line 87)
%             [signal_entropy, signal_energy,               ...
%rows            = 4;

fileName = '../BinFiles/Calib_testcalibsignalidsRC8f3t_4x4_RC_2016_07_22_12_38_58.bin';
legacymode      = false;
normalise       = false;
flashingMode    = 1; % rows-cols
flash           = 8;
channels        = 4;
sampleRate      = 256;
timeFlash       = 125;
[signal_entropy, signal_energy,         ...
signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma,         ...
signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma,             ...
baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, ...
stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma,     ...
baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma,     ...
stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma] = ext_feat(fileName, channels, sampleRate, timeFlash, legacymode)
