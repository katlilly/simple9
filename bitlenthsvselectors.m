
data = csvread('bitwidthvsselectorall.txt')
intbits = data(1:32, 1)
intbitfreqs = data(1:32, 2)
selectors = data(33:end, 1)
selectorfreqs = data(33:end, 2)


plot(intbits, intbitfreqs, 'bo-', selectors, selectorfreqs, 'r+-')
%ylabel('number of ints with x bits or number of words with x selector')
title('Bit widths vs selector frequencies')


%scale selector stats for number of ints in each word

scaledselectorfreqs = zeros(size(selectorfreqs))

scaledselectorfreqs(1) = selectorfreqs(1) * 28
scaledselectorfreqs(2) = selectorfreqs(2) * 14
scaledselectorfreqs(3) = selectorfreqs(3) * 9
scaledselectorfreqs(4) = selectorfreqs(4) * 7
scaledselectorfreqs(5) = selectorfreqs(5) * 5
scaledselectorfreqs(6) = selectorfreqs(6) * 4
scaledselectorfreqs(7) = selectorfreqs(7) * 3
scaledselectorfreqs(8) = selectorfreqs(8) * 2
scaledselectorfreqs(9) = selectorfreqs(9) * 1

intselector = zeros(size(selectors))
for i = 1:5
  intselector(i) = intbitfreqs(i)
end

intselector(6) = sum(intbitfreqs(6:7))
intselector(7) = sum(intbitfreqs(8:9))
intselector(8) = sum(intbitfreqs(10:14))
intselector(9) = sum(intbitfreqs(15:28))



%plot(intbits, intbitfreqs, 'bo-', selectors, selectorfreqs, 'r+-', ...
%selectors, scaledselectorfreqs, 'g*-')
%title('Bit widths vs selector frequencies')

plot(selectors, intselector, 'bo-', selectors, scaledselectorfreqs, 'r+-')
title('Bit widths vs selector frequencies')


%sanity check, this should be a positive number
sum(scaledselectorfreqs) - sum(intselector)
