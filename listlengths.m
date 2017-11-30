lengths = (1:5)
totalitems = ([3076857 7461238 17443866 12949474 274495])
check = sum(totalitems)
cumulative = 41205930
%error = cumulative - check

plot(lengths, totalitems, 'bo')
title('Total items in lists of a given size range')
axis([0 6 0 18e6])

xticks([1 2 3 4 5])
xticklabels({'<100','<1000','<10000','<100000','100000+'})

%## cumulative length of all lists: 41205930
%## 3076857
%## 7461238
%## 17443866
%## 12949474
%## 274495
