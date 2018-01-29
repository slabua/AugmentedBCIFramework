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

function [rsq, rsq_min, rsq_max] = ext_rsq(fileName, rows, flashingMode, tagPredicted, channels, sampleRate, timeFlash, legacymode)

    if (nargin < 8)
        legacymode = false;
    end;
    
    inputSignal  = readBinSignal(fileName, channels)';
    targets = inputSignal(:, end-1);
    rsq_min = -1;
    rsq_max = -1;
    
    if (legacymode),
        rsq = r_square(inputSignal, rows, flashingMode, tagPredicted, channels, sampleRate, timeFlash);
    else
        calibtags = unique(targets);
        if (calibtags ~= -1),
            isCalibration = true;
        else
            isCalibration = false;
        end;

        if (~isCalibration),
            rsq = r_square(inputSignal, rows, flashingMode, tagPredicted, channels, sampleRate, timeFlash);
            
        else
            % split the calibration signal into sub-signals, one for each target analysed
            calib_rsq = split_signal(inputSignal, calibtags, rows, flashingMode, channels, sampleRate, timeFlash);
            rsq       = mean(calib_rsq);
            rsq_min   = min(calib_rsq);
            rsq_max   = max(calib_rsq);
        end;
    end;
    
% utility function
function calib_rsq = split_signal(inputSignal, calibtags, rows, flashingMode, channels, sampleRate, timeFlash)
    for currenttag = 1 : size(calibtags, 1),
        subsignal  = inputSignal(inputSignal(:, end-1) == calibtags(currenttag), :);
        calib_rsq(currenttag) = r_square(subsignal, rows, flashingMode, calibtags(currenttag), channels, sampleRate, timeFlash);
    end;
    