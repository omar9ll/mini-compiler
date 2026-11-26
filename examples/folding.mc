# Everything here is known at compile time, so the optimizer
# folds it down to a handful of constants.
let seconds_per_day = 60 * 60 * 24;
let days = 7;
let week = seconds_per_day * days;
print week;              # 604800
print (2 + 3) * (10 - 4) * 1 + 0;   # 30
