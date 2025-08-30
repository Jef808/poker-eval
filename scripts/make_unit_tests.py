#!/usr/bin/env python3

#!/usr/bin/env python3

import re

with open("hand_values.txt") as f:
    content = f.read()


lines = [l.strip() for l in content.split('\n') if l.strip()]

# For creating unit tests
unit_test_lines = []

for line in lines:
    parts = re.split(r' {3,}', line)
    hand_type = parts[6].strip()
    unit_test_lines.append([parts[5], parts[0], hand_type])


with open("unit_test_hand_values.txt", "w+") as f:
    for utl in unit_test_lines:
        hand = utl[0]
        value = utl[1]
        hand_type = utl[2]

        if hand_type in ['SF', 'F']:
            hand = hand.replace(' ', 'h ') + 'h'
        else:
            hand = hand[0] + 'c ' + hand[2] + 'd ' + hand[4] + 's ' + hand[6] + 'h ' + hand[8] + 'c'

        f.write(f'{hand}   {value}\n')


with (
        open("unit_test_hand_values.txt", "r") as f,
        open("test_cases.cpp", "w+") as f_test
):
    unit_test_lines = f.readlines()
    f_test.write("""#define CATCH_CONFIG_MAIN
#if __has_include(<catch2/catch_all.hpp>)
  #include <catch2/catch_all.hpp>
#else
  #include <catch2/catch.hpp>
#endif

#include "types.h"
#include "utils.h"

TEST_CASE("evaluate produces same values as hand_values.txt source", "[evaluate]") {
""")
    for line in unit_test_lines:
        hand, value = line.split('   ')
        f_test.write(f"  REQUIRE(evaluate(\"{hand.strip()}\") == {value.strip()});\n")
    f_test.write("}\n")
