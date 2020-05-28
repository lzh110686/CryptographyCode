
# -*- coding: cp936 -*-


def isPrime(number):
    import math
    i = 2
    sqrtnum = (int)(math.sqrt(number))
    for i in range(2, sqrtnum + 1):
        if number % i == 0:
            return False
        i = i + 1
    return True


def is_ET_Prime(ee, tt):
    while tt != 0:
        a = ee
        ee = tt
        tt = a % tt
    if ee == 1:
        return True
    else:
        return False


def get_publickey(k, t):
    d = 0
    while ((d * k) % t != 1):
        d += 1
    return d


def encryption(plain, d, n):
    re = (plain ** d) % n

    return re


if __name__ == "__main__":
    print
    "~" * 70
    Flag = False
    while True:
        p = int(input("please input a prime p:"))
        q = int(input("please input a prime q:"))

        if (isPrime(p) and isPrime(q)):
            break
        else:
            print
            "p or q is not prime!"
            continue

    print
    "p=", p, "q=", q

    n = q * p
    t = (q - 1) * (p - 1)
    print("n=", n, "t=", t)

    print("~" * 70)

    Flag == False
    while Flag == False:
        e = int(input("please input a private key:"))
        Flag = is_ET_Prime(e, t)
        if Flag == False:
            print("e is not prime with the t!")

    print("the private key e=", e)

    d = get_publickey(e, t)
    print("the public key d=", d)

    plain = int(ord(input("please input the plain you want to entrypted:")))

    encry = encryption(plain, d, n)
    print("plain", plain, "is encrypted as", encry)
    #print(encry)
    plain1 = encryption(encry, e, n)
    print("encrypt", encry, "is decrypted as", plain1)
