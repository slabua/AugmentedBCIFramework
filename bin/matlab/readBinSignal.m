function signal = readBinSignal(fileName, channels)
    
    path = fileName;
    
    id = fopen(path);
    signal = fread(id, [channels+3, inf], 'float32');
    fclose(id);
    
end