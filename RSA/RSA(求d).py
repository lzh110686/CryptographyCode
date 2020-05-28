#!/usr/bin/python
# -*- coding=utf -*-
 
 
def exgcd(a, n):
    if n == 0:
        return 1, 0
    else:
        x, y = exgcd(n, a % n)
        x, y = y, x - (a // n) * y
        return x, y
 
 
def getReverse(a, n):
    re, y = exgcd(a, n)
    while(re < 0):
        re += n
    return re
 
 
if __name__ == "__main__":
    p = int(input("请输入p:"))
    q = int(input("请输入q:"))
    e = int(input("请输入e:"))
    d = getReverse(e, (p - 1)*(q - 1))
    print('d = ' + str(d))