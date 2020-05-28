#!/usr/bin/python
# -*- coding=utf -*-
import re

table ={'a': 'aaaaa', 'b': 'aaaab', 'c': 'aaaba', 'd': 'aaabb', 'e': 'aabaa', 'f': 'aabab', 'g': 'aabba',
        'h': 'aabbb', 'i': 'abaaa', 'j': 'abaab', 'k': 'ababa', 'l': 'ababb', 'm': 'abbaa', 'n': 'abbab',
        'o': 'abbba', 'p': 'abbbb', 'q': 'baaaa', 'r': 'baaab', 's': 'baaba', 't': 'baabb', 'u': 'babaa',
        'v': 'babab', 'w': 'babba', 'x': 'babbb', 'y': 'bbaaa', 'z': 'bbaab'}
 
 
def bacon(cipher):
    msg = ''
    codes = re.findall(r'.{5}', cipher)
    for code in codes:
        if code == '':
            msg += ' '
        else:
            UNCODE = dict(map(lambda t: (t[1], t[0]), table.items()))
            msg += UNCODE[code]
    return msg
 
 
if __name__ == '__main__':
    cipher = 'aaaaabaabbbaabbaaaaaaaabaababaaaaaaabbabaaabbaaabbaabaaaababaabaaabbabaaabaaabaababbaabbbabaaabababbaaabbabaaabaabaabaaaabbabbaabbaabaabaaabaabaabaababaabbabaaaabbabaabba'
    plaintext = bacon(cipher)
    print(plaintext)