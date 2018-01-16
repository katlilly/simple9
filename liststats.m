stats = csvread('stats_longer_lists.csv');
number = stats(:, 1);
length = stats(:, 2);
mean = stats(:, 3);
stdev = stats(:, 4);
mode = stats(:, 5);
lowexcp= stats(:, 6);
highexcp= stats(:, 7);
modFrac= stats(:, 8);
lowFrac= stats(:, 9);
highFrac= stats(:, 10);
perms= stats(:, 11);

plot(mean, stdev, 'b.', mean, mode, 'r.');
legend('stdev','mode')
xlabel('mean')
title('all lists with length > 1000')

plot(length, perms, 'g.')
title('permutations by list length')
xlabel('list length')
ylabel('permutations of one chosen combination')
axis([0 45000 0 5000])
