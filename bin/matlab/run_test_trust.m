function [mental_wl, focusPC, entropy_allbands, energy_allbands, entropy_dec, energy_dec, ...
          entropy_theta, entropy_alpha, entropy_beta, entropy_gamma,  ...
          energy_theta,  energy_alpha,  energy_beta,  energy_gamma] = run_test_trust()
    
    clear all;
    
    sampleRate = 256;

    pathBasel = '../BinFiles/test_trust/txtBasel';
    pathGameS = '../BinFiles/test_trust/txtGameS';

    filesBasel = ls(pathBasel);
    filesBasel = filesBasel(3:end, :);
    filesGameS = ls(pathGameS);
    filesGameS = filesGameS(3:end, :);

    for subject = 1 : size(filesBasel, 1),
    %for subject = 10 : 16,
        
        subject
        
        userBaselFile = load([pathBasel, '/', filesBasel(subject, :)]);
        userGameSFile = load([pathGameS, '/', filesGameS(subject, :)]);
        %figure, plot(userGameSFile);
        baseline = userBaselFile(userBaselFile(:, end-1) == 1, :); % 0 eyes closed, 1 eyes open
        [fairTrials1, cheatTrials, fairTrials2, fairTrials, cheatTrialsReal] = extract_fair_cheat(userGameSFile);
        
%         [fair_entropy_theta,  fair_entropy_alpha,  fair_entropy_beta,  fair_entropy_gamma,  ...
%          cheat_entropy_theta, cheat_entropy_alpha, cheat_entropy_beta, cheat_entropy_gamma, ...
%          fair_energy_theta,   fair_energy_alpha,   fair_energy_beta,   fair_energy_gamma,   ...
%          cheat_energy_theta,  cheat_energy_alpha,  cheat_energy_beta,  cheat_energy_gamma] = epy_egy_game(userBaselFile, userGameSFile, sampleRate);
        
        [mental_wl(subject, :), focusPC(subject, :), entropy_allbands(subject, :), energy_allbands(subject, :),  ...
         entropy_theta(subject, :), entropy_alpha(subject, :), entropy_beta(subject, :), entropy_gamma(subject, :), ...
         energy_theta(subject, :),  energy_alpha(subject, :),  energy_beta(subject, :),  energy_gamma(subject, :)] = epy_egy_game_trials(baseline, fairTrials1, cheatTrials, fairTrials2, fairTrials, cheatTrialsReal, sampleRate);
     
    end;
    
    entropy_dec(:, :, 1) = entropy_alpha;
    entropy_dec(:, :, 2) = entropy_beta;
    entropy_dec(:, :, 3) = entropy_gamma;
    entropy_dec(:, :, 4) = entropy_theta;
    
    energy_dec(:, :, 1)  = energy_alpha;
    energy_dec(:, :, 2)  = energy_beta;
    energy_dec(:, :, 3)  = energy_gamma;
    energy_dec(:, :, 4)  = energy_theta;
    
    
% Utility function
function [fairTrials1, cheatTrials, fairTrials2, fairTrials, cheatTrialsReal] = extract_fair_cheat(userGameSFile)
    
%     fairTrials  = userGameSFile(userGameSFile(:, end-2) == -3, :);
%     fairTrials  = fairTrials(fairTrials(:, end-1) ~= 0, :);
%     cheatTrials = userGameSFile(userGameSFile(:, end-2) == -4, :);
    
    sessionIndicator  = userGameSFile(:, end-2);
    patternFair2Cheat = [-3, 1, -4];
    patternCheat2Fair = [-4, 1, -3];
    
    %splitF2C    = find_pattern(patternFair2Cheat, sessionIndicator);
    %splitC2F    = find_pattern(patternCheat2Fair, sessionIndicator);
    splitF2C    = find(sessionIndicator == -4, 1, 'first')-1;
    splitC2F    = find(sessionIndicator == -4, 1, 'last')+1;
    
    fairTrials1 = userGameSFile(1 : splitF2C, :);
    fairTrials1 = fairTrials1(fairTrials1(:,end-1) ~= 0, :);
    cheatTrials = userGameSFile(splitF2C+1 : splitC2F-1, :);
    fairTrials2 = userGameSFile(splitC2F+1 : (find(userGameSFile(:,end-2)==1, 1, 'last') -1), :);
    fairTrials  = [fairTrials1; fairTrials2];
    
    [cheatTrialsReal, isCheatTrialReal] = get_actual_cheatTrials(cheatTrials);
    % SLB TODO use isCheatTrialReal to identify on graphs the actual cheats
    
    
% Utility function
function index = find_pattern(pattern, vector)
    nA     = numel(vector);
    np     = numel(pattern);
    buffer = ~any(spdiags(repmat(vector(:), 1, np), 0:np-1, nA, nA) - ...
             spdiags(repmat(pattern, nA, 1), 0:np-1, nA, nA), 2);
    index  = find(full(buffer(1:nA-np+1))) + 1;
    
    
% Utility function
function [cheatTrialsReal, isCheatTrialReal] = get_actual_cheatTrials(cheatTrials)
    nextTrialC  = find(cheatTrials(:, end-2) == 1);
    nextTrialC = [1; nextTrialC; size(cheatTrials, 1)];
    
    cheatTrialsReal = [];
    isCheatTrialReal = [];
    for trial = 1 : size(nextTrialC)-1,
        trialSignal = cheatTrials(nextTrialC(trial)+1:nextTrialC(trial+1), :);
        if ((sum(trialSignal(:, end-1) == 3) ~= 0) || (sum(trialSignal(:, end-1) == 4) ~= 0)),
            cheatTrialsReal  = [cheatTrialsReal; trialSignal];
            isCheatTrialReal = [isCheatTrialReal, 1];
        else,
            isCheatTrialReal = [isCheatTrialReal, 0];
        end;
    end;
    
    