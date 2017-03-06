% offline main
function [signal_entropy,   signal_energy,   ...
          baseline_entropy, stimuli_entropy, ...
          baseline_energy,  stimuli_energy,  ...
          rsq] = run()
    %% VARIABLES
    % enable this variable to preprocess the old bin format
    legacymode      = true;
    normalise       = false;
    
    rows            = 4;
    flashingMode    = 0; % single square
    % flashingMode    = 1; % rows-cols
    flash           = 8;
    channels        = 4;
    sampleRate      = 256;
    timeFlash       = 125;
    % calibrationtags = 3;
    % windowlen       = 200;
    path            = '../BinFiles/testpreliminary/';
    
    %% COLOURS
    bands           = {    'alpha',     'beta',    'gamma',    'theta'};
    % colours         = {      'red',    'green',     'blue',    'black'};
    coloursRGB1     = [ .9, .7, .7; .7, .9, .7; .7, .7, .9; .2, .2, .2];
    coloursRGB2     = [ .9, .1, .1; .1, .9, .1; .1, .1, .9; .1, .1, .1];
    
    %% CLASSIFICATION RESULTS
    tagPredicted1 = [ 7  8 11  6  5 ; % classified
                      7  8 11  6 12]; % subject-chosen
    tagPredicted2 = [14 10  3 16  1 ;
                     14 10  3 12  1];
    tagPredicted3 = [11 12 13  5  8 ;
                     11  5 13 10  8];
    tagPredicted4 = [ 7  6 16  9  3 ;
                      7  9 16 10  3];
    tagPredicted5 = [ 4  8  6 15 14 ;
                      4 14  6 11  9];
    tagPredicted6 = [10 15 10  4  7 ;
                      6 15 10  4  7];
    
    tagP = []; % [classified; subject-chosen]
    tagP(:, :, 1) = tagPredicted1;
    tagP(:, :, 2) = tagPredicted2;
    tagP(:, :, 3) = tagPredicted3;
    tagP(:, :, 4) = tagPredicted4;
    tagP(:, :, 5) = tagPredicted5;
    tagP(:, :, 6) = tagPredicted6;
    
    tagPgap = tagP;
    tagPgap(2, :, :) = tagPgap(2, :, :) .* -1;
    tagPgap = sum(tagPgap) == 0;
    
%     tagMatch = [];
%     for p = 1 : size(tagPgap, 3),
%         tagMatch(p, :) = tagPgap(:, :, p);
%     end;
    tagMatch = reshape(tagPgap, size(tagPgap, 2), size(tagPgap, 3))';
    
    successpc = (sum(tagMatch, 2) / size(tagMatch, 2) * 100)';
    errorpc = 100 - successpc;
    
    %% COMMANDS
    command_epyegy  = ['[epy_min, epy_max, egy_min, egy_max, ' ...
                       'signal_entropy(i, j), '          ...
                       'signal_energy(i, j), '           ...
                       'baseline_entropy(i, j), '        ...
                       'baseline_energy(i, j), '         ...
                       'stimuli_entropy(i, j), '         ...
                       'stimuli_energy(i, j), '          ...
                       'signal_entropy_theta(i, j), '    ...
                       'signal_entropy_alpha(i, j), '    ...
                       'signal_entropy_beta(i, j), '     ...
                       'signal_entropy_gamma(i, j), '    ...
                       'signal_energy_theta(i, j), '     ...
                       'signal_energy_alpha(i, j), '     ...
                       'signal_energy_beta(i, j), '      ...
                       'signal_energy_gamma(i, j), '     ...
                       'baseline_entropy_theta(i, j), '  ...
                       'baseline_entropy_alpha(i, j), '  ...
                       'baseline_entropy_beta(i, j), '   ...
                       'baseline_entropy_gamma(i, j), '  ...
                       'stimuli_entropy_theta(i, j), '   ...
                       'stimuli_entropy_alpha(i, j), '   ...
                       'stimuli_entropy_beta(i, j), '    ...
                       'stimuli_entropy_gamma(i, j), '   ...
                       'baseline_energy_theta(i, j), '   ...
                       'baseline_energy_alpha(i, j), '   ...
                       'baseline_energy_beta(i, j), '    ...
                       'baseline_energy_gamma(i, j), '   ...
                       'stimuli_energy_theta(i, j), '    ...
                       'stimuli_energy_alpha(i, j), '    ...
                       'stimuli_energy_beta(i, j), '     ...
                       'stimuli_energy_gamma(i, j)] '    ...
                       '= ext_feat(fileName, channels, sampleRate, timeFlash, legacymode);'];
                       %'= ext_feat(fileName, rows, flashingMode, flash, channels, sampleRate, timeFlash, legacymode);'];
    
    command_rsquare1 = 'rsq1(i, j) = ext_rsq(fileName, rows, flashingMode, tagPredicted, channels, sampleRate, timeFlash, legacymode);';
    command_rsquare2 = 'rsq2(i, j) = ext_rsq(fileName, rows, flashingMode, tagPredicted, channels, sampleRate, timeFlash, legacymode);';
    
    %% SIGNALS ANALYSIS
    folders = ls(path);
    folders = folders(3:end, :);
    
    for j = 1 : size(folders, 1),
        
        folder = findEndOfString(folders(j, :));
        userpath = [path, folder, '/'];
        userfiles = ls([userpath, '*bin']);
        
        for i = 1 : size(userfiles, 1),
            userfile = findEndOfString(userfiles(i, :));
            fileName = [userpath, userfile];
            eval(command_epyegy);
            
            tagPredicted = tagP(1, i, j);
            eval(command_rsquare1);
            tagPredicted = tagP(2, i, j);
            eval(command_rsquare2);
        end;
        
    end;
    
    %% RESULTING VARIABLES
