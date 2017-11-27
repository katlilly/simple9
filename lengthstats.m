a = csvread('lengthfreqs.txt');
lengths = a(:,1);
freqs = a(:,2);

plot(lengths,freqs)

lengths = a(1:100, 1)
freqs = a(1:100, 2)

plot(lengths,freqs)

%axis tight
axis([15 100 0 3e3])
xlabel('List length')
ylabel('Number of lists')
title('List length freqencies')
