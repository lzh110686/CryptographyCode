import base64
#加密
s = '66'
a = base64.b64encode(s.encode('utf-8'))
print(a)
#解密
print (base64.b64decode(a))