%     signal_entropy = [];
%     signal_entropy(:, :, 1) = signal_entropy_alpha;
%     signal_entropy(:, :, 2) = signal_entropy_beta;
%     signal_entropy(:, :, 3) = signal_entropy_theta;
%     signal_entropy(:, :, 4) = signal_entropy_gamma;
    
    baseline_entropy = [];
    baseline_entropy(:, :, 1) = baseline_entropy_alpha;
    baseline_entropy(:, :, 2) = baseline_entropy_beta;
    baseline_entropy(:, :, 3) = baseline_entropy_theta;
    baseline_entropy(:, :, 4) = baseline_entropy_gamma;
    
    stimuli_entropy = [];
    stimuli_entropy(:, :, 1) = stimuli_entropy_alpha;
    stimuli_entropy(:, :, 2) = stimuli_entropy_beta;
    stimuli_entropy(:, :, 3) = stimuli_entropy_theta;
    stimuli_entropy(:, :, 4) = stimuli_entropy_gamma;
    
%     signal_energy = [];
%     signal_energy(:, :, 1) = signal_energy_alpha;
%     signal_energy(:, :, 2) = signal_energy_beta;
%     signal_energy(:, :, 3) = signal_energy_theta;
%     signal_energy(:, :, 4) = signal_energy_gamma;
    
    baseline_energy = [];
    baseline_energy(:, :, 1) = baseline_energy_alpha;
    baseline_energy(:, :, 2) = baseline_energy_beta;
    baseline_energy(:, :, 3) = baseline_energy_theta;
    baseline_energy(:, :, 4) = baseline_energy_gamma;
    
    stimuli_energy = [];
    stimuli_energy(:, :, 1) = stimuli_energy_alpha;
    stimuli_energy(:, :, 2) = stimuli_energy_beta;
    stimuli_energy(:, :, 3) = stimuli_energy_theta;
    stimuli_energy(:, :, 4) = stimuli_energy_gamma;
    
    norm_baseline_entropy = [];
    for b = 1 : size(baseline_entropy, 3),
        norm_baseline_entropy(:, :, b) = baseline_entropy(:, :, b) - min(min(baseline_entropy(:, :, b)));
        norm_baseline_entropy(:, :, b) = - norm_baseline_entropy(:, :, b) / max(max(norm_baseline_entropy(:, :, b)));
    end;
    
    norm_stimuli_entropy = [];
    for b = 1 : size(stimuli_entropy, 3),
        norm_stimuli_entropy(:, :, b) = stimuli_entropy(:, :, b) - min(min(stimuli_entropy(:, :, b)));
        norm_stimuli_entropy(:, :, b) = - norm_stimuli_entropy(:, :, b) / max(max(norm_stimuli_entropy(:, :, b)));
    end;
    
    norm_baseline_energy = [];
    for b = 1 : size(baseline_energy, 3),
        norm_baseline_energy(:, :, b) = baseline_energy(:, :, b) - min(min(baseline_energy(:, :, b)));
        norm_baseline_energy(:, :, b) = norm_baseline_energy(:, :, b) / max(max(norm_baseline_energy(:, :, b)));
    end;
    
    norm_stimuli_energy = [];
    for b = 1 : size(stimuli_energy, 3),
        norm_stimuli_energy(:, :, b) = stimuli_energy(:, :, b) - min(min(stimuli_energy(:, :, b)));
        norm_stimuli_energy(:, :, b) = norm_stimuli_energy(:, :, b) / max(max(norm_stimuli_energy(:, :, b)));
    end;
    
    if (normalise),
        baseline_entropy = norm_baseline_entropy;
        stimuli_entropy  = norm_stimuli_entropy;
        baseline_energy  = norm_baseline_energy;
        stimuli_energy   = norm_stimuli_energy;
    end;
    
    % mean on raw values
    avg_baseline_epy = mean(mean(baseline_entropy(:, :, 1:3), 3), 1);
    avg_stimuli_epy  = mean(mean(stimuli_entropy(:, :, 1:3), 3), 1);
    avg_baseline_egy = mean(mean(baseline_energy(:, :, 1:3), 3), 1);
    avg_stimuli_egy  = mean(mean(stimuli_energy(:, :, 1:3), 3), 1);
    
