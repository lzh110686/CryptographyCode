# define  _CRT_SECURE_NO_WARNINGS
# include "stdio.h"
# include "stdlib.h"
# include "string.h"
# include "DES_Table.h"

///////////////////////////////////////////////////////////////
//功能:	打印16进制字符串
//参数:	BitGroup			输入的16进制字符串
//		nBit				输入的打印个数
//返回值:void
void Print_Table(const unsigned char* BitGroup, int nBit)
{
	for (int i = 0; i < nBit; i++)
	{
		printf("%X", BitGroup[i]);
	}
	printf("\n");
}


///////////////////////////////////////////////////////////////
//功能:	根据Byte字符串组获取其对应的Bit组   0x34 --> 00110100
//参数:	Byte				输入的字符串组
//		Bit					输出的对应的Bit组
//返回值:返回0成功	
int ByteToBit(const unsigned char *Byte, unsigned char *Bit)
{
	int ret = 0;

	for(int i = 0; i < 8; i ++)
	{
		for (int j = 0; j < 8; j++)
		{
			Bit[i * 8 + j] = Byte[i] >> (7 - j) & 1;
		}
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	根据Bit组获取其对应的字符串  00110100 --> 0x34
//参数:	Bit				输入的Bit组
//		Byte			输出的字符串组
//返回值:返回0成功	
int BitToByte(const unsigned char *Bit, unsigned char *Byte)
{
	int ret = 0;
	
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			Byte[i] |= (Bit[i * 8 + j] & 1) << (7-j);
		}
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	根据Bit组获取其对应的字符串  0011 --> 0x03
//参数:	Bit_reIP_Table	输入的密文Bit组
//		CipherText		输出的16进制字符串
//返回值:返回0成功	
int BitToHex(const unsigned char *Bit_reIP_Table, unsigned char *CipherText)
{
	int ret = 0;

	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			CipherText[i] |= Bit_reIP_Table[(i << 2) + j] << (3 - j);
		}
	}

	return ret;
}


