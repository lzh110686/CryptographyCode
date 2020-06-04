from libnum import n2s,s2n
from gmpy2 import invert

def egcd(a, b):
	if a == 0:
		return (b, 0, 1)
	else:
		g, y, x = egcd(b % a, a)
		return (g, x - (b // a) * y, y)

def main():
    n = int('a5f7f8aaa82921f70aad9ece4eb77b62112f51ac2be75910b3137a28d22d7ef3be3d734dabb9d853221f1a17b1afb956a50236a7e858569cdfec3edf350e1f88ad13c1efdd1e98b151ce2a207e5d8b6ab31c2b66e6114b1d5384c5fa0aad92cc079965d4127339847477877d0a057335e2a761562d2d56f1bebb21374b729743',16)
    c1 = int('6fdcbfb5cd2cacd032ef7200fd49b9f304a6dbd8399f4a91a72d1d9150f97b3b513f44dfc56f6f7c8ec41a8ef9b93a80230a1e65e29d2ef519bb83931d4b0c7a589059cfdf2d571660ab790a9c7e085e3018bf19748abd6d521952b68bc9594c1ad34726658bd9bd445d3b6381ceee57328838e8a129867e505be0ca0d1a1da5',16)
    c2 = int('8caeaa7d272f9606fee9222efd1d922143db738b95bd64746b27bc4c0fd979a2c57b4735131a4391a81bf5f0c0c8eea41d4f91bed4d17784b1956fd89882b97c98009051ac3a03964499c864524d3ddc10299c0290e91707b62ce89b118afe558151be39d61de0483def52c6cb546132ecab85143715bc593a2892b1e41b37b9',16)
    e1 = int('6b8a5ae7',16)
    e2 = int('4042c3955',16)
    s = egcd(e1, e2)
    s1 = s[1]
    s2 = s[2]
    # 求模反元素
    if s1<0:
        s1 = - s1
        c1 = invert(c1, n)
    elif s2<0:
        s2 = - s2
        c2 = invert(c2, n)

    m = pow(c1,s1,n)*pow(c2,s2,n) % n
    print (n2s(m))

if __name__ == '__main__':
  main()