%     % mean on normalised values
%     avg_baseline_epy = mean(mean(norm_baseline_entropy(:, :, 1:3), 3), 1);
%     avg_stimuli_epy  = mean(mean(norm_stimuli_entropy(:, :, 1:3), 3), 1);
%     avg_baseline_egy = mean(mean(norm_baseline_energy(:, :, 1:3), 3), 1);
%     avg_stimuli_egy  = mean(mean(norm_stimuli_energy(:, :, 1:3), 3), 1);
    
    mbepy = mean(avg_baseline_epy);
    sbepy = std(avg_baseline_epy);
    msepy = mean(avg_stimuli_epy);
    ssepy = std(avg_stimuli_epy);
    
    mbegy = mean(avg_baseline_egy);
    sbegy = std(avg_baseline_egy);
    msegy = mean(avg_stimuli_egy);
    ssegy = std(avg_stimuli_egy);
    
    epy = [];
    egy = [];
    
    epy_avg = reshape(mean(baseline_entropy, 2), size(baseline_entropy, 1), size(baseline_entropy, 3));
    egy_avg = reshape(mean(baseline_energy, 2), size(baseline_energy, 1), size(baseline_energy, 3));
    
    rsq = [];
    rsq(1, :, :) = rsq1;
    rsq(2, :, :) = rsq2;
    rsq(3, :, :) = rsq1 - rsq2;
    
    rsqNorm = [];
    scaleMatrix = reshape(max(max(rsq)), 1, size(rsq, 3));
    for p = 1 : size(rsq, 3),
        rsqNorm(:, :, p) = rsq(:, :, p) / scaleMatrix(p);
    end;
    
    rsq_target_avg  = reshape(mean(mean(rsq(1:2, :, :), 3), 1), 1, size(rsq, 2));
    rsqGap_user_avg = mean(abs(reshape(rsqNorm(3, :, :), size(rsqNorm, 2), size(rsqNorm, 3))));
    
    %% PLOTS
    
