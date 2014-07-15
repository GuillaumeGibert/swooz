function [] = generate_fake_bottles(outDirectory,outFilename, values, newPositionTime, fps, libInfos)

% set variables
numbersValues = newPositionTime * fps;
timeInterval = newPositionTime / fps;

sizeValues = size(values);

% init arrays
times = zeros(numbersValues*(sizeValues(1)-1)+1, 1);
intervals = zeros(numbersValues*(sizeValues(1)-1)+1, sizeValues(2));

sizeIntervals = size(intervals);

% fill first line of intervals
for ii=1:sizeIntervals(2)
    intervals(1,ii) = values(1,ii);
end

% fill intervals and times arrays
for ii=1:sizeValues(1)-1
    
    % computes times
    currentTime = (ii-1) * newPositionTime;
    afterTime = ii * newPositionTime;

    
    % create interval time array
    time = decomposeInterval(currentTime, afterTime, numbersValues);
    
    for jj=1:sizeValues(2)
        
        % create intervales values array
        interval = decomposeInterval(values(ii,jj),values(ii+1,jj), numbersValues);
                
        for kk=1:numbersValues
            intervals(1 + (ii-1)*numbersValues+ kk, jj) = interval(kk);
        end        
    end
    
    
    for jj=1:numbersValues
       times(1 + (ii-1)*numbersValues+ jj) = time(jj);
    end
end
    


outSavePath = [outDirectory '/' outFilename '_' int2str(fps) '_' num2str(newPositionTime) '.txt'];

fid = fopen(outSavePath, 'w');

if (fid~=0)
	fprintf(fid, [libInfos '\n']);
	for ii=1:sizeIntervals(1)
        
        fprintf(fid, '%f', times(ii));
        
        for jj=1:sizeIntervals(2)
                        
            fprintf(fid, ' %f', intervals(ii,jj));
        end
        
        fprintf(fid, '\n');
		
	end
	
	fclose(fid);
else
	sprintf('cannot open file!!')
end