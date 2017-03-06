%function plot_test_results()
    
    legendData = ['Baseline'; 'Fair    '; 'Cheat   '];
    legendData = cellstr(legendData);
    
    %%% Sessions
    
    % Entropy / Energy global
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Entropy / Energy (baseline, fair, cheat)');
    subplot(2, 1, 1);
    bar(entropy_allbands);
    title(sprintf('Entropy'));
    axis([0, size(entropy_allbands, 1)+1, 0, 0.3]);
    set(gca, 'XTick', [1 : size(entropy_allbands, 1)], 'YGrid', 'on');
    subplot(2, 1, 2);
    bar(energy_allbands);
    title(sprintf('Energy')), xlabel('Subjects');
    axis([0, size(energy_allbands, 1)+1, 0, 6]);
    set(gca, 'XTick', [1 : size(energy_allbands, 1)], 'YGrid', 'on');
    legend(legendData, 'Location', 'NorthEast');
    
    % Mental Workload global
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Mental Workload (baseline, fair, cheat)');
    bar(mental_workload);
    title(sprintf('Mental Workload')), xlabel('Subjects');
    axis([0, size(mental_workload, 1)+1, 0, 2]);
    set(gca, 'XTick', [1 : size(mental_workload, 1)], 'YGrid', 'on');
    legend(legendData, 'Location', 'NorthEast');
    
    % Visual Focus global
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Visual Focus % (fair, cheat)');
    bar(focusPC(:, 2:3));
    title(sprintf('Visual Focus %%')), xlabel('Subjects');
    axis([0, size(focusPC, 1)+1, 0, 35]);
    set(gca, 'XTick', [1 : size(focusPC, 1)], 'YGrid', 'on');
    legend(legendData(2:end), 'Location', 'NorthEast');
    
    % Entropy Cheat-to-Win vs Cheat-to-Lose
    epy_w = entropy_allbands([1:2:size(entropy_allbands, 1)], :);
    epy_l = entropy_allbands([2:2:size(entropy_allbands, 1)], :);
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Entropy (baseline, fair, cheat)');
    subplot(2, 1, 1);
    %plot(epy_w, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    bar(epy_w);
    title(sprintf('Entropy for Cheat-to-Win class'));
    axis([0, size(epy_w, 1)+1, 0, 0.3]);
    set(gca, 'XTick', [1 : size(entropy_allbands, 1)], 'YGrid', 'on');
    subplot(2, 1, 2);
    %plot(epy_l, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    bar(epy_l);
    title(sprintf('Entropy for Cheat-to-Lose class')), xlabel('Subjects');
    axis([0, size(epy_l, 1)+1, 0, 0.3]);
    set(gca, 'XTick', [1 : size(entropy_allbands, 1)], 'YGrid', 'on');
    legend(legendData, 'Location', 'SouthEast');
    
    % Energy Cheat-to-Win vs Cheat-to-Lose
    egy_w = energy_allbands([1:2:size(energy_allbands, 1)], :);
    egy_l = energy_allbands([2:2:size(energy_allbands, 1)], :);
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Energy (baseline, fair, cheat)');
    subplot(2, 1, 1);
    %plot(egy_w, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    bar(egy_w);
    title(sprintf('Energy for Cheat-to-Win class'));
    axis([0, size(egy_w, 1)+1, 0, 6]);
    set(gca, 'XTick', [1 : size(entropy_allbands, 1)], 'YGrid', 'on');
    subplot(2, 1, 2);
    %plot(egy_l, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    bar(egy_l);
    title(sprintf('Energy for Cheat-to-Lose class')), xlabel('Subjects');
    axis([0, size(egy_l, 1)+1, 0, 6]);
    set(gca, 'XTick', [1 : size(entropy_allbands, 1)], 'YGrid', 'on');
    legend(legendData, 'Location', 'NorthEast');
    
    % Mental Workload Cheat-to-Win vs Cheat-to-Lose
    mw_w = mental_workload([1:2:size(mental_workload, 1)], :);
    mw_l = mental_workload([2:2:size(mental_workload, 1)], :);
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Mental Workload (baseline, fair, cheat)');
    subplot(2, 1, 1);
    %plot(mw_w, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    bar(mw_w);
    title(sprintf('Mental Workload for Cheat-to-Win class'));
    axis([0, size(mw_w, 1)+1, 0, 1.5]);
    set(gca, 'XTick', [1 : size(mental_workload, 1)], 'YGrid', 'on');
    subplot(2, 1, 2);
    %plot(mw_l, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    bar(mw_l);
    title(sprintf('Mental Workload for Cheat-to-Lose class')), xlabel('Subjects');
    axis([0, size(mw_l, 1)+1, 0, 1.5]);
    set(gca, 'XTick', [1 : size(mental_workload, 1)], 'YGrid', 'on');
    legend(legendData, 'Location', 'South');
    
    % Visual Focus Cheat-to-Win vs Cheat-to-Lose
    focusPC_w = focusPC([1:2:size(focusPC, 1)], 2:3);
    focusPC_l = focusPC([2:2:size(focusPC, 1)], 2:3);
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Visual Focus % (baseline, fair, cheat)');
    subplot(2, 1, 1);
    %plot(focusPC_w, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    bar(focusPC_w);
    title(sprintf('Visual Focus %% for Cheat-to-Win class'));
    axis([0, size(focusPC_w, 1)+1, 0, 40]);
    set(gca, 'XTick', [1 : size(focusPC_w, 1)], 'YGrid', 'on');
    legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(2, 1, 2);
    %plot(focusPC_l, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    bar(focusPC_l);
    title(sprintf('Visual Focus %% for Cheat-to-Lose class')), xlabel('Subjects');
    axis([0, size(focusPC_w, 1)+1, 0, 40]);
    set(gca, 'XTick', [1 : size(focusPC_l, 1)], 'YGrid', 'on');
    
    %%% Trials
    
    % Entropy per trial
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Entropy (trials)');
    subplot(8, 2, 1);
    plot(trialsSubject01_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    title(sprintf('Entropy for Cheat-to-Win class')), ylabel('Subj 01');
    axis([1, size(trialsSubject01_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject01_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 2);
    plot(trialsSubject02_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    title(sprintf('Entropy for Cheat-to-Lose class')), ylabel('Subj 02');
    axis([1, size(trialsSubject02_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject02_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 3);
    plot(trialsSubject03_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 03');
    axis([1, size(trialsSubject03_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject03_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 4);
    plot(trialsSubject04_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 04');
    axis([1, size(trialsSubject04_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject04_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 5);
    plot(trialsSubject05_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 05');
    axis([1, size(trialsSubject05_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject05_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 6);
    plot(trialsSubject06_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 06');
    axis([1, size(trialsSubject06_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject06_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 7);
    plot(trialsSubject07_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 07');
    axis([1, size(trialsSubject07_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject07_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 8);
    plot(trialsSubject08_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 08');
    axis([1, size(trialsSubject08_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject08_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 9);
    plot(trialsSubject09_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 09');
    axis([1, size(trialsSubject09_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject09_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 10);
    plot(trialsSubject10_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 10');
    axis([1, size(trialsSubject10_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject10_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 11);
    plot(trialsSubject11_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 11');
    axis([1, size(trialsSubject11_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject11_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 12);
    plot(trialsSubject12_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 12');
    axis([1, size(trialsSubject12_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject12_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 13);
    plot(trialsSubject13_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 13');
    axis([1, size(trialsSubject13_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject13_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 14);
    plot(trialsSubject14_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 14');
    axis([1, size(trialsSubject14_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject14_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 15);
    plot(trialsSubject15_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 15');
    axis([1, size(trialsSubject15_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject15_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 16);
    plot(trialsSubject16_entropy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 16');
    axis([1, size(trialsSubject16_entropy, 2), 0, 6]);
    set(gca, 'XTick', [1 : size(trialsSubject16_entropy, 2)], 'YTick', [0:2:6], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    
    % Energy per trial
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Energy (trials)');
    subplot(8, 2, 1);
    plot(trialsSubject01_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    title(sprintf('Energy for Cheat-to-Win class')), ylabel('Subj 01');
    axis([1, size(trialsSubject01_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject01_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 2);
    plot(trialsSubject02_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    title(sprintf('Energy for Cheat-to-Lose class')), ylabel('Subj 02');
    axis([1, size(trialsSubject02_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject02_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 3);
    plot(trialsSubject03_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 03');
    axis([1, size(trialsSubject03_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject03_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 4);
    plot(trialsSubject04_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 04');
    axis([1, size(trialsSubject04_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject04_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 5);
    plot(trialsSubject05_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 05');
    axis([1, size(trialsSubject05_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject05_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 6);
    plot(trialsSubject06_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 06');
    axis([1, size(trialsSubject06_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject06_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 7);
    plot(trialsSubject07_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 07');
    axis([1, size(trialsSubject07_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject07_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 8);
    plot(trialsSubject08_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 08');
    axis([1, size(trialsSubject08_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject08_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 9);
    plot(trialsSubject09_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 09');
    axis([1, size(trialsSubject09_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject09_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 10);
    plot(trialsSubject10_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 10');
    axis([1, size(trialsSubject10_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject10_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 11);
    plot(trialsSubject11_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 11');
    axis([1, size(trialsSubject11_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject11_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 12);
    plot(trialsSubject12_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 12');
    axis([1, size(trialsSubject12_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject12_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 13);
    plot(trialsSubject13_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 13');
    axis([1, size(trialsSubject13_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject13_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 14);
    plot(trialsSubject14_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 14');
    axis([1, size(trialsSubject14_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject14_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 15);
    plot(trialsSubject15_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 15');
    axis([1, size(trialsSubject15_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject15_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 16);
    plot(trialsSubject16_energy, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 16');
    axis([1, size(trialsSubject16_energy, 2), 0, 1]);
    set(gca, 'XTick', [1 : size(trialsSubject16_energy, 2)], 'YTick', [0:0.5:1], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    
    % Mental Workload per trial
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Mental Workload (trials)');
    subplot(8, 2, 1);
    plot(trialsSubject01_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    title(sprintf('Mental Workload for Cheat-to-Win class')), ylabel('Subj 01');
    axis([1, size(trialsSubject01_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject01_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 2);
    plot(trialsSubject02_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    title(sprintf('Mental Workload for Cheat-to-Lose class')), ylabel('Subj 02');
    axis([1, size(trialsSubject02_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject02_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 3);
    plot(trialsSubject03_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 03');
    axis([1, size(trialsSubject03_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject03_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 4);
    plot(trialsSubject04_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 04');
    axis([1, size(trialsSubject04_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject04_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 5);
    plot(trialsSubject05_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 05');
    axis([1, size(trialsSubject05_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject05_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 6);
    plot(trialsSubject06_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 06');
    axis([1, size(trialsSubject06_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject06_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 7);
    plot(trialsSubject07_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 07');
    axis([1, size(trialsSubject07_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject07_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 8);
    plot(trialsSubject08_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 08');
    axis([1, size(trialsSubject08_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject08_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 9);
    plot(trialsSubject09_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 09');
    axis([1, size(trialsSubject09_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject09_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 10);
    plot(trialsSubject10_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 10');
    axis([1, size(trialsSubject10_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject10_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 11);
    plot(trialsSubject11_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 11');
    axis([1, size(trialsSubject11_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject11_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 12);
    plot(trialsSubject12_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 12');
    axis([1, size(trialsSubject12_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject12_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 13);
    plot(trialsSubject13_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 13');
    axis([1, size(trialsSubject13_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject13_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 14);
    plot(trialsSubject14_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 14');
    axis([1, size(trialsSubject14_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject14_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 15);
    plot(trialsSubject15_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 15');
    axis([1, size(trialsSubject15_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject15_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 16);
    plot(trialsSubject16_mentalwl, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 16');
    axis([1, size(trialsSubject16_mentalwl, 2), 0, 3]);
    set(gca, 'XTick', [1 : size(trialsSubject16_mentalwl, 2)], 'YTick', [0:3], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    
    % Visual Focus per trial
    figure, set(gcf, 'numbertitle', 'off', 'name', 'Visual Focus % (trials)');
    subplot(8, 2, 1);
    plot(trialsSubject01_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    title(sprintf('Visual Focus %% for Cheat-to-Win class')), ylabel('Subj 01');
    axis([1, size(trialsSubject01_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject01_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 2);
    plot(trialsSubject02_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    title(sprintf('Visual Focus %% for Cheat-to-Lose class')), ylabel('Subj 02');
    axis([1, size(trialsSubject02_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject02_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 3);
    plot(trialsSubject03_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 03');
    axis([1, size(trialsSubject03_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject03_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 4);
    plot(trialsSubject04_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 04');
    axis([1, size(trialsSubject04_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject04_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 5);
    plot(trialsSubject05_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 05');
    axis([1, size(trialsSubject05_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject05_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 6);
    plot(trialsSubject06_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 06');
    axis([1, size(trialsSubject06_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject06_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 7);
    plot(trialsSubject07_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 07');
    axis([1, size(trialsSubject07_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject07_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 8);
    plot(trialsSubject08_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 08');
    axis([1, size(trialsSubject08_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject08_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 9);
    plot(trialsSubject09_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 09');
    axis([1, size(trialsSubject09_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject09_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 10);
    plot(trialsSubject10_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    ylabel('Subj 10');
    axis([1, size(trialsSubject10_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject10_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 11);
    plot(trialsSubject11_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 11');
    axis([1, size(trialsSubject11_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject11_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 12);
    plot(trialsSubject12_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 12');
    axis([1, size(trialsSubject12_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject12_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 13);
    plot(trialsSubject13_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 13');
    axis([1, size(trialsSubject13_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject13_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 14);
    plot(trialsSubject14_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 14');
    axis([1, size(trialsSubject14_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject14_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 15);
    plot(trialsSubject15_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 15');
    axis([1, size(trialsSubject15_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject15_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    subplot(8, 2, 16);
    plot(trialsSubject16_focusPC, 'LineWidth', 1.5, 'LineSmoothing', 'on');
    xlabel('trials'), ylabel('Subj 16');
    axis([1, size(trialsSubject16_focusPC, 2), 0, 100]);
    set(gca, 'XTick', [1 : size(trialsSubject16_focusPC, 2)], 'YTick', [0:50:100], 'YGrid', 'on');
    %legend(legendData(2:end), 'Location', 'NorthEast');
    
    %%% Average comparison
    
    legendDataClass = ['Cheat-to-Win '; 'Cheat-to-Lose'];
    legendDataClass = cellstr(legendDataClass);
    
    legendDataSubSession = ['Fair 1'; 'Cheat '; 'Fair 2'];
    legendDataSubSession = cellstr(legendDataSubSession);
    
    fairTrials1     = [1:5];
    fairTrials2     = [11:15];
    fairTrialsAll   = [1:5, 11:15];
    cheatTrials     = [6:10];
    ft = [[fairTrials1, fairTrials1]; fairTrialsAll];
    
%     for i = 1:1,
        %fairTrials = ft(i, :);
        fairTrials = fairTrials1;

        % Entropy
        meanEntropyR_fair_win = mean(mean( ...
            [trialsSubjectR01_entropy(fairTrials);
             trialsSubjectR03_entropy(fairTrials);
             trialsSubjectR07_entropy(fairTrials);
             trialsSubjectR09_entropy(fairTrials);
             trialsSubjectR11_entropy(fairTrials);
             trialsSubjectR13_entropy(fairTrials);
             trialsSubjectR15_entropy(fairTrials)] ...
            ));
        meanEntropyR_fair_lose = mean(mean( ...
            [trialsSubjectR02_entropy(fairTrials);
             trialsSubjectR04_entropy(fairTrials);
             trialsSubjectR06_entropy(fairTrials);
             trialsSubjectR08_entropy(fairTrials);
             trialsSubjectR10_entropy(fairTrials);
             trialsSubjectR14_entropy(fairTrials);
             trialsSubjectR16_entropy(fairTrials)] ...
            ));
        meanEntropyR_cheat_win = mean(mean( ...
            [trialsSubjectR01_entropy(cheatTrials);
             trialsSubjectR03_entropy(cheatTrials);
             trialsSubjectR07_entropy(cheatTrials);
             trialsSubjectR09_entropy(cheatTrials);
             trialsSubjectR11_entropy(cheatTrials);
             trialsSubjectR13_entropy(cheatTrials);
             trialsSubjectR15_entropy(cheatTrials)] ...
            ));
        meanEntropyR_cheat_lose = mean(mean( ...
            [trialsSubjectR02_entropy(cheatTrials);
             trialsSubjectR04_entropy(cheatTrials);
             trialsSubjectR06_entropy(cheatTrials);
             trialsSubjectR08_entropy(cheatTrials);
             trialsSubjectR10_entropy(cheatTrials);
             trialsSubjectR14_entropy(cheatTrials);
             trialsSubjectR16_entropy(cheatTrials)] ...
            ));
        meanEntropyR_fair_win2 = mean(mean( ...
            [trialsSubjectR01_entropy(fairTrials2);
             trialsSubjectR03_entropy(fairTrials2);
             trialsSubjectR07_entropy(fairTrials2);
             trialsSubjectR09_entropy(fairTrials2);
             trialsSubjectR11_entropy(fairTrials2);
             trialsSubjectR13_entropy(fairTrials2);
             trialsSubjectR15_entropy(fairTrials2)] ...
            ));
        meanEntropyR_fair_lose2 = mean(mean( ...
            [trialsSubjectR02_entropy(fairTrials2);
             trialsSubjectR04_entropy(fairTrials2);
             trialsSubjectR06_entropy(fairTrials2);
             trialsSubjectR08_entropy(fairTrials2);
             trialsSubjectR10_entropy(fairTrials2);
             trialsSubjectR14_entropy(fairTrials2);
             trialsSubjectR16_entropy(fairTrials2)] ...
            ));

        % Energy
        meanEnergyR_fair_win = mean(mean( ...
            [trialsSubjectR01_energy(fairTrials);
             trialsSubjectR03_energy(fairTrials);
             trialsSubjectR07_energy(fairTrials);
             trialsSubjectR09_energy(fairTrials);
             trialsSubjectR11_energy(fairTrials);
             trialsSubjectR13_energy(fairTrials);
             trialsSubjectR15_energy(fairTrials)] ...
            ));
        meanEnergyR_fair_lose = mean(mean( ...
            [trialsSubjectR02_energy(fairTrials);
             trialsSubjectR04_energy(fairTrials);
             trialsSubjectR06_energy(fairTrials);
             trialsSubjectR08_energy(fairTrials);
             trialsSubjectR10_energy(fairTrials);
             trialsSubjectR14_energy(fairTrials);
             trialsSubjectR16_energy(fairTrials)] ...
            ));
        meanEnergyR_cheat_win = mean(mean( ...
            [trialsSubjectR01_energy(cheatTrials);
             trialsSubjectR03_energy(cheatTrials);
             trialsSubjectR07_energy(cheatTrials);
             trialsSubjectR09_energy(cheatTrials);
             trialsSubjectR11_energy(cheatTrials);
             trialsSubjectR13_energy(cheatTrials);
             trialsSubjectR15_energy(cheatTrials)] ...
            ));
        meanEnergyR_cheat_lose = mean(mean( ...
            [trialsSubjectR02_energy(cheatTrials);
             trialsSubjectR04_energy(cheatTrials);
             trialsSubjectR06_energy(cheatTrials);
             trialsSubjectR08_energy(cheatTrials);
             trialsSubjectR10_energy(cheatTrials);
             trialsSubjectR14_energy(cheatTrials);
             trialsSubjectR16_energy(cheatTrials)] ...
            ));
        meanEnergyR_fair_win2 = mean(mean( ...
            [trialsSubjectR01_energy(fairTrials2);
             trialsSubjectR03_energy(fairTrials2);
             trialsSubjectR07_energy(fairTrials2);
             trialsSubjectR09_energy(fairTrials2);
             trialsSubjectR11_energy(fairTrials2);
             trialsSubjectR13_energy(fairTrials2);
             trialsSubjectR15_energy(fairTrials2)] ...
            ));
        meanEnergyR_fair_lose2 = mean(mean( ...
            [trialsSubjectR02_energy(fairTrials2);
             trialsSubjectR04_energy(fairTrials2);
             trialsSubjectR06_energy(fairTrials2);
             trialsSubjectR08_energy(fairTrials2);
             trialsSubjectR10_energy(fairTrials2);
             trialsSubjectR14_energy(fairTrials2);
             trialsSubjectR16_energy(fairTrials2)] ...
            ));

        % Mental Workload
        meanMentalwlR_fair_win = mean(mean( ...
            [trialsSubjectR01_mentalwl(fairTrials);
             trialsSubjectR03_mentalwl(fairTrials);
             trialsSubjectR07_mentalwl(fairTrials);
             trialsSubjectR09_mentalwl(fairTrials);
             trialsSubjectR11_mentalwl(fairTrials)] ...
            ));
        meanMentalwlR_fair_lose = mean(mean( ...
            [trialsSubjectR02_mentalwl(fairTrials);
             trialsSubjectR04_mentalwl(fairTrials);
             trialsSubjectR06_mentalwl(fairTrials);
             trialsSubjectR08_mentalwl(fairTrials);
             trialsSubjectR10_mentalwl(fairTrials)] ...
            ));
        meanMentalwlR_cheat_win = mean(mean( ...
            [trialsSubjectR01_mentalwl(cheatTrials);
             trialsSubjectR03_mentalwl(cheatTrials);
             trialsSubjectR07_mentalwl(cheatTrials);
             trialsSubjectR09_mentalwl(cheatTrials);
             trialsSubjectR11_mentalwl(cheatTrials)] ...
            ));
        meanMentalwlR_cheat_lose = mean(mean( ...
            [trialsSubjectR02_mentalwl(cheatTrials);
             trialsSubjectR04_mentalwl(cheatTrials);
             trialsSubjectR06_mentalwl(cheatTrials);
             trialsSubjectR08_mentalwl(cheatTrials);
             trialsSubjectR10_mentalwl(cheatTrials)] ...
            ));
        meanMentalwlR_fair_win2 = mean(mean( ...
            [trialsSubjectR01_mentalwl(fairTrials2);
             trialsSubjectR03_mentalwl(fairTrials2);
             trialsSubjectR07_mentalwl(fairTrials2);
             trialsSubjectR09_mentalwl(fairTrials2);
             trialsSubjectR11_mentalwl(fairTrials2)] ...
            ));
        meanMentalwlR_fair_lose2 = mean(mean( ...
            [trialsSubjectR02_mentalwl(fairTrials2);
             trialsSubjectR04_mentalwl(fairTrials2);
             trialsSubjectR06_mentalwl(fairTrials2);
             trialsSubjectR08_mentalwl(fairTrials2);
             trialsSubjectR10_mentalwl(fairTrials2)] ...
            ));

        % Visual Focus
        meanFocusPCR_fair_win = mean(mean( ...
            [trialsSubjectR01_focusPC(fairTrials);
             trialsSubjectR03_focusPC(fairTrials);
             trialsSubjectR07_focusPC(fairTrials);
             trialsSubjectR09_focusPC(fairTrials);
             trialsSubjectR11_focusPC(fairTrials)] ...
            ));
        meanFocusPCR_fair_lose = mean(mean( ...
            [trialsSubjectR02_focusPC(fairTrials);
             trialsSubjectR04_focusPC(fairTrials);
             trialsSubjectR06_focusPC(fairTrials);
             trialsSubjectR08_focusPC(fairTrials);
             trialsSubjectR10_focusPC(fairTrials)] ...
            ));
        meanFocusPCR_cheat_win = mean(mean( ...
            [trialsSubjectR01_focusPC(cheatTrials);
             trialsSubjectR03_focusPC(cheatTrials);
             trialsSubjectR07_focusPC(cheatTrials);
             trialsSubjectR09_focusPC(cheatTrials);
             trialsSubjectR11_focusPC(cheatTrials)] ...
            ));
        meanFocusPCR_cheat_lose = mean(mean( ...
            [trialsSubjectR02_focusPC(cheatTrials);
             trialsSubjectR04_focusPC(cheatTrials);
             trialsSubjectR06_focusPC(cheatTrials);
             trialsSubjectR08_focusPC(cheatTrials);
             trialsSubjectR10_focusPC(cheatTrials)] ...
            ));
        meanFocusPCR_fair_win2 = mean(mean( ...
            [trialsSubjectR01_focusPC(fairTrials2);
             trialsSubjectR03_focusPC(fairTrials2);
             trialsSubjectR07_focusPC(fairTrials2);
             trialsSubjectR09_focusPC(fairTrials2);
             trialsSubjectR11_focusPC(fairTrials2)] ...
            ));
        meanFocusPCR_fair_lose2 = mean(mean( ...
            [trialsSubjectR02_focusPC(fairTrials2);
             trialsSubjectR04_focusPC(fairTrials2);
             trialsSubjectR06_focusPC(fairTrials2);
             trialsSubjectR08_focusPC(fairTrials2);
             trialsSubjectR10_focusPC(fairTrials2)] ...
            ));
        
        %%% stddev
        % Entropy
        stdEntropyR_fair_win = (std2( ...
            [trialsSubjectR01_entropy(fairTrials);
             trialsSubjectR03_entropy(fairTrials);
             trialsSubjectR07_entropy(fairTrials);
             trialsSubjectR09_entropy(fairTrials);
             trialsSubjectR11_entropy(fairTrials)] ...
            ));
        stdEntropyR_fair_lose = (std2( ...
            [trialsSubjectR02_entropy(fairTrials);
             trialsSubjectR04_entropy(fairTrials);
             trialsSubjectR06_entropy(fairTrials);
             trialsSubjectR08_entropy(fairTrials);
             trialsSubjectR10_entropy(fairTrials)] ...
            ));
        stdEntropyR_cheat_win = (std2( ...
            [trialsSubjectR01_entropy(cheatTrials);
             trialsSubjectR03_entropy(cheatTrials);
             trialsSubjectR07_entropy(cheatTrials);
             trialsSubjectR09_entropy(cheatTrials);
             trialsSubjectR11_entropy(cheatTrials)] ...
            ));
        stdEntropyR_cheat_lose = (std2( ...
            [trialsSubjectR02_entropy(cheatTrials);
             trialsSubjectR04_entropy(cheatTrials);
             trialsSubjectR06_entropy(cheatTrials);
             trialsSubjectR08_entropy(cheatTrials);
             trialsSubjectR10_entropy(cheatTrials)] ...
            ));
        stdEntropyR_fair_win2 = (std2( ...
            [trialsSubjectR01_entropy(fairTrials2);
             trialsSubjectR03_entropy(fairTrials2);
             trialsSubjectR07_entropy(fairTrials2);
             trialsSubjectR09_entropy(fairTrials2);
             trialsSubjectR11_entropy(fairTrials2)] ...
            ));
        stdEntropyR_fair_lose2 = (std2( ...
            [trialsSubjectR02_entropy(fairTrials2);
             trialsSubjectR04_entropy(fairTrials2);
             trialsSubjectR06_entropy(fairTrials2);
             trialsSubjectR08_entropy(fairTrials2);
             trialsSubjectR10_entropy(fairTrials2)] ...
            ));

        % Energy
        stdEnergyR_fair_win = (std2( ...
            [trialsSubjectR01_energy(fairTrials);
             trialsSubjectR03_energy(fairTrials);
             trialsSubjectR07_energy(fairTrials);
             trialsSubjectR09_energy(fairTrials);
             trialsSubjectR11_energy(fairTrials)] ...
            ));
        stdEnergyR_fair_lose = (std2( ...
            [trialsSubjectR02_energy(fairTrials);
             trialsSubjectR04_energy(fairTrials);
             trialsSubjectR06_energy(fairTrials);
             trialsSubjectR08_energy(fairTrials);
             trialsSubjectR10_energy(fairTrials)] ...
            ));
        stdEnergyR_cheat_win = (std2( ...
            [trialsSubjectR01_energy(cheatTrials);
             trialsSubjectR03_energy(cheatTrials);
             trialsSubjectR07_energy(cheatTrials);
             trialsSubjectR09_energy(cheatTrials);
             trialsSubjectR11_energy(cheatTrials)] ...
            ));
        stdEnergyR_cheat_lose = (std2( ...
            [trialsSubjectR02_energy(cheatTrials);
             trialsSubjectR04_energy(cheatTrials);
             trialsSubjectR06_energy(cheatTrials);
             trialsSubjectR08_energy(cheatTrials);
             trialsSubjectR10_energy(cheatTrials)] ...
            ));
        stdEnergyR_fair_win2 = (std2( ...
            [trialsSubjectR01_energy(fairTrials2);
             trialsSubjectR03_energy(fairTrials2);
             trialsSubjectR07_energy(fairTrials2);
             trialsSubjectR09_energy(fairTrials2);
             trialsSubjectR11_energy(fairTrials2)] ...
            ));
        stdEnergyR_fair_lose2 = (std2( ...
            [trialsSubjectR02_energy(fairTrials2);
             trialsSubjectR04_energy(fairTrials2);
             trialsSubjectR06_energy(fairTrials2);
             trialsSubjectR08_energy(fairTrials2);
             trialsSubjectR10_energy(fairTrials2)] ...
            ));

        % Mental Workload
        stdMentalwlR_fair_win = (std2( ...
            [trialsSubjectR01_mentalwl(fairTrials);
             trialsSubjectR03_mentalwl(fairTrials);
             trialsSubjectR07_mentalwl(fairTrials);
             trialsSubjectR09_mentalwl(fairTrials);
             trialsSubjectR11_mentalwl(fairTrials)] ...
            ));
        stdMentalwlR_fair_lose = (std2( ...
            [trialsSubjectR02_mentalwl(fairTrials);
             trialsSubjectR04_mentalwl(fairTrials);
             trialsSubjectR06_mentalwl(fairTrials);
             trialsSubjectR08_mentalwl(fairTrials);
             trialsSubjectR10_mentalwl(fairTrials)] ...
            ));
        stdMentalwlR_cheat_win = (std2( ...
            [trialsSubjectR01_mentalwl(cheatTrials);
             trialsSubjectR03_mentalwl(cheatTrials);
             trialsSubjectR07_mentalwl(cheatTrials);
             trialsSubjectR09_mentalwl(cheatTrials);
             trialsSubjectR11_mentalwl(cheatTrials)] ...
            ));
        stdMentalwlR_cheat_lose = (std2( ...
            [trialsSubjectR02_mentalwl(cheatTrials);
             trialsSubjectR04_mentalwl(cheatTrials);
             trialsSubjectR06_mentalwl(cheatTrials);
             trialsSubjectR08_mentalwl(cheatTrials);
             trialsSubjectR10_mentalwl(cheatTrials)] ...
            ));
        stdMentalwlR_fair_win2 = (std2( ...
            [trialsSubjectR01_mentalwl(fairTrials2);
             trialsSubjectR03_mentalwl(fairTrials2);
             trialsSubjectR07_mentalwl(fairTrials2);
             trialsSubjectR09_mentalwl(fairTrials2);
             trialsSubjectR11_mentalwl(fairTrials2)] ...
            ));
        stdMentalwlR_fair_lose2 = (std2( ...
            [trialsSubjectR02_mentalwl(fairTrials2);
             trialsSubjectR04_mentalwl(fairTrials2);
             trialsSubjectR06_mentalwl(fairTrials2);
             trialsSubjectR08_mentalwl(fairTrials2);
             trialsSubjectR10_mentalwl(fairTrials2)] ...
            ));

        % Visual Focus
        stdFocusPCR_fair_win = (std2( ...
            [trialsSubjectR01_focusPC(fairTrials);
             trialsSubjectR03_focusPC(fairTrials);
             trialsSubjectR07_focusPC(fairTrials);
             trialsSubjectR09_focusPC(fairTrials);
             trialsSubjectR11_focusPC(fairTrials)] ...
            ));
        stdFocusPCR_fair_lose = (std2( ...
            [trialsSubjectR02_focusPC(fairTrials);
             trialsSubjectR04_focusPC(fairTrials);
             trialsSubjectR06_focusPC(fairTrials);
             trialsSubjectR08_focusPC(fairTrials);
             trialsSubjectR10_focusPC(fairTrials)] ...
            ));
        stdFocusPCR_cheat_win = (std2( ...
            [trialsSubjectR01_focusPC(cheatTrials);
             trialsSubjectR03_focusPC(cheatTrials);
             trialsSubjectR07_focusPC(cheatTrials);
             trialsSubjectR09_focusPC(cheatTrials);
             trialsSubjectR11_focusPC(cheatTrials)] ...
            ));
        stdFocusPCR_cheat_lose = (std2( ...
            [trialsSubjectR02_focusPC(cheatTrials);
             trialsSubjectR04_focusPC(cheatTrials);
             trialsSubjectR06_focusPC(cheatTrials);
             trialsSubjectR08_focusPC(cheatTrials);
             trialsSubjectR10_focusPC(cheatTrials)] ...
            ));
        stdFocusPCR_fair_win2 = (std2( ...
            [trialsSubjectR01_focusPC(fairTrials2);
             trialsSubjectR03_focusPC(fairTrials2);
             trialsSubjectR07_focusPC(fairTrials2);
             trialsSubjectR09_focusPC(fairTrials2);
             trialsSubjectR11_focusPC(fairTrials2)] ...
            ));
        stdFocusPCR_fair_lose2 = (std2( ...
            [trialsSubjectR02_focusPC(fairTrials2);
             trialsSubjectR04_focusPC(fairTrials2);
             trialsSubjectR06_focusPC(fairTrials2);
             trialsSubjectR08_focusPC(fairTrials2);
             trialsSubjectR10_focusPC(fairTrials2)] ...
            ));
        %%%
        
%         % Entropy
%         figure, set(gcf, 'numbertitle', 'off', 'name', 'Entropy');
%         bar([meanEntropyR_fair_win,  meanEntropyR_fair_lose;
%              meanEntropyR_cheat_win, meanEntropyR_cheat_lose]);
%         title(sprintf('Entropy')); %, xlabel('Fair / Cheat');
%         set(gca, 'XTickLabel', xtickLabels, 'YGrid', 'on');
%         legend(legendDataMean, 'Location', 'NorthEast');
% 
%         % Energy
%         figure, set(gcf, 'numbertitle', 'off', 'name', 'Energy');
%         bar([meanEnergyR_fair_win,  meanEnergyR_fair_lose;
%              meanEnergyR_cheat_win, meanEnergyR_cheat_lose]);
%         title(sprintf('Energy')); %, xlabel('Fair / Cheat');
%         set(gca, 'XTickLabel', xtickLabels, 'YGrid', 'on');
%         legend(legendDataMean, 'Location', 'NorthEast');
% 
%         % Mental Workload
%         figure, set(gcf, 'numbertitle', 'off', 'name', 'Mental Workload');
%         bar([meanMentalwlR_fair_win,  meanMentalwlR_fair_lose;
%              meanMentalwlR_cheat_win, meanMentalwlR_cheat_lose]);
%         title(sprintf('Mental Workload')); %, xlabel('Fair / Cheat');
%         set(gca, 'XTickLabel', xtickLabels, 'YGrid', 'on');
%         legend(legendDataMean, 'Location', 'NorthEast');
% 
%         % Visual Focus
%         figure, set(gcf, 'numbertitle', 'off', 'name', 'Visual Focus %');
%         bar([meanFocusPCR_fair_win,  meanFocusPCR_fair_lose;
%              meanFocusPCR_cheat_win, meanFocusPCR_cheat_lose]);
%         title(sprintf('Visual Focus %%')); %, xlabel('Fair / Cheat');
%         set(gca, 'XTickLabel', xtickLabels, 'YGrid', 'on');
%         legend(legendDataMean, 'Location', 'NorthEast');
%         
%     end;
    
        % Entropy
        figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Entropy per sub-session');
        bar([meanEntropyR_fair_win,  meanEntropyR_fair_lose;
             meanEntropyR_cheat_win, meanEntropyR_cheat_lose;
             meanEntropyR_fair_win2, meanEntropyR_fair_lose2]);
        title(sprintf('Average Entropy per sub-session')); %, xlabel('Fair / Cheat');
        set(gca, 'XTickLabel', legendDataSubSession, 'YGrid', 'on');
        legend(legendDataClass, 'Location', 'SouthEast');

        % Energy
        figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Energy per sub-session');
        bar([meanEnergyR_fair_win,  meanEnergyR_fair_lose;
             meanEnergyR_cheat_win, meanEnergyR_cheat_lose;
             meanEnergyR_fair_win2, meanEnergyR_fair_lose2]);
        title(sprintf('Average Energy per sub-session')); %, xlabel('Fair / Cheat');
        set(gca, 'XTickLabel', legendDataSubSession, 'YGrid', 'on');
        legend(legendDataClass, 'Location', 'NorthEast');

        % Mental Workload
        figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Mental Workload per sub-session');
        bar([meanMentalwlR_fair_win,  meanMentalwlR_fair_lose;
             meanMentalwlR_cheat_win, meanMentalwlR_cheat_lose;
             meanMentalwlR_fair_win2, meanMentalwlR_fair_lose2]);
        title(sprintf('Average Mental Workload per sub-session')); %, xlabel('Fair / Cheat');
        set(gca, 'XTickLabel', legendDataSubSession, 'YGrid', 'on');
        legend(legendDataClass, 'Location', 'NorthEast');

        % Visual Focus
        figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Visual Focus % per sub-session');
        bar([meanFocusPCR_fair_win,  meanFocusPCR_fair_lose;
             meanFocusPCR_cheat_win, meanFocusPCR_cheat_lose;
             meanFocusPCR_fair_win2, meanFocusPCR_fair_lose2]);
        title(sprintf('Average Visual Focus %% per sub-session')); %, xlabel('Fair / Cheat');
        set(gca, 'XTickLabel', legendDataSubSession, 'YGrid', 'on');
        legend(legendDataClass, 'Location', 'NorthWest');
        
        %%%
        
        % Entropy
        figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Entropy per sub-session');
        bar([meanEntropyR_fair_win,  meanEntropyR_cheat_win,  meanEntropyR_fair_win2;
             meanEntropyR_fair_lose, meanEntropyR_cheat_lose, meanEntropyR_fair_lose2]);
        title(sprintf('Average Entropy per sub-session')); %, xlabel('Fair / Cheat');
        set(gca, 'XTickLabel', legendDataClass, 'YGrid', 'on');
        legend(legendDataSubSession, 'Location', 'SouthEast');
        
        % Energy
        figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Energy per sub-session');
        bar([meanEnergyR_fair_win,  meanEnergyR_cheat_win,  meanEnergyR_fair_win2;
             meanEnergyR_fair_lose, meanEnergyR_cheat_lose, meanEnergyR_fair_lose2]);
        title(sprintf('Average Energy per sub-session')); %, xlabel('Fair / Cheat');
        set(gca, 'XTickLabel', legendDataClass, 'YGrid', 'on');
        legend(legendDataSubSession, 'Location', 'NorthEast');
        
        % Mental Workload
        figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Mental Workload per sub-session');
        bar([meanMentalwlR_fair_win,  meanMentalwlR_cheat_win,  meanMentalwlR_fair_win2;
             meanMentalwlR_fair_lose, meanMentalwlR_cheat_lose, meanMentalwlR_fair_lose2]);
        title(sprintf('Average Mental Workload per sub-session')); %, xlabel('Fair / Cheat');
        set(gca, 'XTickLabel', legendDataClass, 'YGrid', 'on');
        legend(legendDataSubSession, 'Location', 'SouthEast');
        
        % Visual Focus
        figure, set(gcf, 'numbertitle', 'off', 'name', 'Average Visual Focus % per sub-session');
        bar([meanFocusPCR_fair_win,  meanFocusPCR_cheat_win,  meanFocusPCR_fair_win2;
             meanFocusPCR_fair_lose, meanFocusPCR_cheat_lose, meanFocusPCR_fair_lose2]);
        title(sprintf('Average Visual Focus %% per sub-session')); %, xlabel('Fair / Cheat');
        set(gca, 'XTickLabel', legendDataClass, 'YGrid', 'on');
        legend(legendDataSubSession, 'Location', 'NorthEast');
        
        