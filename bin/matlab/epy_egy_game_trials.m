%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% 
%% UniPA BCI Framework: an Augmented BCI Framework
%% 
%% Copyright (C) 2016-2017 Salvatore La Bua (slabua@gmail.com)
%% RoboticsLab, Department of Industrial and Digital Innovation (DIID),
%% Universita' degli Studi di Palermo, V. delle Scienze, Palermo, Italy.
%% 
%% http://www.slblabs.com/projects/unipa-bci-framework
%% https://github.com/slabua/UniPABCIFramework
%% 
%% 
%% This program is free software: you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published by
%% the Free Software Foundation, either version 3 of the License, or
%% (at your option) any later version.
%% 
%% This program is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty of
%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%% GNU General Public License for more details.
%% 
%% You should have received a copy of the GNU General Public License
%% along with this program.  If not, see <http://www.gnu.org/licenses/>.
%% 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Extraction of signal features
% function [fair_entropy_theta,  fair_entropy_alpha,  fair_entropy_beta,  fair_entropy_gamma,  ...
%           cheat_entropy_theta, cheat_entropy_alpha, cheat_entropy_beta, cheat_entropy_gamma, ...
%           fair_energy_theta,   fair_energy_alpha,   fair_energy_beta,   fair_energy_gamma,   ...
%           cheat_energy_theta,  cheat_energy_alpha,  cheat_energy_beta,  cheat_energy_gamma] = epy_egy_game(userBaselFile, userGameSFile, sampleRate)

function [mental_wl, focusPC, entropy_allbands, energy_allbands,  ...
          entropy_theta, entropy_alpha, entropy_beta, entropy_gamma, ...
          energy_theta,  energy_alpha,  energy_beta,  energy_gamma] = epy_egy_game_trials(baseline, fairTrials1, cheatTrials, fairTrials2, fairTrials, cheatTrialsReal, sampleRate)
    
    showPlots = false;
    
    channels = size(baseline, 2) - 3;
    
    if (showPlots),
        figure;
        subplot(3, 1, 1);
        plot(baseline);
        subplot(3, 1, 2);
        plot(fairTrials);
        subplot(3, 1, 3);
        plot(cheatTrials);
    end;
    
    fairTrials1 = fairTrials1(1:end-1, :);
    
