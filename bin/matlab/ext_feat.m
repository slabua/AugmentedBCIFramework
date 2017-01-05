function [epy_min, epy_max, egy_min, egy_max, ...
          signal_entropy, signal_energy, baseline_entropy, baseline_energy, stimuli_entropy, stimuli_energy, ...
          signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma,         ...
          signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma,             ...
          baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, ...
          stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma,     ...
          baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma,     ...
          stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma] = ext_feat(fileName, channels, sampleRate, timeFlash, legacymode)

    if (nargin < 5)
        legacymode = false;
    end;
    
    inputSignal  = readBinSignal(fileName, channels)';
    %%%%%%%%%%%%%%%%% REMOVE %%%%%%%%%%%%%%%%%
    %inputSignal(end,end-1) = inputSignal(end-1,end-1);
    %%%%%%%%%%%%%%%%% REMOVE %%%%%%%%%%%%%%%%%
    targets = inputSignal(:, end-1);
    epy_min =  1;
    epy_max =  1;
    egy_min = -1;
    egy_max = -1;
    
    if (legacymode),
        [signal_entropy, signal_energy, baseline_entropy, baseline_energy, stimuli_entropy, stimuli_energy, ...
         signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma,         ...
         signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma,             ...
         baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, ...
         stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma,     ...
         baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma,     ...
         stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma] = epy_egy(inputSignal, channels, sampleRate, timeFlash, legacymode);
        
    else
        calibtags = unique(targets);
        if (calibtags ~= -1),
            isCalibration = true;
        else
            isCalibration = false;
        end;

        if (~isCalibration),
            [signal_entropy, signal_energy, baseline_entropy, baseline_energy, stimuli_entropy, stimuli_energy, ...
             signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma,         ...
             signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma,             ...
             baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, ...
             stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma,     ...
             baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma,     ...
             stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma] = epy_egy(inputSignal, channels, sampleRate, timeFlash);
            
        else
            % split the calibration signal into sub-signals,
            % one for each target analysed
            [epy_min, epy_max, egy_min, egy_max, ...
             signal_entropy, signal_energy, baseline_entropy, baseline_energy, stimuli_entropy, stimuli_energy, ...
             signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma,         ...
             signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma,             ...
             baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, ...
             stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma,     ...
             baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma,     ...
             stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma] = split_signal(inputSignal, calibtags, channels, sampleRate, timeFlash);
            
        end;
    end;
    
% utility function
function [epy_min, epy_max, egy_min, egy_max, ...
          signal_entropy, signal_energy, baseline_entropy, baseline_energy, stimuli_entropy, stimuli_energy, ...
          signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma,         ...
          signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma,             ...
          baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, ...
          stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma,     ...
          baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma,     ...
          stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma] = split_signal(inputSignal, calibtags, channels, sampleRate, timeFlash)
    
    signepy        = [];
    signegy        = [];
    baseepy        = [];
    stimepy        = [];
    baseegy        = [];
    stimegy        = [];
    for currenttag = 1 : size(calibtags, 1),
        subsignal  = inputSignal(inputSignal(:, end-1) == calibtags(currenttag), :);
        [signal_entropy, signal_energy, baseline_entropy, baseline_energy, stimuli_entropy, stimuli_energy, ...
         signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma,         ...
         signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma,             ...
         baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, ...
         stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma,     ...
         baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma,     ...
         stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma] = epy_egy(subsignal, channels, sampleRate, timeFlash);
        
        [signal_entropyC, signal_energyC,     ...
         baseline_entropyC, stimuli_entropyC, ...
         baseline_energyC, stimuli_energyC] = output_compression(signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma,                        ...
                                                                                signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma,             ...
                                                                                baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, ...
                                                                                stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma,     ...
                                                                                baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma,     ...
                                                                                stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma);
        
        signepy(currenttag, :) = signal_entropyC;
        signegy(currenttag, :) = signal_energyC;
        
        baseepy(currenttag, :) = baseline_entropyC;
        stimepy(currenttag, :) = stimuli_entropyC;
        
        baseegy(currenttag, :) = baseline_energyC;
        stimegy(currenttag, :) = stimuli_energyC;
        
        signepy_nondec(currenttag) = signal_entropy;
        signegy_nondec(currenttag) = signal_energy;
        baseepy_nondec(currenttag) = baseline_entropy;
        stimepy_nondec(currenttag) = stimuli_entropy;
        baseegy_nondec(currenttag) = baseline_energy;
        stimegy_nondec(currenttag) = stimuli_energy;
    end;
    
    epy_min_nondec = min(signepy_nondec);
    epy_max_nondec = max(signepy_nondec);
    egy_min_nondec = min(signegy_nondec);
    egy_max_nondec = max(signegy_nondec);
    
    epy_mean_nondec = mean(signepy_nondec);
    egy_mean_nondec = mean(signegy_nondec);
    
    % min and max entropy/energy of signal bands
    signepy_bands_min = min(signepy);
    signepy_bands_max = max(signepy);
    signegy_bands_min = min(signegy);
    signegy_bands_max = max(signegy);
    % min and max entropy/energy of baseline bands
    baseepy_bands_min = min(baseepy);
    baseepy_bands_max = max(baseepy);
    baseegy_bands_min = min(baseegy);
    baseegy_bands_max = max(baseegy);
    % min and max entropy/energy of stimuli bands
    stimepy_bands_min = min(stimepy);
    stimepy_bands_max = max(stimepy);
    stimegy_bands_min = min(stimegy);
    stimegy_bands_max = max(stimegy);
    
    % using signal for min/max %%%%%%%%%%%%%%%%%%%%%%%%
    epy_min = min(mean(signepy, 2));
    epy_max = max(mean(signepy, 2));
    %if (epy_max > 0), epy_max = epy_min; end;
    egy_min = min(mean(signegy, 2));
    egy_max = max(mean(signegy, 2));
    
    % using stimuli for min/max %%%%%%%%%%%%%%%%%%%%%%%%
