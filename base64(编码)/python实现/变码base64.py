import base64
table_tmp = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'N', 'M','O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b','c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p','q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '9', '1', '2', '3','4', '5', '6', '7', '8', '0', '+', '/']
table_original = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
table_tmp_str = ''
str1 = 'eG1hbntmYWM9NmJhczM0'   #待解密字符串
for i in table_tmp:
	table_tmp_str += i
print(table_tmp_str)
print(base64.b64decode(str1.translate(str.maketrans(table_tmp_str,table_original)).encode('utf-8')))