///////////////////////////////////////////////////////////////
//功能:	根据Bit组获取其对应的字符串  0x03 -->> 0011
//参数:	CipherText		输入的16进制字符串
//		BitGroup		输出的密文Bit组
//返回值:返回0成功	
int HexToBit(const unsigned char *CipherText, unsigned char *BitGroup)
{
	int ret = 0;

	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			BitGroup[4 * i + j] = CipherText[i] >> (3 - j) & 1;
		}
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	进行PC-1初始密钥置换 64 ---> 56
//参数:	BitKey			输入的密钥Bit组
//		BitKey_PC_1		输出的置换结果
//返回值:返回0成功	
int PC_1_Substitution(const unsigned char *BitKey, unsigned char *BitKey_PC_1)
{
	int ret = 0;

	for (int i = 0; i < 56; i++)
	{
		BitKey_PC_1[i] = BitKey[PC_1_Table[i] - 1];
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	进行PC-2密钥置换 56 ---> 48
//参数:	BitKey			输入的Bit组
//		SubKey			输出的子密钥
//返回值:返回0成功	
int PC_2_Substitution(const unsigned char *BitKey, unsigned char *SubKey)
{
	int ret = 0;

	for (int i = 0; i < 48; i++)
	{
		SubKey[i] = BitKey[PC_2_Table[i] - 1];
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	将SrcBitGroup左移nBit到DesBitGroup中
//参数:	SrcBitGroup		输入的轮转前的结果
//		DesBitGroup		输出的轮转后的结果
//		nBit			左移的位数
//返回值:返回0成功	
int BitRound_L(const unsigned char* SrcBitGroup, unsigned char* DesBitGroup, int nBit)
{
	int ret = 0;

	memcpy(DesBitGroup,				&SrcBitGroup[nBit], 28 - nBit);
	memcpy(&DesBitGroup[28 - nBit], SrcBitGroup,		nBit);

	return ret;
}


///////////////////////////////////////////////////////////////
//功能:	计算BitKey对应的16个子密钥
//参数:	BitKey			输入的密钥Bit组
//		BitSubKey		输出的16组子密钥  16*48
//返回值:返回0成功	
int CalculateSubKey(const unsigned char *BitKey, unsigned char(*BitSubKey)[48])
{
	int ret = 0;

	//1、定义变量
	unsigned char BitKey_PC_1[56];		//56位PC-1置换后的密钥
	unsigned char BitC_Table[17][28];	//17组28位C表
	unsigned char BitD_Table[17][28];	//17组28位D表
	unsigned char BitCD_Table[56];		//合并的CD表

	//2、变量初始化
	memset(BitKey_PC_1, 0, sizeof(BitKey_PC_1));
	memset(BitC_Table,	0, sizeof(BitC_Table));
	memset(BitD_Table,	0, sizeof(BitD_Table));
	memset(BitCD_Table, 0, sizeof(BitCD_Table));

	//3、进行PC-1初始密钥置换	64位 ---> 56位
	PC_1_Substitution(BitKey, BitKey_PC_1);

	//4、将置换后的结果分成C、D两组
	memcpy(BitC_Table[0], BitKey_PC_1,		28);
	memcpy(BitD_Table[0], &BitKey_PC_1[28], 28);

	//5、循环处理 循环16次
	for (int i = 0; i < 16; i++)
	{
		//(1)、将C、D两组进行轮转移位操作	左移
		BitRound_L(BitC_Table[i], BitC_Table[i + 1], Bit_Round[i]);
		BitRound_L(BitD_Table[i], BitD_Table[i + 1], Bit_Round[i]);

		//(2)、将轮转后的结果合并
		memcpy(BitCD_Table,			BitC_Table[i + 1], 28);
		memcpy(&BitCD_Table[28],	BitD_Table[i + 1], 28);

		//(3)、进行PC-2密钥置换 得到其中一个子密钥 56位 ---> 48位
		PC_2_Substitution(BitCD_Table, BitSubKey[i]);
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	对明文Bit组进行初始IP置换
//参数:	BitPlain		输入的明文Bit组
//		Bit_IP_Table	输出的IP置换后结果
//返回值:返回0成功	
int IP_Substitution(const unsigned char* BitPlain, unsigned char* Bit_IP_Table)
{
	int ret = 0;

	for (int i = 0; i < 64; i++)
	{
		Bit_IP_Table[i] = BitPlain[IP_Table[i] - 1];
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	对BitR_Table Bit组进行扩展置换  32 --->> 48
//参数:	BitR_Table		输入的R组 Bit组
//		BitE_Table		输出的扩展置换后结果
//返回值:返回0成功	
int E_Substitution(const unsigned char* BitR_Table, unsigned char* BitE_Table)
{
	int ret = 0;

	for (int i = 0; i < 48; i++)
	{
		BitE_Table[i] = BitR_Table[E_Table[i] - 1];
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	进行异或运算
//参数:	Bit1_Table		输入的第一个 Bit组
//		Bit2_Table		输入的第二个 Bit组
//		Bit_Xor			输出的异或运算的结果
//		nBit			输入的异或运算的位数
//返回值:返回0成功	
int DES_XOR(const unsigned char* Bit1_Table, const unsigned char* Bit2_Table, unsigned char* Bit_Xor/*异或运算的结果*/, int nBit/*异或运算的位数*/)
{
	int ret = 0;
	for (int i = 0; i < nBit; i++)
	{
		Bit_Xor[i] = Bit1_Table[i] ^ Bit2_Table[i];
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	对Bit_Integer Bit组进行P置换  
//参数:	Bit_Integer		输入的S盒交换后的Bit组
//		BitP_Table		输出的P置换后结果
//返回值:返回0成功	
int P_Substitution(const unsigned char *Bit_Integer, unsigned char* BitP_Table)
{
	int ret = 0;

	for (int i = 0; i < 32; i++)
	{
		BitP_Table[i] = Bit_Integer[P_Table[i] - 1];
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	对数据进行F函数处理
//参数:	BitR_Table		输入的R Bit组
//		SubKey			输入的子密钥 Bit组
//		BitP_Table		输出的P表置换的结果(F函数最终处理结果)
//返回值:返回0成功	
int DES_F_Function(const unsigned char* BitR_Table, const unsigned char* SubKey, unsigned char* BitP_Table/*输出P表置换的结果*/)
{
	int ret = 0;

	//1、定义变量
	unsigned char BitE_Table[48];		//E(R)扩展置换结果
	unsigned char Bit_Xor[8][6];		//存放异或运算的结果
	unsigned char Row;					//S盒的行号
	unsigned char Col;					//S盒的列号
	unsigned char Integer;				//从S盒中取得的32位整数
	unsigned char Bit_Integer[8][4];	//将整数变成Bit位

	//2、变量初始化
	memset(BitE_Table, 0, sizeof(BitE_Table));
	memset(Bit_Xor, 0, sizeof(Bit_Xor));
	Row = 0;
	Col = 0;
	Integer = 0;
	memset(Bit_Integer, 0, sizeof(Bit_Integer));

	//3、进行扩展置换		32 -->> 48
	E_Substitution(BitR_Table, BitE_Table);

	//4、将置换后的结果与传入的子密钥进行异或运算
	DES_XOR(BitE_Table, SubKey, (unsigned char*)Bit_Xor, 48);

	//5、将运算后的结果进行8个S盒压缩置换
	for (int i = 0; i < 8; i++)
	{
		//(1)、计算S盒的行号和列号
		Row = (Bit_Xor[i][0] << 1) + Bit_Xor[i][5];
		Col = (Bit_Xor[i][1] << 3) + (Bit_Xor[i][2] << 2) + (Bit_Xor[i][3] << 1) + Bit_Xor[i][4];

		//(2)、从S盒中取得整数
		Integer = S_Table[i][Row][Col];
		
		//(3)、将取得的4Bit数转换成Bit组
		for (int j = 0; j < 4; j++)
		{
			Bit_Integer[i][j] = Integer >> (3 - j) & 1;
		}
	}

	//6、进行P置换
	P_Substitution((unsigned char *)Bit_Integer, BitP_Table);

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	对L、R交叉合并后的结果进行逆初始值置换
//参数:	BitRL_Table		输入的Bit组
//		Bit_reIP_Table	输出的逆初始值置换的结果
//返回值:返回0成功	
int reIP_Substitution(const unsigned char *BitRL_Table, unsigned char *Bit_reIP_Table)
{
	int ret = 0;

	for (int i = 0; i < 64; i++)
	{
		Bit_reIP_Table[i] = BitRL_Table[reIP_Table[i] - 1];
	}

	return ret;
}




///////////////////////////////////////////////////////////////
//功能:	将字符串进行DES加密
//参数:	PlainText		输入的明文字符串
//		PlainText		输入的密钥字符串
//		CipherText		输出的密文
//返回值:返回0成功	
int DES_EnCryption(unsigned char *PlainText, unsigned char *PassWord, unsigned char *CipherText)
{
	int ret = 0;

	//1、定义变量
	unsigned char BitPlain[64];			//明文Bit组
	unsigned char BitKey[64];		    //密钥Bit组
	unsigned char BitSubKey[16][48];	//子密钥Bit组
	unsigned char Bit_IP_Table[64];		//初始置换后的明文表
	unsigned char BitL_Table[17][32];	//L表Bit组
	unsigned char BitR_Table[17][32];	//R表Bit组
	unsigned char BitRL_Table[64];		//交换合并后的LR组
	unsigned char Bit_F_Out[32];		//F函数计算出的结果Bit组
	unsigned char Bit_reIP_Table[64];	//逆初始置换后的密文表

	//2、变量初始化
	memset(BitPlain,		0, sizeof(BitPlain));
	memset(BitKey,			0, sizeof(BitKey));
	memset(BitSubKey,		0, sizeof(BitSubKey));
	memset(Bit_IP_Table,	0, sizeof(Bit_IP_Table));
	memset(BitL_Table,		0, sizeof(BitL_Table));
	memset(BitR_Table,		0, sizeof(BitR_Table));
	memset(BitRL_Table,		0, sizeof(BitRL_Table));
	memset(Bit_F_Out,		0, sizeof(Bit_F_Out));
	memset(Bit_reIP_Table,	0, sizeof(Bit_reIP_Table));

	//3、将字符串转换成Bit组
	ByteToBit(PlainText, BitPlain);
	ByteToBit(PassWord , BitKey);

	//4、计算Password Bit组组成的16个子密钥
	CalculateSubKey(BitKey, BitSubKey);

	//5、对明文Bit组进行初始IP置换
	IP_Substitution(BitPlain, Bit_IP_Table);

	//6、将置换后的结果分位 L、R两组		暂且前后分组
	memcpy(BitL_Table[0], Bit_IP_Table,			32);
	memcpy(BitR_Table[0], &Bit_IP_Table[32],	32);

	//7、16轮的循环处理
	for (int i = 0; i < 16; i++)
	{
		//(1)、将R组和子密钥组进行F函数运算
		DES_F_Function(BitR_Table[i], BitSubKey[i], Bit_F_Out);

		//(2)、L组盒F函数的输出结果进行异或运算
		DES_XOR(BitL_Table[i], Bit_F_Out, BitR_Table[i + 1], 32);

		//(3)、Li+1 = Ri
		memcpy(BitL_Table[i + 1], BitR_Table[i], 32);
	}

	//8、L[16]和R[16]进行交叉合并
	memcpy(BitRL_Table,			BitR_Table[16], 32);
	memcpy(&BitRL_Table[32],	BitL_Table[16], 32);

	//9、将交叉合并后的结果进行逆初始值置换	生成密文Bit组
	reIP_Substitution(BitRL_Table, Bit_reIP_Table);

	//10、将密文Bit组转换成密文16进制字符串
	BitToHex(Bit_reIP_Table, CipherText);

	return ret;
}


///////////////////////////////////////////////////////////////
//功能:	将SrcBitGroup右移nBit到DesBitGroup中
//参数:	SrcBitGroup		输入的轮转前的结果
//		DesBitGroup		输出的轮转后的结果
//		nBit			左移的位数
//返回值:返回0成功	
int BitRound_R(const unsigned char* SrcBitGroup, unsigned char* DesBitGroup, int nBit)
{
	memcpy(&DesBitGroup[nBit],	SrcBitGroup,				(28 - nBit));
	memcpy(DesBitGroup,			&SrcBitGroup[28 - nBit],	nBit);

	return 0;
}


///////////////////////////////////////////////////////////////
//功能:	计算BitKey对应的16个逆子密钥----用于解密	
//参数:	BitKey			输入的密钥Bit组
//		BitSubKey		输出的16组逆子密钥  16*48
//返回值:返回0成功	
int CalculateReSubKey(const unsigned char *BitKey, unsigned char(*BitReSubKey)[48])
{
	int ret = 0;

	//1、定义变量
	unsigned char BitKey_PC_1[56];		//56位PC-1置换后的密钥
	unsigned char BitC_Table[16][28];	//16组28位C表	  由于第一组不需要移动，所以比加密时少了一组
	unsigned char BitD_Table[16][28];	//16组28位D表	  由于第一组不需要移动，所以比加密时少了一组
	unsigned char BitCD_Table[56];		//合并的CD表

	//2、变量初始化
	memset(BitKey_PC_1,	0, sizeof(BitKey_PC_1));
	memset(BitC_Table,	0, sizeof(BitC_Table));
	memset(BitD_Table,	0, sizeof(BitD_Table));
	memset(BitCD_Table, 0, sizeof(BitCD_Table));

	//3、进行PC-1初始密钥置换
	PC_1_Substitution(BitKey, BitKey_PC_1);

	//4、将置换后的结果分成C、D两组
	memcpy(BitC_Table[0], BitKey_PC_1,		28);
	memcpy(BitD_Table[0], &BitKey_PC_1[28], 28);

	//5、在不进行轮转的情况下直接将PC-1的结果进行PC-2置换，生成第一组逆子密钥
	PC_2_Substitution(BitKey_PC_1, BitReSubKey[0]);

	//6、进行15轮循环处理
	for (int i = 0; i < 15; i++)
	{
		//(1)、将C、D两组进行右移操作		形成与正向子密钥内容相同顺序相反的密钥
		BitRound_R(BitC_Table[i], BitC_Table[i + 1], Bit_Round[16 - (i + 1)]);
		BitRound_R(BitD_Table[i], BitD_Table[i + 1], Bit_Round[16 - (i + 1)]);


		//(2)、将轮转后的结果合并
		memcpy(BitCD_Table,			BitC_Table[i + 1], 28);
		memcpy(BitCD_Table + 28,	BitD_Table[i + 1], 28);

		//(3)、将合并后的Bit组进行PC-2置换
		PC_2_Substitution(BitCD_Table, BitReSubKey[i + 1]);
	}

	return ret;
}


///////////////////////////////////////////////////////////////
//功能:	将字符串进行DES解密
//参数:	CipherText		输入的密文字符串
//		PlainText		输入的密钥字符串
//		DeCipherText	输出的明文 
//返回值:返回0成功	
int DES_DeCipher(const unsigned char *CipherText, const unsigned char* PassWord, unsigned char *DeCipherText)
{
	int ret = 0;

	//1、定义变量
	unsigned char BitCipher[64];		//待解密的密文Bit组
	unsigned char BitKey[64];			//密钥的Bit组
	unsigned char BitReSubKey[16][48];	//逆子密钥组
	unsigned char Bit_IP_Table[64];		//初始置换后的密文表
	unsigned char BitL_Table[17][32];	//L表Bit组
	unsigned char BitR_Table[17][32];	//R表Bit组
	unsigned char BitRL_Table[64];		//交换合并后的LR组
	unsigned char Bit_F_Out[32];		//F函数计算出的结果Bit组
	unsigned char Bit_reIP_Table[64];	//逆初始置换后的明文表

	//2、变量初始化
	memset(BitCipher,		0, sizeof(BitCipher));
	memset(BitKey,			0, sizeof(BitKey));
	memset(BitReSubKey,		0, sizeof(BitReSubKey));
	memset(Bit_IP_Table,	0, sizeof(Bit_IP_Table));
	memset(BitL_Table,		0, sizeof(BitL_Table));
	memset(BitR_Table,		0, sizeof(BitR_Table));
	memset(BitRL_Table,		0, sizeof(BitRL_Table));
	memset(Bit_F_Out,		0, sizeof(Bit_F_Out));
	memset(Bit_reIP_Table,	0, sizeof(Bit_reIP_Table));

	//3、将密文和密钥转换成Bit组
	HexToBit(CipherText, BitCipher);
	ByteToBit(PassWord,  BitKey);

	//4、生成16组逆子密钥	--->>也就是将加密密钥的顺序反过来，为了保持解密格式一致
	CalculateReSubKey(BitKey, BitReSubKey);	//可能有问题

	//5、对密文Bit组进行初始IP置换
	IP_Substitution(BitCipher, Bit_IP_Table);

	//6、将置换后的结果分位 L、R两组		暂且前后分组
	memcpy(BitL_Table[0], Bit_IP_Table,		 32);
	memcpy(BitR_Table[0], &Bit_IP_Table[32], 32);

	//7、16轮的循环处理
	for (int i = 0; i < 16; i++)
	{
		//(1)、将R组和逆子密钥组进行F函数运算
		DES_F_Function(BitR_Table[i], BitReSubKey[i], Bit_F_Out);

		//(2)、L组盒F函数的输出结果进行异或运算
		DES_XOR(BitL_Table[i], Bit_F_Out, BitR_Table[i + 1], 32);

		//(3)、Li+1 = Ri
		memcpy(BitL_Table[i + 1], BitR_Table[i], 32);
	}

	//8、L[16]和R[16]进行交叉合并
	memcpy(BitRL_Table, BitR_Table[16], 32);
	memcpy(&BitRL_Table[32], BitL_Table[16], 32);

	//9、将交叉合并后的结果进行逆初始值置换	生成明文Bit组
	reIP_Substitution(BitRL_Table, Bit_reIP_Table);

	//10、将明文Bit组转换成明文字符串
	BitToByte(Bit_reIP_Table, DeCipherText);

	return ret;
}

int main()
{
	int ret = 0; 
	
	//1、定义变量
	unsigned char PlainText[9];				//要加密的明文(String)
	unsigned char PassWord[9];				//加密的密钥(String)
	unsigned char CipherText[16];			//密文(Hex)
	unsigned char DeCipherText[9];			//解密后的明文(String)

	//2、初始化变量
	memset(PlainText, 0, sizeof(PlainText));
	memset(PassWord, 0, sizeof(PassWord));
	memset(CipherText, 0, sizeof(CipherText));
	memset(DeCipherText, 0, sizeof(DeCipherText));

	printf("Please input Plain Text   ：");
	gets(PlainText);
	printf("Please input PassWord     ：");
	gets(PassWord);

	//3、进行DES加密
	DES_EnCryption(PlainText, PassWord, CipherText);

	printf("Plain Text      :%s\n", PlainText);
	printf("PassWord        :%s\n", PassWord);
	printf("CipherText      :");
	Print_Table(CipherText, 16);
	
	//4、进行DES解密
	DES_DeCipher(CipherText, PassWord, DeCipherText);
	printf("DeCipherText    :%s\n", DeCipherText);

	system("pause");
	return ret;
}