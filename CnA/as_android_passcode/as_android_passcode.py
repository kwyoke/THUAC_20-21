import sys
from itertools import permutations


invalid_dict = {
    	1: [3,7,9],
    	2: [8],
    	3: [1,7,9],
    	4: [6],
    	5: [],
    	6: [4],
    	7: [1,3,9],
    	8: [2],
    	9: [3,1,7]
    }

midnum_dict = {
 		(1, 3): 2,
 		(1, 7): 4,
 		(1, 9): 5,
 		(2, 8): 5,
 		(3, 1): 2,
 		(3, 7): 5,
 		(3, 9): 6,
 		(4, 6): 5,
 		(6, 4): 5,
 		(7, 1): 4,
 		(7, 3): 5,
 		(7, 9): 8,
 		(8, 2): 5,
 		(9, 3): 6,
 		(9, 1): 5,
 		(9, 7): 8
 }

all_digits = [1,2,3,4,5,6,7,8,9]

def check_valid_code(code):
	used_dig = []
	for i in range(len(code) - 1):
		used_dig.append(code[i])
		if code[i+1] in invalid_dict[code[i]]:
			if midnum_dict[(code[i], code[i+1])] not in used_dig:
				return False
	return True

def num_possib_codes(a,b, l):
	count = 0

	digits_free = all_digits.copy()
	digits_free.remove(a)
	digits_free.remove(b)

	all_codes = list(permutations(digits_free, l-2))

	for code in all_codes:
		full_code = [a]
		for i in list(code):
			full_code.append(i)
		full_code.append(b)

		if check_valid_code(full_code):
			count += 1

	return count


if __name__ == '__main__':
    a, b, l = sys.stdin.readline().split(' ')
    a = int(a)
    b = int(b)
    l = int(l)

    if (l<2):
    	ans = 0
    else: 
    	ans = num_possib_codes(a,b,l)


    print(ans)