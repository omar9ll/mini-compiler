#!/usr/bin/env bash
# Runs each example with and without the optimizer and checks the output.
set -u
cd "$(dirname "$0")/.."
pass=0; fail=0

check() {  # name, expected output, command...
    local name="$1" expected="$2"; shift 2
    local got
    got="$("$@" 2>&1)"
    if [[ "$got" == "$expected" ]]; then pass=$((pass+1)); echo "PASS  $name"
    else fail=$((fail+1)); echo "FAIL  $name"; echo "  expected: $expected"; echo "  got:      $got"; fi
}

for opt in "" "--no-opt"; do
    tag="${opt:-opt}"
    check "basics ($tag)"  $'60\n34\n-14\n2'        ./minic $opt examples/basics.mc
    check "folding ($tag)" $'604800\n30'             ./minic $opt examples/folding.mc
    check "runtime ($tag)" $'24\n3.5'                ./minic $opt examples/runtime.mc
done

tmp=$(mktemp -d)
printf 'print x;\n' > "$tmp/undef.mc"
check "undefined variable" "compile error: line 1: variable 'x' used before it was defined" ./minic "$tmp/undef.mc"
printf 'let x = 0;\nlet y = 4;\nprint y / (x * 1);\n' > "$tmp/div0.mc"
check "division by zero"  "runtime error: line 3: division by zero" ./minic --no-opt "$tmp/div0.mc"
printf 'let = 5;\n' > "$tmp/syntax.mc"
check "syntax error" "compile error: line 1: expected a variable name but found '='" ./minic "$tmp/syntax.mc"
printf 'print 2 * x + 0;\n' > "$tmp/fold.mc"; printf 'let x = 3;\n' | cat - "$tmp/fold.mc" > "$tmp/fold2.mc"
check "constant propagation" $'0000  CONST  3\n0001  STORE  x\n0002  CONST  6\n0003  PRINT \n0004  HALT  ' ./minic --dump "$tmp/fold2.mc"
rm -rf "$tmp"

echo; echo "$pass passed, $fail failed"
[[ $fail -eq 0 ]]