%     nTrialF1    = sum(fairTrials1(:,end-2) == 1) + 1;
%     nTrialC     = sum(cheatTrials(:,end-2) == 1) + 1;
%     nTrialF2    = sum(fairTrials2(:,end-2) == 1) + 1;
    
    nextTrialF1 = find(fairTrials1(:, end-2) == 1);
    nextTrialF1 = [1; nextTrialF1; size(fairTrials1, 1)];
    nextTrialF2 = find(fairTrials2(:, end-2) == 1);
    nextTrialF2 = [1; nextTrialF2; size(fairTrials2, 1)];
    %nextTrialF  = find(fairTrials(:, end-2) == 1);
    %nextTrialF = [1; nextTrialF; size(fairTrials, 1)];
    nextTrialC  = find(cheatTrials(:, end-2) == 1);
    nextTrialC = [1; nextTrialC; size(cheatTrials, 1)];
    nextTrialCReal  = find(cheatTrialsReal(:, end-2) == 1);
    nextTrialCReal = [1; nextTrialCReal; size(cheatTrialsReal, 1)];
    
    %% Single trial analysis
    % SLB TODO MAKE THESE VARIABLE AVAILABLE AS OUTPUT
    [trialF1_mentalwl, trialF1_focusPC, trialF1_entropy, trialF1_energy, ...
     trialF1_entropy_alpha, trialF1_entropy_beta, trialF1_entropy_gamma, trialF1_entropy_theta, ...
     trialF1_energy_alpha,  trialF1_energy_beta,  trialF1_energy_gamma,  trialF1_energy_theta] = run_trial(nextTrialF1, fairTrials1, sampleRate);
    
    [trialC_mentalwl, trialC_focusPC, trialC_entropy, trialC_energy, ...
     trialC_entropy_alpha, trialC_entropy_beta, trialC_entropy_gamma, trialC_entropy_theta, ...
     trialC_energy_alpha,  trialC_energy_beta,  trialC_energy_gamma,  trialC_energy_theta] = run_trial(nextTrialC, cheatTrials, sampleRate);
 
    [trialCReal_mentalwl, trialCReal_focusPC, trialCReal_entropy, trialCReal_energy, ...
     trialCReal_entropy_alpha, trialCReal_entropy_beta, trialCReal_entropy_gamma, trialCReal_entropy_theta, ...
     trialCReal_energy_alpha,  trialCReal_energy_beta,  trialCReal_energy_gamma,  trialCReal_energy_theta] = run_trial(nextTrialCReal, cheatTrialsReal, sampleRate);
    
    [trialF2_mentalwl, trialF2_focusPC, trialF2_entropy, trialF2_energy, ...
     trialF2_entropy_alpha, trialF2_entropy_beta, trialF2_entropy_gamma, trialF2_entropy_theta, ...
     trialF2_energy_alpha,  trialF2_energy_beta,  trialF2_energy_gamma,  trialF2_energy_theta] = run_trial(nextTrialF2, fairTrials2, sampleRate);
        
    %% Category analysis (baseline, fair, cheat)
    [baseline_mentalwl, baseline_focusPC, baseline_entropy, baseline_energy, ...
     baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, baseline_entropy_theta, ...
     baseline_energy_alpha,  baseline_energy_beta,  baseline_energy_gamma,  baseline_energy_theta] = extract_features(baseline, sampleRate);
    
    [fair_mentalwl, fair_focusPC, fair_entropy, fair_energy, ...
     fair_entropy_alpha, fair_entropy_beta, fair_entropy_gamma, fair_entropy_theta, ...
     fair_energy_alpha,  fair_energy_beta,  fair_energy_gamma,  fair_energy_theta] = extract_features(fairTrials, sampleRate);
    
    [cheat_mentalwl, cheat_focusPC, cheat_entropy, cheat_energy, ...
     cheat_entropy_alpha, cheat_entropy_beta, cheat_entropy_gamma, cheat_entropy_theta, ...
     cheat_energy_alpha,  cheat_energy_beta,  cheat_energy_gamma,  cheat_energy_theta] = extract_features(cheatTrialsReal, sampleRate);
    
    %% Features grouping
    entropy_alpha    = [baseline_entropy_alpha, fair_entropy_alpha, cheat_entropy_alpha];
    entropy_beta     = [baseline_entropy_beta,  fair_entropy_beta,  cheat_entropy_beta];
    entropy_gamma    = [baseline_entropy_gamma, fair_entropy_gamma, cheat_entropy_gamma];
    entropy_theta    = [baseline_entropy_theta, fair_entropy_theta, cheat_entropy_theta];
    
    energy_alpha     = [baseline_energy_alpha, fair_energy_alpha, cheat_energy_alpha];
    energy_beta      = [baseline_energy_beta,  fair_energy_beta,  cheat_energy_beta];
    energy_gamma     = [baseline_energy_gamma, fair_energy_gamma, cheat_energy_gamma];
    energy_theta     = [baseline_energy_theta, fair_energy_theta, cheat_energy_theta];
    
    entropy_allbands = [baseline_entropy, fair_entropy, cheat_entropy];
    energy_allbands  = [baseline_energy,  fair_energy,  cheat_energy];
    
    mental_wl        = [baseline_mentalwl, fair_mentalwl, cheat_mentalwl];
    
    focusPC          = [baseline_focusPC, fair_focusPC, cheat_focusPC];
    
    
