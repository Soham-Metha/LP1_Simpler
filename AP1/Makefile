
test_all: clean all
	./a.out < tc1 > tc1_out
	diff tc1_out tc1_expected --suppress-common-lines -y --ignore-all-space && echo "\nTC1 PASSED\n\n"
	./a.out < tc2 > tc2_out
	diff tc2_out tc2_expected --suppress-common-lines -y --ignore-all-space && echo "\nTC2 PASSED\n\n"
	./a.out < tc3 > tc3_out
	diff tc3_out tc3_expected --suppress-common-lines -y --ignore-all-space && echo "\nTC3 PASSED\n\n"

all:
	g++ ./assembler_pass_1.cpp -Wall -Wextra

clean:
	rm ./a.out