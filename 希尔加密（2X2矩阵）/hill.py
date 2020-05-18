str1=input("请输入加密字符串： ")
s=str1.replace(' ', '')
key=input("请输入key：")
table1="abcdefghijklmnopqrstuvwxyz"
def StrtoInt(s):
    for x in range(26):
        i=x+1
        if (ord(s)-97)==i:
            return i+1;
        elif s=='a':
            return i;
s1=""
for i in range(len(s)):
    newint=i+1
    if(newint%2==0):
        m=(StrtoInt(s[newint-2:newint-1])*int(key[0:1])+StrtoInt(s[newint-1:newint])*int(key[1:2]))%26
        s1+=table1[m-1:m]
        n=(StrtoInt(s[newint - 2:newint - 1]) * int(key[2:3]) + StrtoInt(s[newint - 1:newint]) * int(key[3:4])) % 26
        s1+=table1[n-1:n]
print(s1)