% Utility function
function [mental_wl, focusPC, entropy_allbands, energy_allbands, ...
          entropy_alpha, entropy_beta, entropy_gamma, entropy_theta, ...
          energy_alpha,  energy_beta,  energy_gamma, energy_theta] = extract_features(signal, sampleRate)
    
    newmethod = true;
    
    channels    = size(signal, 2) - 3;
    signalFocus = signal;
    signal      = signal(:, 1:channels);
    
    %%% SLB TODO REMOVE
    zoneF  = [1, 2, 3, 9, 10];
    zoneT  = [4, 5, 6, 11, 12, 13];
    zoneP  = [7, 8, 14, 15, 16];
    signal = signal(:, zoneF);
    %%%
    
    % High-pass filter
    wn = 4/(sampleRate/2); % 4Hz in rad/s
    [b, a] = butter(4, wn, 'high');
    signal = filtfilt(b, a, signal);
    
    % Low-pass filter
    wn = 45/(sampleRate/2); % 45Hz in rad/s
    [b, a] = butter(4, wn, 'low');
    signal = filtfilt(b, a, signal);
    
    % Notch filter
    signal = notchFilter(signal, sampleRate, 50);
    
    [energy_alpha,  energy_beta,  energy_gamma,  energy_theta, ...
     entropy_alpha, entropy_beta, entropy_gamma, entropy_theta] = waveletDecomposition(signal);
    
    if (newmethod),
        % entropy_allbands = entropy(mean(signal, 2));
        entropy_allbands = EntropyS(mean(signal, 2));
    else,
        entropy_allbands = wentropy(mean(signal, 2), 'shannon');
    end;
    energy_allbands = sum(mean(signal ./ (sampleRate/channels), 2) .^ 2);
    
    mental_wl = mental_workload(energy_alpha, energy_beta, energy_theta);
    
    focusPC = extract_focus(signalFocus);
    
    
% Utility function
function [mental_wl, focusPC, trial_entropy, trial_energy, ...
          trial_entropy_alpha, trial_entropy_beta, trial_entropy_gamma, trial_entropy_theta, ...
          trial_energy_alpha,  trial_energy_beta,  trial_energy_gamma,  trial_energy_theta] = run_trial(indices, signal, sampleRate)
    
    for trial = 1 : size(indices)-1,
        
        trial
        
        trialSignal = signal(indices(trial):indices(trial+1), :);
        %figure, plot(trialSignal);
        [mental_wl(trial), focusPC(trial), trial_entropy(trial), trial_energy(trial), ...
         trial_entropy_alpha(trial), trial_entropy_beta(trial), trial_entropy_gamma(trial), trial_entropy_theta(trial), ...
         trial_energy_alpha(trial),  trial_energy_beta(trial),  trial_energy_gamma(trial),  trial_energy_theta(trial)] = extract_features(trialSignal, sampleRate);
    end;
    
    
% Utility function
%SLB TODO CHECK
function [zone_entropy, zone_energy, ...
          zone_entropy_alpha, zone_entropy_beta, zone_entropy_gamma, zone_entropy_theta, ...
          zone_energy_alpha,  zone_energy_beta,  zone_energy_gamma,  zone_energy_theta] = run_zones(zone, signal, sampleRate)
    
    zoneF = [1, 2, 3, 9, 10];
    zoneT = [4, 5, 6, 11, 12, 13];
    zoneP = [7, 8, 14, 15, 16];
    
    signalF = signal(:, zoneF);
    signalT = signal(:, zoneT);
    signalP = signal(:, zoneP);
    
    [zoneF_mentalwl, zoneF_focusPC, zoneF_entropy, zoneF_energy, ...
     zoneF_entropy_alpha, zoneF_entropy_beta, zoneF_entropy_gamma, zoneF_entropy_theta, ...
     zoneF_energy_alpha,  zoneF_energy_beta,  zoneF_energy_gamma,  zoneF_energy_theta] = extract_features(signalF, sampleRate);
    
    [zoneT_mentalwl, zoneT_focusPC, zoneT_entropy, zoneT_energy, ...
     zoneT_entropy_alpha, zoneT_entropy_beta, zoneT_entropy_gamma, zoneT_entropy_theta, ...
     zoneT_energy_alpha,  zoneT_energy_beta,  zoneT_energy_gamma,  zoneT_energy_theta] = extract_features(signalT, sampleRate);
    
    [zoneP_mentalwl, zoneP_focusPC, zoneP_entropy, zoneP_energy, ...
     zoneP_entropy_alpha, zoneP_entropy_beta, zoneP_entropy_gamma, zoneP_entropy_theta, ...
     zoneP_energy_alpha,  zoneP_energy_beta,  zoneP_energy_gamma,  zoneP_energy_theta] = extract_features(signalP, sampleRate);
    
    
 % Utility function
 function mwl = mental_workload(alpha, beta, theta)
     mwl = beta ./ (alpha + theta);
     
     
% Utility function
function focusPC = extract_focus(trialSignal)
    focusPC = sum(trialSignal(:, end) == 1) / size(trialSignal, 1) * 100;
    
    
% Utility function
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

    