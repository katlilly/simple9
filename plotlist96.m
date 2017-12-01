list96 = csvread('list96.csv');

listnumber = list96(1,1)
length = list96(2,1)

rawbitwidths = list96(4:34,:);

% row 44 is start of 1 bit list
% rown 286 is end of 28 bit list
bitsbyselector = list96(44:286,:);


selectorbybits = list96(287:end,:);

maxbits = 28
%for i = 0:9
%  selector = selectorbybits(287+i:end,:)
%end

selector1 = selectorbybits(2:28,2)
selector2 = selectorbybits(30:56,2)
selector3 = selectorbybits(58:84,2)
selector4 = selectorbybits(86:112,2)
selector5 = selectorbybits(114:140,2)
selector7 = selectorbybits(142:168,2)
selector9 = selectorbybits(170:196,2)
selector14 = selectorbybits(198:224,2)
selector28 = selectorbybits(226:252,2)
fls = (1:27)

%{
plot(fls, selector2, fls, selector3, fls, selector4, ...
     fls, selector5, fls, selector7)
xlabel('bit width')
ylabel('number of ints')
title('List #96, 25722 postings')
legend({'2 bit selector','3 bit selector','4 bit selector','5 bit selector','7 bit selector'})
axis([1 10 0 5000])
%}


%**************************************
%*** plot where n-bit ints went *******
%**************************************
bitsbyselector = list96(44:286,:);
bitwidth1 = bitsbyselector(1:9,2)
bitwidth2 = bitsbyselector(10:18,2)
bitwidth3 = bitsbyselector(19:27,2)
bitwidth4 = bitsbyselector(28:36,2)
bitwidth5 = bitsbyselector(37:45,2)
bitwidth6 = bitsbyselector(46:54,2)
bitwidth7 = bitsbyselector(55:63,2)
bitwidth9 = bitsbyselector(64:72,2)
bitwidth14 = bitsbyselector(73:81,2)

selectors = (1:9)
selectors(6) = 7;
selectors(7) = 9;
selectors(8) = 14;
selectors(9) = 28;


plot(selectors, bitwidth1, selectors, bitwidth2, selectors, bitwidth3, selectors, bitwidth4, selectors, bitwidth5, selectors, bitwidth6, selectors, bitwidth7)
xlabel('selector')
ylabel('number of ints')
title('List #96, 25722 postings')
legend('1-bit ints','2-bit ints','3-bit ints','4-bit ints','5-bit ints', '6-bit ints', '7-bit ints')
axis([1 14 0 3500])

xticks([1 2 3 4 5 7 9 14])



%plot raw bit with counts
%***************************
bitwidths = csvread('list96bitwidths.csv')

bitwidths = bitwidths(4:13,:)
bits = bitwidths(1:end,1)
freqs = bitwidths(1:end,2)

plot(bits, freqs)
title('List #96, 25722 postings')
xlabel('bitwidths')
ylabel('number of ints')


%plot wasted bit stats for each selector
%******************************************
wastedbits = csvread('list96wbperselector.csv')
selectors = wastedbits(3:end,1)
timesused = wastedbits(3:end,2)
totalwasted = wastedbits(3:end,3)
wastedperword = wastedbits(3:end,4)
wastedperword(isnan(wastedperword)) = 0;

plot(selectors, totalwasted)
axis([0 10 0 20000])
title('total wasted bits for each selector, list96')

plot(selectors, wastedperword)
title('mean wasted bits per word for each selector, list96')
xticks([1 2 3 4 5 7 9 14])
axis([0 10 0 14])
