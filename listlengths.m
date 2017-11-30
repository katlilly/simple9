lengths = (1:5)
totalitems = ([3076857 7461238 17443866 12949474 274495])
check = sum(totalitems)
cumulative = 41205930
%error = cumulative - check

plot(lengths, totalitems, 'bo')
title(Total items in lists of a given size range)
ylable()
