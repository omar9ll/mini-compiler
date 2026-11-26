# Values that depend on earlier statements are still computed correctly.
let a = 3;
let b = a * a + 1;      # 10
let a = b - a;          # 7  (reassignment)
print a * 2 + b;        # 24
print a / 2;            # 3.5
