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
if a>b:max1=a
print(max1,type(max1))
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

lst=[1,'hello','world']
print(lst)
lst2=list('HELLOWORLD')
print(lst2)
lst3=(list(range(1,11,3)))
print(lst3)
print(lst+lst2+lst3)
print(max(lst3))
lst4=[10,20,30]
del lst4

#使用遍历循环for遍历列表元素
for item in lst:
    print(item,end='')
#使用for循环，range（）函数，len（）函数，根据索引进行遍历
for i in range(0,len(lst)):
    print(lst[i],end='')
print('\n')
#使用enumearte函数进行遍历
for index,item in enumerate(lst):
    print(index,item) #index是序号，不是索引
#手动修改序号索引值
for index,item in enumerate(lst,start=1):
    print(index,item)
for index, item in enumerate(lst,1):
        print(index, item)

print('原列表:',lst,id(lst))
lst.append('abc')
print('增加元素之后:',lst,id(lst)) #元素可变但内存地址不变，所以列表是可变数据类型
lst.insert(1,100)
print(lst)
lst.remove(100)
print('移除元素后：',lst)
print(lst.pop(1))
print(lst)
# lst.clear()
# print(lst)
lst.reverse()
print('反转后：',lst)
new_lst=lst.copy()
print('复制后的新列表：',new_lst)
lst[1]='my'
print('改变元素后：',lst)
lst=[3,53,78,4,98,11,9]

lst.sort()
print('升序',lst)
lst.sort(reverse=True)
print('降序',lst)
#忽略大小写进行比较
lst.sort(key=str.lower)  #注意str.lower后不加括号（是参数）
sorted_lst=sorted(lst)


