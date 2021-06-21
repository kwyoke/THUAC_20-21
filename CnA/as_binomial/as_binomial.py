import sys
from math import factorial as fact

if __name__ == '__main__':
    a, b = sys.stdin.readline().split(' ')
    a = int(a)
    b = int(b)
    if b > a-b:
    	b = a-b

    out = fact(a)//fact(b)//fact(a-b)
    ans = out%(10**9+7)
    print(ans)