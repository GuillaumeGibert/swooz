
nbJoints = 6;
fps = 60
timeFrame = 1 / fps

numberMove = 12
timeMove = 3

nbFrames = numberMove * timeMove * fps;


timeMatrix = zeros(nbFrames, 1);
jointMatrix = zeros(nbFrames, nbJoints);

minJ = [-0.69 -1.22 -0.95]
maxJ = [0.52 1.04 0.87]

times = zeros(timeMove, 1);

for l_frame=1:numberMove
    times(l_frame) = l_frame * timeMove;
end

% get(4).asDouble()); // up/down head
% get(6).asDouble()); // left/right head
% get(5).asDouble()); // head



for l_ii=1:nbFrames
    
    currentTime = l_ii * timeFrame;
    timeMatrix(l_ii) = currentTime;
        
    % neutral -> min -> neutral -> max -> neutral [joint 6]
    % neutral -> min -> neutral -> max -> neutral [joint 5]
    % neutral -> min -> neutral -> max -> neutral [joint 4]
    if(currentTime < times(1)) 
        jointMatrix(l_ii, 6) = minJ(3) * (currentTime / timeMove);
    elseif (currentTime < times(2))
        jointMatrix(l_ii, 6) = minJ(3) * (timeMove - (currentTime -times(1)))/timeMove;
    elseif (currentTime < times(3))
        jointMatrix(l_ii, 6) = maxJ(3) * (currentTime - times(2)) /timeMove;
    elseif (currentTime < times(4))
        jointMatrix(l_ii, 6) = maxJ(3) * (timeMove - (currentTime -times(3)))/2;
    elseif (currentTime < times(5))
        jointMatrix(l_ii, 5) = minJ(2) * (currentTime - times(4)) /timeMove;
    elseif (currentTime < times(6))
        jointMatrix(l_ii, 5) = minJ(2) * (timeMove - (currentTime -times(5)))/timeMove;
    elseif (currentTime < times(7))
        jointMatrix(l_ii, 5) = maxJ(2) * (currentTime - times(6)) /timeMove;     
    elseif (currentTime < times(8))
        jointMatrix(l_ii, 5) = maxJ(2) * (timeMove - (currentTime -times(7)))/timeMove;
    elseif (currentTime < times(9))
        jointMatrix(l_ii, 4) = minJ(1) * (currentTime - times(8)) /timeMove;
    elseif (currentTime < times(10))
        jointMatrix(l_ii, 4) = minJ(1) * (timeMove - (currentTime -times(9)))/timeMove;
    elseif (currentTime < times(11))
        jointMatrix(l_ii, 4) = maxJ(1) * (currentTime - times(10)) /timeMove;     
    elseif (currentTime < times(12))
        jointMatrix(l_ii, 4) = maxJ(1) * (timeMove - (currentTime -times(11)))/timeMove;            
    end
end


fid = fopen('facelab.txt', 'w');
if (fid~=0)
	fprintf(fid, 'facelab coredata head COREDATA_LIB\n')
	for l_frame=1:nbFrames
		fprintf(fid, '%f %f %f %f %f %f %f\n', timeMatrix(l_frame,1), jointMatrix(l_frame,:));
	end
	
	fclose(fid);
else
	sprintf('cannot open file!!')
end