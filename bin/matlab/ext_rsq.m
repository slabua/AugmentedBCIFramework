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
    