%     epy_min_stim = min(mean(stimepy, 2));
%     epy_max_stim = max(mean(stimepy, 2));
%     egy_min_stim = min(mean(stimegy, 2));
%     egy_max_stim = max(mean(stimegy, 2));
    
    signepy = mean(signepy, 1);
    signegy = mean(signegy, 1);
    baseepy = mean(baseepy, 1);
    stimepy = mean(stimepy, 1);
    baseegy = mean(baseegy, 1);
    stimegy = mean(stimegy, 1);
    
    [signal_entropy, signal_energy, baseline_entropy, baseline_energy, stimuli_entropy, stimuli_energy, ...
     signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma,         ...
     signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma,             ...
     baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, ...
     stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma,     ...
     baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma,     ...
     stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma] = output_expansion(signepy, signegy, baseepy, stimepy, baseegy, stimegy);
    
% utility function
function [signal_entropy, signal_energy,     ...
          baseline_entropy, stimuli_entropy, ...
          baseline_energy, stimuli_energy] = output_compression(signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma,         ...
                                                                                signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma,             ...
                                                                                baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, ...
                                                                                stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma,     ...
                                                                                baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma,     ...
                                                                                stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma)
    
    signal_entropy   = [signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma, signal_entropy_theta];
    signal_energy    = [signal_energy_alpha, signal_energy_beta, signal_energy_gamma, signal_energy_theta];
    baseline_entropy = [baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, baseline_entropy_theta];
    stimuli_entropy  = [stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma, stimuli_entropy_theta];
    baseline_energy  = [baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma, baseline_energy_theta];
    stimuli_energy   = [stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma, stimuli_energy_theta];
    
% utility function
function [signal_entropy, signal_energy, baseline_entropy, baseline_energy, stimuli_entropy, stimuli_energy,                                ...
          signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma,         ...
          signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma,             ...
          baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, ...
          stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma,     ...
          baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma,     ...
          stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma] = output_expansion(signal_entropy, signal_energy, baseline_entropy, stimuli_entropy, baseline_energy, stimuli_energy)
    
    signal_entropy_alpha = signal_entropy(1);
    signal_entropy_beta  = signal_entropy(2);
    signal_entropy_theta = signal_entropy(3);
    signal_entropy_gamma = signal_entropy(4);
    signal_entropy       = mean(signal_entropy);
    
    signal_energy_alpha = signal_energy(1);
    signal_energy_beta  = signal_energy(2);
    signal_energy_theta = signal_energy(3);
    signal_energy_gamma = signal_energy(4);
    signal_energy       = mean(signal_energy);
    
    baseline_entropy_alpha = baseline_entropy(1);
    baseline_entropy_beta  = baseline_entropy(2);
    baseline_entropy_gamma = baseline_entropy(3);
    baseline_entropy_theta = baseline_entropy(4);
    baseline_entropy       = mean(baseline_entropy);
    
    stimuli_entropy_alpha = stimuli_entropy(1);
    stimuli_entropy_beta  = stimuli_entropy(2);
    stimuli_entropy_gamma = stimuli_entropy(3);
    stimuli_entropy_theta = stimuli_entropy(4);
    stimuli_entropy       = mean(stimuli_entropy);
    
    baseline_energy_alpha = baseline_energy(1);
    baseline_energy_beta  = baseline_energy(2);
    baseline_energy_gamma = baseline_energy(3);
    baseline_energy_theta = baseline_energy(4);
    baseline_energy       = mean(baseline_energy);
    
    stimuli_energy_alpha = stimuli_energy(1);
    stimuli_energy_beta  = stimuli_energy(2);
    stimuli_energy_gamma = stimuli_energy(3);
    stimuli_energy_theta = stimuli_energy(4);
    stimuli_energy       = mean(stimuli_energy);

