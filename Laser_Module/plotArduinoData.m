function f = plotArduinoData(MatrixData)
    angle = MatrixData(:,1);
    
    distance = MatrixData(:,2);
    
    angleR = deg2rad(angle);
    polarscatter(angleR,distance,'o');    
end