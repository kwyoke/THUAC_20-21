import sys
from math import factorial as fact

if __name__ == '__main__':
    x1, y1, x2, y2 = sys.stdin.readline().split(' ')
    x1 = int(x1)
    y1 = int(y1)
    x2 = int(x2)
    y2 = int(y2)

    
    if b > a-b:
    	b = a-b

    out = fact(a)//fact(b)//fact(a-b)
    ans = out%(10**9+7)
    print(ans)