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

%function rsq = r_square(fileName, rows, flashingMode, tagPredicted, channels, sampleRate, timeFlash)
function rsq = r_square(signal, rows, flashingMode, tagPredicted, channels, sampleRate, timeFlash)
    
    %signal  = readBinSignal(fileName, channels)';
    stimuli = signal(:, end);
    signal  = signal(:, 1:channels);
    dataSegLength = timeFlash;
    
    % memo
    % cond1="'(states.StimulusCode > 0) & (states.StimulusType == 1)'";
    % cond2="'(states.StimulusCode > 0) & (states.StimulusType == 0)'";
    
    if (flashingMode),
        id = mappingIDtoRC(tagPredicted, rows);
        targetConditions1 = sprintf('(stimuli ~= 0 & (stimuli == id(1) | stimuli == id(2)))');
        targetConditions2 = sprintf('(stimuli ~= 0 & (stimuli ~= id(1) & stimuli ~= id(2)))');
        cond1 = (stimuli ~= 0 & (stimuli == id(1) | stimuli == id(2)));
        cond2 = (stimuli ~= 0 & (stimuli ~= id(1) & stimuli ~= id(2)));
        condition1idxstr = sprintf(...
            'condition1idx = find( (trialnr == cur_trial) & (stimuli ~= 0 & (stimuli == id(1) | stimuli == id(2))), 1 );');
        condition2idxstr = sprintf(...
            'condition2idx = find( (trialnr == cur_trial) & (stimuli ~= 0 & (stimuli ~= id(1) & stimuli ~= id(2))), 1 );');
    else
        targetConditions1 = sprintf('(stimuli ~= 0 & stimuli == tagPredicted)');
        targetConditions2 = sprintf('(stimuli ~= 0 & stimuli ~= tagPredicted)');
        cond1 = (stimuli ~= 0 & stimuli == tagPredicted);
        cond2 = (stimuli ~= 0 & stimuli ~= tagPredicted);
        condition1idxstr = sprintf(...
            'condition1idx = find((trialnr == cur_trial) & (stimuli ~= 0 & stimuli == tagPredicted), 1);');
        condition2idxstr = sprintf(...
            'condition2idx = find((trialnr == cur_trial) & (stimuli ~= 0 & stimuli ~= tagPredicted), 1);');
    end;
    
    targetConditions = [targetConditions1; targetConditions2];
    
    
    targetIdxs1 = find(cond1);
    targetIdxs2 = find(cond2);
    targetIdx = [targetIdxs1; targetIdxs2];
    
    triallength = ceil(dataSegLength * sampleRate / 1000); % convert from ms into samples
    % timeBinsMs = (.5 : triallength - .5) / sampleRate * 1000;
    
    trialEnds = [0; find(diff(targetIdx) ~= 1); length(targetIdx)];
    trialLengths = diff(trialEnds);
    
    goodTrials = find(trialLengths ./ mean(trialLengths) >= 0.7);
    % badTrials = length(trialLengths) - length(goodTrials);
    
    trialnr = zeros(size(signal, 1), 1);
    
    for i = 1 : length(goodTrials)
        trialnr(targetIdx(trialEnds(goodTrials(i))+1) : targetIdx(trialEnds(goodTrials(i)+1))) = i;
    end
    
    trials = unique(trialnr);
    trials = trials(trials > 0);
    lastNum = trialnr(1);
    
    bInc1 = 0;
    bInc2 = 0;
    cond1Cnt = 0;
    cond2Cnt = 0;
    
    for idxSamp = 1 : length(trialnr)
        trialNum = trialnr(idxSamp);
        
        if (trialNum ~= lastNum || idxSamp == length(trialnr))
            if bInc1
              cond1Cnt = cond1Cnt + 1;
              bInc1 = 0;
            end
            if bInc2
              cond2Cnt = cond2Cnt + 1;
              bInc2 = 0;
            end
            lastNum = trialNum;
        end
        
        if (~bInc1 && cond1(idxSamp))
            bInc1 = 1;
        end
        
        if (length(targetConditions) == 2)
            if (~bInc2 && cond2(idxSamp))
                bInc2 = 1;
            end
        end
        
    end
    
    avgdata1 = zeros(triallength, channels, cond1Cnt);
    avgdata2 = zeros(triallength, channels, cond2Cnt);
    
    cond1Cnt = 1;
    cond2Cnt = 1;
    
    
    
    for cur_trial = min(trials) : max(trials)
        % condition 1
        eval(condition1idxstr);
        if ((isempty(condition1idx) == 0) && (length(signal) >= condition1idx + triallength))
            
            if ((size(avgdata1, 3) < cond1Cnt))
                avgdata1(:, :, end+1 : end+1000) = zeros(triallength, channels, 1000);
            end
            avgdata1(:, :, cond1Cnt) = signal(condition1idx : condition1idx+triallength-1, :);
            cond1Cnt = cond1Cnt + 1;
        end
        
        if (size(targetConditions, 1) == 2)
            % condition 2
            eval(condition2idxstr);
            if ((isempty(condition2idx) == 0) && (length(signal) >= condition2idx + triallength))

                if (size(avgdata2, 3) < cond2Cnt)
                    avgdata2(:,:, end+1 : end+1000) = zeros(triallength, channels, 1000);
                end
                avgdata2(:, :, cond2Cnt) = signal(condition2idx : condition2idx+triallength-1, :);
                cond2Cnt = cond2Cnt + 1;
            end
        end
    end
    
    if (size(avgdata1, 3) >= cond1Cnt)
        avgdata1(:, :, cond1Cnt : end) = [];
    end
    
    if (size(targetConditions, 1) == 2)
        if (size(avgdata2, 3) >= cond2Cnt)
            avgdata2(:, :, cond2Cnt:end) = [];
        end
    end
    
    ressq = calc_rsqu(double(avgdata1), double(avgdata2), 1);
    rsq = max(max(ressq));
    
% utility function
function IDs = mappingIDtoRC(tag, rows)
    IDs = zeros(2, 1);
    IDs(1) = ceil(tag / rows);
    IDs(2) = mod(tag, rows) + rows;
    
    