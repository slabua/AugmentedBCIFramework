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