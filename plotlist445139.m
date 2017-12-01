list445139 = csvread('list445139.csv');

listnumber = list445139(1,1);
length = list445139(2,1);

rawbitwidths = list445139(4:34,:);



% row 44 is start of 1 bit list
% rown 286 is end of 28 bit list


%**************************************
%*** plot where n-bit ints went *******
%**************************************
bitsbyselector = list445139(44:286,:);
bitwidth1 = bitsbyselector(1:9,2);
bitwidth2 = bitsbyselector(10:18,2);
bitwidth3 = bitsbyselector(19:27,2);
bitwidth4 = bitsbyselector(28:36,2);

selectors = (1:9);

%plot(selectors, bitwidth1, selectors, bitwidth2, selectors, bitwidth3, selectors, bitwidth4)
%xlabel('selector')
%ylabel('number of ints')
%title('List #445139, 157870 postings')
%legend('1-bit ints','2-bit ints','3-bit ints','4-bit ints')



axis([1 9 0 100000])

xticks([1 2 3 4 5 7 9 14])
  
%*************************************
%**** plot how selectors were used ***
%*************************************
selectorbybits = list445139(287:end,:);

selector1 = selectorbybits(2:28,2);
selector2 = selectorbybits(30:56,2);
selector3 = selectorbybits(58:84,2);
selector4 = selectorbybits(86:112,2);
selector5 = selectorbybits(114:140,2);
selector7 = selectorbybits(142:168,2);
selector9 = selectorbybits(170:196,2);
selector14 = selectorbybits(198:224,2);
selector28 = selectorbybits(226:252,2);
fls = (1:27);

%plot(fls, selector1, fls, selector2, fls, selector3, ...
%     fls, selector4, fls, selector5)
%xlabel('bit width')
%ylabel('number of ints')
%title('List #445139, 157870 postings')
%  legend({'1 bit selector', '2 bit selector','3 bit selector','4 bit selector','5 bit selector'})

%axis([1 5 0 80000])



%plot raw bit with counts
%***************************
bitwidths = csvread('list445139bitwidths.csv')

bitwidths = bitwidths(4:10,:)
bits = bitwidths(1:end,1)
freqs = bitwidths(1:end,2)

plot(bits, freqs)
title('List #445139, 157870 postings')
xlabel('bitwidths')
ylabel('number of ints')


%plot wasted bit stats for each selector
%******************************************
wastedbits = csvread('list445139wbperselector.csv')
selectors = wastedbits(3:end,1)
timesused = wastedbits(3:end,2)
totalwasted = wastedbits(3:end,3)
wastedperword = wastedbits(3:end,4)
wastedperword(isnan(wastedperword)) = 0;


plot(selectors, wastedperword)
title('mean wasted bits per word for each selector, list445139')
xticks([1 2 3 4 5 7 9 14])
axis([0 10 0 18])

plot(selectors, totalwasted)
axis([0 10 0 60000])
title('total wasted bits for each selector, list445139')
