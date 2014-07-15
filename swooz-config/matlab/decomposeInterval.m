function [outValues] = decomposeInterval(startValue,endValue,number)

diff = endValue - startValue;
valueToIncrement = diff / number;

outValues = [number 1];

outValues(1) = startValue;

for ii=1:number
    outValues(ii) = startValue + ii * valueToIncrement;
end
