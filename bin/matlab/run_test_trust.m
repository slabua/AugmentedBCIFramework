function [userBaselFiles, userGameSFiles] = run_test_trust()
    path = '../BinFiles/test_trust/';

    folders = ls(path);
    folders = folders(3:end, :);

    for j = 1 : size(folders, 1),
        
        folder = findEndOfString(folders(j, :));
        userpath = [path, folder, '/'];
        
        userBaselFiles = ls([userpath, 'Basel*bin']);
        userGameSFiles = ls([userpath, 'GameS*bin']);
        
        %inputSignalBasel(j) = readBinSignal(userBaselFiles(j), 16)';
        
        for i = 1 : size(userBaselFiles, 1),
            userfile = findEndOfString(userBaselFiles(i, :));
            BaselFileName = [userpath, userfile]
            if (i < 10),
                BaselFileNameString = ['BaselFileName_Subject0', num2str(j)];
                BaselSignalNameString = ['inputSignalBasel_Subject0', num2str(i)];
            else,
                BaselFileNameString = ['BaselFileName_Subject', num2str(j)];
                BaselSignalNameString = ['inputSignalBasel_Subject', num2str(i)];
            end;
            eval([BaselSignalNameString, ' = transpose(readBinSignal(BaselFileName, 16));']);
        end;
%         for i = 1 : size(userGameSFiles, 1),
%             userfile = findEndOfString(userGameSFiles(i, :));
%             GameSFileName = [userpath, userfile]
%         end;
        
    end;
    
    a = 1;

% utility function
function name = findEndOfString(string)
    firstspace = find(string == ' ', 1);
    if (isempty(firstspace))
        name = string;
    else
        name = string(1 : firstspace-1);
    end;