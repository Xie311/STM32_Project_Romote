#!/usr/bin/env python
# -*- coding:utf-8 -*-
# Author:X311

#print(*objects, sep=' ', end='\n', file=sys.stdout)

#print(eval(input('Enter a number: '))**2)

# & | ^ ~ << >>
print(round(2.35,1))
print("七章一章没看"[2:])
a,b,c,d,e,f='winter'
print(a,b,c,d,e,f)
print('想学就想')
a=52.1
b=52
if a>b:max=a
print(max,type(max))
flag='def'
result='abc'if flag else 'def'
print(result)

score='A'
match score:
    case 'A':
        print('excellent')

for i in 'winter':
    print(i,end='')

sum1=0
for i in range(1,11): #前闭后开
    sum1+=i
print(sum1)

pass

s='Hello'
s1=s[::-1]
print(s1*3)

print('e' in s)
print('e' not in s)
print(len(s),end='')
print(s.count('l'),end='')
print(s.index('l'))
print(min(s))

