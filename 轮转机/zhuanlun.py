import re
sss='1:  <ZWAXJGDLUBVIQHKYPNTCRMOSFE<2:  <KPBELNACZDTRXMJQOYHGVSFUWI<3:  <BDMAIZVRNSJUWFHTEQGYXPLOCK<4:  <RPLNDVHGFCUKTEBSXQYIZMJWAO<5:  <IHFRLABEUOTSGJVDKCPMNZQWXY<6:  <AMKGHIWPNYCJBFZDRUSLOQXVET<7:  <GWTHSPYBXIZULVKMRAFDCEONJQ<8:  <NOZUTWDCVRJLXKISEFAPMYGHBQ<9:  <XPLTDSRFHENYVUBMCQWAOIKZGJ<10: <UDNAJFBOWTGVRSCZQKELMXYIHP<11： <MNBVCXZQWERTPOIUYALSKDJFHG<12： <LVNCMXZPQOWEIURYTASBKJDFHG<13： <JZQAWSXCDERFVBGTYHNUMKILOP<'
m='NFQKSEVOQOFNP'
#将sss转化成列表,正则表达式，re.S在此模式下"."的匹配不受限制，可匹配所有字符
content=re.findall(r'<(.*?)<',sss,re.S)
iv=[2,3,7,5,13,12,9,1,8,10,4,11,6]
print(content)
lll=[]
for i in range(13):
    index=content[iv[i]-1].index(m[i])
    lll.append(index)
print(lll)
for i in range(0,26):
    flag=""
    for j in range(13):
        flag+=content[iv[j]-1][(lll[j]+i)%26]
    print(flag.lower())