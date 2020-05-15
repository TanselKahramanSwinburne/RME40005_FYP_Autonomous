function f = plotArduinoData(MatrixData)
    angle = MatrixData(:,1);
    
    distance = MatrixData(:,2)/1000;
    
    for col=1:size(angle,1)
        while (angle(col) > 360)
            angle(col) = angle(col) - 360;
        end
    end
    angleR = deg2rad(angle)
    polarscatter(angleR,distance,'o');    
end