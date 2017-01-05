function [signal, signal_baseline, signal_stimuli] = base_stim(signal, channels, sampleRate, timeFlash, legacymode)

% function [signal, signal_baseline, signal_stimuli] = base_stim(fileName, rows, flashingMode, flash, channels, sampleRate, timeFlash, legacymode)
%     signal = readBinSignal(fileName, channels)';
    
    
    if (nargin < 5),
        legacymode = false;
    end;
    
    if (legacymode),
        stimseqlen = ceil(sampleRate * timeFlash / 1000);
        
        singlestim = [find(signal(:, channels+2) ~= 0), signal(signal(:, channels+2) ~= 0, channels+2)];
        for i = 1 : size(singlestim, 1),
            for j = 0 : stimseqlen-1,
                signal(singlestim(i, 1) + j, channels+2) = singlestim(i, 2);
            end,
        end;
    end;
    
    signal_baseline = signal(signal(:, channels+2) == 0, :);
    signal_stimuli  = signal(signal(:, channels+2) ~= 0, :);
end