%     figure, set(gcf, 'numbertitle', 'off', 'name', 'Signal entropy');
%     for p = 1 : size(signal_entropy, 2),
%         subplot(3, 2, p), title(sprintf('Subject %i', p));
%         hold on;
%         plot(signal_entropy(:, p, 1), 'red');   % alpha
%         plot(signal_entropy(:, p, 2), 'green'); % beta
%         plot(signal_entropy(:, p, 3), 'blue');  % gamma
%         plot(signal_entropy(:, p, 4), 'black'); % theta
%         hold off;
%     end;

    figure, set(gcf, 'numbertitle', 'off', 'name', 'Signal Entropy');
    for p = 1 : size(signal_entropy, 2),
        subplot(3, 2, p), title(sprintf('Subject %i', p));
        hold on;
        plot(signal_entropy(:, p)', 'Color', coloursRGB2(2, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
        %plot(mean(signal_entropy2(:, p, :), 3), '--', 'Color', coloursRGB2(2, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
        set(gca, 'XTick', [1 : size(signal_entropy, 1)]);
        hold off;
    end;
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Signal Energy');
    for p = 1 : size(signal_energy, 2),
        subplot(3, 2, p), title(sprintf('Subject %i', p));
        hold on;
        plot(signal_energy(:, p)', 'Color', coloursRGB2(1, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
        %plot(sum(signal_energy2(:, p, :), 3), '--red', 'LineWidth', 1.5, 'LineSmoothing', 'on');
        set(gca, 'XTick', [1 : size(signal_energy, 1)]);
        hold off;
    end;
    
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Baseline Entropy');
    for p = 1 : size(baseline_entropy, 2),
        subplot(3, 2, p), title(sprintf('Subject %i', p));
        hold on;
        for b = 1 : size(baseline_entropy, 3),
            plot(baseline_entropy(:, p, b), 'Color', coloursRGB2(b, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
            set(gca, 'XTick', [1 : size(baseline_entropy, 1)]);
        end;
        hold off;
    end;
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Stimuli Entropy');
    for p = 1 : size(stimuli_entropy, 2),
        subplot(3, 2, p), title(sprintf('Subject %i', p));
        hold on;
        for b = 1 : size(stimuli_entropy, 3),
            plot(stimuli_entropy(:, p, b), 'Color', coloursRGB2(b, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
            set(gca, 'XTick', [1 : size(stimuli_entropy, 1)]);
        end;
        hold off;
    end;
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Baseline Energy');
    for p = 1 : size(baseline_energy, 2),
        subplot(3, 2, p), title(sprintf('Subject %i', p));
        hold on;
        for b = 1 : size(baseline_energy, 3),
            plot(baseline_energy(:, p, b), 'Color', coloursRGB2(b, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
            set(gca, 'XTick', [1 : size(baseline_energy, 1)]);
        end;
        hold off;
    end;
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Stimuli Energy');
    for p = 1 : size(stimuli_energy, 2),
        subplot(3, 2, p), title(sprintf('Subject %i', p));
        hold on;
        for b = 1 : size(stimuli_energy, 3),
            plot(stimuli_energy(:, p, b), 'Color', coloursRGB2(b, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
            set(gca, 'XTick', [1 : size(stimuli_energy, 1)]);
        end;
        hold off;
    end;
    
    figure, set(gcf, 'numbertitle', 'off', 'name', 'R^2');
    for p = 1 : size(rsq, 3),
        subplot(3, 2, p), title(sprintf('Subject %i', p)); % , xlabel('target n.'), ylabel('R^2');
        hold on;
        scatter(find(tagMatch(p, :) == 1), rsq1(find(tagMatch(p, :) == 1), p), [], coloursRGB2(2, :), 'filled', 'LineWidth', 1.5);
        plot(rsq1(:, p), 'Color', coloursRGB2(1, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
        plot(rsq2(:, p), 'Color', coloursRGB2(3, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
        set(gca, 'XTick', [1 : size(rsq, 2)]);
        hold off;
    end;
    
    figure, set(gcf, 'numbertitle', 'off', 'name', 'R^2');
    for p = 1 : size(rsq, 3),
        subplot(3, 2, p), title(sprintf('Subject %i', p)); % , xlabel('target n.'), ylabel('R^2');
        hold on;
        bar(rsq(:, :, p)');
        % axis([0.5 size(rsq, 2)+0.5 min(0, min(min(rsq(:, :, p)))) max(0, max(max(rsq(:, :, p))))]);
        set(gca, 'XTick', [1 : size(rsq, 2)]);
        axis('tight'), axis('auto y');
        hold off;
    end;

    figure, set(gcf, 'numbertitle', 'off', 'name', 'Base/Stim Entropy');
    for p = 1 : size(baseline_entropy, 2),
        base_epy = reshape(baseline_entropy(:, p, :), size(baseline_entropy, 1), size(baseline_entropy, 3));
        stim_epy = reshape(stimuli_entropy(:, p, :), size(stimuli_entropy,  1), size(stimuli_entropy,  3));
        epy(1, :, :) = base_epy;
        epy(2, :, :) = stim_epy;
        epy(3, :, :) = stim_epy - base_epy;
        for b = 1 : size(epy, 3),
            % subplot(size(baseline_entropy, 2), size(epy, 3), ((p-1) * size(epy, 3) + b)), title(sprintf('Band %i', b));
            subplot(size(baseline_entropy, 2), size(epy, 3), ((p-1) * size(epy, 3) + b)), title(sprintf(char(bands(b))));
            hold on;
            bar(epy(:, :, b)');
            set(gca, 'XTick', [1 : size(baseline_entropy, 1)]);
            % axis([0.5 size(epy, 2)+0.5 min(0, min(min(epy(:, :, b)))) max(0, max(max(epy(:, :, b))))]);
            axis('tight'); % , axis('auto y');
            hold off;
        end;
    end;
    
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Base/Stim Energy');
    for p = 1 : size(baseline_energy, 2),
        base_egy = reshape(baseline_energy(:, p, :), size(baseline_energy, 1), size(baseline_energy, 3));
        stim_egy = reshape(stimuli_energy(:, p, :), size(stimuli_energy,  1), size(stimuli_energy,  3));
        egy(1, :, :) = base_egy;
        egy(2, :, :) = stim_egy;
        egy(3, :, :) = stim_egy - base_egy;
        for b = 1 : size(egy, 3),
            subplot(size(stimuli_energy, 2), size(egy, 3), ((p-1) * size(egy, 3) + b)), title(sprintf(char(bands(b))));
            hold on;
            bar(egy(:, :, b)');
            set(gca, 'XTick', [1 : size(baseline_energy, 1)]);
            % axis([0.5 size(egy, 2)+0.5 min(0, min(min(egy(:, :, b)))) max(0, max(max(egy(:, :, b))))]);
            axis('tight'); % , axis('auto y');
            hold off;
        end;
    end;
    
    %
    
%     figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Entropy');
%     epy_avg = mean(mean(signal_entropy, 3), 2)';
%     plot(epy_avg);
%     
%     figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Energy');
%     egy_avg = mean(mean(signal_energy, 3), 2)';
%     plot(egy_avg);
    
    %
    
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Entropy/Energy');
    for b = 1 : size(epy_avg, 2),
        subplot(2, size(epy_avg, 2), b), title(sprintf(char(bands(b))));
        hold on;
        bar(epy_avg(:, b), 0.5, 'FaceColor', coloursRGB1(2, :)); % char(colours(b)));
        % h = plot(epy_avg(:, b), char(colours(b)), 'LineWidth', 1.5, 'LineSmoothing', 'on');
        h = plot(epy_avg(:, b), 'Color', coloursRGB2(b, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
        set(gca, 'XTick', [1 : size(epy_avg, 1)]);
        % set(h, 'marker', '+');
        axis('tight') , axis('auto y');
        hold off;
        subplot(2, size(epy_avg, 2), size(epy_avg, 2)+b), title(sprintf(char(bands(b))));
        hold on;
        bar(egy_avg(:, b), 0.5, 'FaceColor', coloursRGB1(1, :)); % char(colours(b)));
        % h = plot(egy_avg(:, b), char(colours(b)), 'LineWidth', 1.5, 'LineSmoothing', 'on');
        h = plot(egy_avg(:, b), 'Color', coloursRGB2(b, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
        set(gca, 'XTick', [1 : size(egy_avg, 1)]);
        % set(h, 'marker', '+');
        axis('tight') , axis('auto y');
        hold off;
    end;
    %figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Entropy/Energy');
    %subplot(1, 2, 1), title(sprintf('Average Entropy'));
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Entropy'); %, title(sprintf('Average Entropy'));
    hold on;
    bar(mean(signal_entropy, 2), 0.5, 'FaceColor', coloursRGB1(2, :));
    plot(mean(signal_entropy, 2), 'Color', coloursRGB2(2, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
    set(gca, 'XTick', [1 : size(signal_entropy, 2)]);
    axis('tight'), axis('auto y');
    hold off;
    %subplot(1, 2, 2), title(sprintf('Average Energy'));
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Energy'); %, title(sprintf('Average Energy'));
    hold on;
    bar(mean(signal_energy, 2), 0.5, 'FaceColor', coloursRGB1(1, :));
    plot(mean(signal_energy, 2), 'Color', coloursRGB2(1, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
    set(gca, 'XTick', [1 : size(signal_energy, 2)]);
    axis('tight'), axis('auto y');
    hold off;
%     subplot(2, size(epy_avg, 2), b), title(sprintf(char(bands(b))));
%     hold on;
%     plot(egy_avg(:, b), char(colours(b)), 'LineWidth', 1.5, 'LineSmoothing', 'on');
%     hold off;
%     subplot(2, size(epy_avg, 2), size(epy_avg, 2)+b), title(sprintf(char(bands(b))));
    
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Average R^2');
    hold on;
    bar(rsq_target_avg, 0.5, 'FaceColor', coloursRGB1(3, :)); % char(colours(b)));
    h = plot(rsq_target_avg, 'Color', coloursRGB2(3, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
    set(gca, 'XTick', [1 : size(rsq_target_avg, 2)]);
    axis('tight'), axis('auto y');
    hold off;
    
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Avg Classfication Error vs Avg R^2 Gap');
    hold on;
    b = bar([errorpc / max(errorpc); rsqGap_user_avg / max(rsqGap_user_avg)]');
    set(b(1), 'FaceColor', coloursRGB1(1, :));
    set(b(2), 'FaceColor', coloursRGB1(3, :));
    set(gca, 'XTick', [1 : size(errorpc, 2)]);
    %plot(errorpc / max(errorpc), '--', 'Color', coloursRGB2(1, :), 'LineWidth', 1, 'LineSmoothing', 'on');
    %plot(rsq_user_avg / max(rsq_user_avg), '--', 'Color', coloursRGB2(3, :), 'LineWidth', 1, 'LineSmoothing', 'on');
    axis('tight'), axis('auto y');
    hold off;
    
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Classfication Success distribution');
    hax = axes;
    hold on;
    x = linspace(0, mean(successpc)*2, 100);
    norm = normpdf(x, mean(successpc), std(successpc));
    plot(x, norm, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    line([mode(successpc), mode(successpc)], get(hax, 'YLim'), 'Color', coloursRGB2(1, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
    line([mean(successpc), mean(successpc)], get(hax, 'YLim'), 'Color', coloursRGB2(3, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
    hold off;
    
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Entropy distribution Base/Stim');
    hold on;
    %x = [-25 : .1 : 10] * 1e5;
    mepy = [mbepy, msepy];
    sepy = [sbepy, ssepy];
    ml = min(mbepy, msepy);
    mr = max(mbepy, msepy);
    sl = min(sbepy, ssepy);
    sr = max(sbepy, ssepy);
    x = linspace(ml-sr*4, mr+sr*4, 100);
    %idl = find(mepy == ml);
    %idr = find(mepy == mr);
    %x = linspace(ml-sepy(idl)*4, mr+sepy(idr)*4, 100);
    norm = normpdf(x, mbepy, sbepy);
    plot(x, norm, 'Color', coloursRGB2(3, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
    norm = normpdf(x, msepy, ssepy);
    plot(x, norm, 'Color', coloursRGB2(2, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
    hold off;
    
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Energy distribution Base/Stim');
    hold on;
    %x = [5 : .1 : 20];
    megy = [mbegy, msegy];
    segy = [sbegy, ssegy];
    ml = min(mbegy, msegy);
    mr = max(mbegy, msegy);
    sl = min(sbegy, ssegy);
    sr = max(sbegy, ssegy);
    x = linspace(ml-sr*4, mr+sr*4, 100);
    %idl = find(megy == ml);
    %idr = find(megy == mr);
    %x = linspace(ml-segy(idl)*4, mr+segy(idr)*4, 100);
    norm = normpdf(x, mbegy, sbegy);
    plot(x, norm, 'Color', coloursRGB2(3, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
    norm = normpdf(x, msegy, ssegy);
    plot(x, norm, 'Color', coloursRGB2(2, :), 'LineWidth', 1.5, 'LineSmoothing', 'on');
    hold off;
    
    
    
    
    
% utility function
function name = findEndOfString(string)
    firstspace = find(string == ' ', 1);
    if (isempty(firstspace))
        name = string;
    else
        name = string(1 : firstspace-1);
    end;