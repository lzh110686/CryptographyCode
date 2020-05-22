# define  _CRT_SECURE_NO_WARNINGS
# include "stdio.h"
# include "stdlib.h"
# include "string.h"
# include "AES_Table.h"


int Print_Table(const unsigned char* Table, int TableSize)
{
	for (int i = 0; i < TableSize; i++)
	{
		printf("%02X ", Table[i]);
	}
	printf("\n");

	return 0;
}


///////////////////////////////////////////////////////////////
//功能:	将字符串转换成AES标准数组
//参数:	String			输入的字符串
//		Array			输出的AES标准数组
//返回值:int
int StringToArray(const unsigned char *String, unsigned char (*Array)[4])
{
	int ret = 0;

	for (int i = 0; i < 16; i++)
	{
		Array[i & 0x03][i >> 2] = String[i];
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	将AES标准数组转换成字符串
//参数:	Array			输入的AES标准数组
//		String			输出的字符串
//返回值:int
int ArrayToString(const unsigned char(*Array)[4], unsigned char *String)
{
	int ret = 0;

	for (int i = 0; i < 16; i++)
	{
		String[i] = Array[i & 0x03][i >> 2];
	}

	return ret;
}


///////////////////////////////////////////////////////////////
//功能:	对指定的扩展密钥矩阵列进行S盒替换
//参数:	ExtendKeyArray		输入的扩展密钥矩阵
//		nCol				输入的列号
//返回值:int
int Key_S_Substitution(unsigned char(*ExtendKeyArray)[44], unsigned int nCol)
{
	int ret = 0;

	for (int i = 0; i < 4; i++)
	{
		ExtendKeyArray[i][nCol] = S_Table[(ExtendKeyArray[i][nCol]) >> 4][(ExtendKeyArray[i][nCol]) & 0x0F];
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	对列号是四的倍数的密钥扩展矩阵执行G函数
//参数:	ExtendKeyArray		输入的扩展密钥矩阵
//		nCol				输入的列号
//返回值:int
int G_Function(unsigned char(*ExtendKeyArray)[44], unsigned int nCol)
{
	int ret = 0;

	//1、将扩展密钥矩阵的nCol-1列复制到nCol列上，并将nCol列第一行的元素移动到最后一行，其他行数上移一行
	for (int i = 0; i < 4; i++)
	{
		ExtendKeyArray[i][nCol] = ExtendKeyArray[(i + 1) % 4][nCol - 1];
	}

	//2、将nCol列进行S盒替换
	Key_S_Substitution(ExtendKeyArray, nCol);

	//3、将该列第一行元素与Rcon进行异或运算
	ExtendKeyArray[0][nCol] ^= Rcon[nCol / 4];

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	计算扩展密钥数组
//参数:	PasswordArray			输入的密钥字符串数组
//		ExtendKeyArray			输出的扩展密钥数组
//返回值:int
int CalculateExtendKeyArray(const unsigned char(*PasswordArray)[4], unsigned char(*ExtendKeyArray)[44])
{
	int ret = 0;

	//1、将密钥数组放入前四列扩展密钥组
	for (int i = 0; i < 16; i++)
	{
		ExtendKeyArray[i & 0x03][i >> 2] = PasswordArray[i & 0x03][i >> 2];
	}

	//2、计算扩展矩阵的后四十列
	for (int i = 1; i < 11; i++)	//进行十轮循环
	{
		//(1)如果列号是4的倍数，这执行G函数  否则将nCol-1列复制到nCol列上
		G_Function(ExtendKeyArray, 4*i);

		//(2)每一轮中，各列进行异或运算
		//		列号是4的倍数
		for (int k = 0; k < 4; k++)//行号
		{
			ExtendKeyArray[k][4 * i] = ExtendKeyArray[k][4 * i] ^ ExtendKeyArray[k][4 * (i - 1)];
		}

		//		其他三列
		for (int j = 1; j < 4; j++)//每一轮的列号
		{
			for (int k = 0; k < 4; k++)//行号
			{
				ExtendKeyArray[k][4 * i + j] = ExtendKeyArray[k][4 * i + j - 1] ^ ExtendKeyArray[k][4 * (i - 1) + j];
			}
		}
	}

	return ret;
}


///////////////////////////////////////////////////////////////
//功能:	轮密钥加
//参数:	PlainArray			输入的明文数组矩阵
//		ExtendKeyArray		输入的扩展密钥数组
//		MinCol				输入的最小列号
//返回值:int
int AddRoundKey(unsigned char(*PlainArray)[4], unsigned char(*ExtendKeyArray)[44], unsigned int MinCol)
{
	int ret = 0;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			PlainArray[i][j] ^= ExtendKeyArray[i][MinCol + j];
		}
	}

	return ret;
}


///////////////////////////////////////////////////////////////
//功能:	对明文矩阵进行S盒的字节代换
//参数:	PlainArray			输入的明文矩阵
//返回值:int
int Plain_S_Substitution(unsigned char *PlainArray)
{
	int ret = 0;

	for (int i = 0; i < 16; i++)
	{
		PlainArray[i] = S_Table[PlainArray[i] >> 4][PlainArray[i] & 0x0F];
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	ShiftRows子层的行位移
//参数:	PlainArray			输入的明文矩阵(强制转换成int类型)
//返回值:int
int ShiftRows(unsigned int *PlainArray)
{
	int ret = 0;

	//第一行 不移位
	//PlainArray[0] = PlainArray[0];

	//第二行 左移8Bit
	PlainArray[1] = (PlainArray[1] >> 8) | (PlainArray[1] << 24);

	//第三行 左移16Bit
	PlainArray[2] = (PlainArray[2] >> 16) | (PlainArray[2] << 16);

	//第四行 左移24Bit
	PlainArray[3] = (PlainArray[3] >> 24) | (PlainArray[3] << 8);

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	伽罗瓦域内的乘法运算	GF(128)
//参数:	Num_L			输入的左参数
//		Num_R			输入的右参数
//返回值:计算结果
char GaloisMultiplication(unsigned char Num_L, unsigned char Num_R)
{
	//定义变量
	unsigned char Result = 0;		//伽罗瓦域内乘法计算的结果

	while (Num_L)
	{
		//如果Num_L最低位是1就异或Num_R，相当于加上Num_R * 1
		if (Num_L & 0x01)
		{
			Result ^= Num_R;
		}

		//Num_L右移一位，相当于除以2
		Num_L = Num_L >> 1;

		//如果Num_R最高位为1
		if (Num_R & 0x80)
		{
			//左移一位相当于乘二
			Num_R = Num_R << 1;		//注：这里会丢失最高位，但是不用担心

			Num_R ^= 0x1B;	//计算伽罗瓦域内除法Num_R = Num_R / (x^8(刚好丢失最高位) + x^4 + x^3 + x^1 + 1)
		}
		else
		{
			//左移一位相当于乘二
			Num_R = Num_R << 1;
		}
	}

	return Result;
}


///////////////////////////////////////////////////////////////
//功能:	MixColum子层的列混淆
//参数:	PlainArray			输入的明文矩阵
//返回值:int
int MixColum(unsigned char(*PlainArray)[4])
{
	int ret = 0;

	//定义变量
	unsigned char ArrayTemp[4][4];

	//初始化变量
	memcpy(ArrayTemp, PlainArray, 16);

	//矩阵乘法 4*4
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			PlainArray[i][j] =
				GaloisMultiplication(MixArray[i][0], ArrayTemp[0][j]) ^
				GaloisMultiplication(MixArray[i][1], ArrayTemp[1][j]) ^
				GaloisMultiplication(MixArray[i][2], ArrayTemp[2][j]) ^
				GaloisMultiplication(MixArray[i][3], ArrayTemp[3][j]);
		}
	}

	return ret;
}

int AES_EnCryption(const unsigned char *PlainText, const unsigned char *PassWord, unsigned char *CipherText)
{
	int ret = 0;

	//1、定义变量
	unsigned char PlainArray[4][4];			//明文的4*4矩阵128Bit
	unsigned char PasswordArray[4][4];		//密钥扩展前的4*4矩阵128Bit
	unsigned char ExtendKeyArray[4][44];	//存储密钥扩展后的矩阵4*44

	//2、初始化变量
	memset(PlainArray,		0, 16);
	memset(PasswordArray,	0, 16);
	memset(ExtendKeyArray,	0, 176);

	//3、将字符串转换成AES指定的数组排列
	StringToArray(PlainText,		PlainArray);
	StringToArray(PassWord,			PasswordArray);

	//4、计算扩展密钥
	CalculateExtendKeyArray(PasswordArray, ExtendKeyArray);

	//5、轮密钥加
	AddRoundKey(PlainArray, ExtendKeyArray, 0);

	//6、循环前n-1轮
	for (int i = 1; i < 10; i++)
	{
		//(1)字节代换层
		Plain_S_Substitution((unsigned char *)PlainArray);

		//(2)ShiftRow子层	行位移
		ShiftRows((unsigned int *)PlainArray);

		//(3)MixColum子层	列混淆
		MixColum(PlainArray);

		//(4)轮密钥加
		AddRoundKey(PlainArray, ExtendKeyArray, 4 * i);
	}

	//7、最后一轮
	//(1)字节代换层
	Plain_S_Substitution((unsigned char *)PlainArray);

	//(2)ShiftRow子层	行位移
	ShiftRows((unsigned int *)PlainArray);

	//(3)轮密钥加
	AddRoundKey(PlainArray, ExtendKeyArray, 4 * 10);

	//8、传出输出的密文	注意密文的排列方式
	ArrayToString(PlainArray, CipherText);

	return ret;
}



///////////////////////////////////////////////////////////////
//功能:	逆向ShiftRows子层的行位移
//参数:	CipherArray			输入的密文矩阵(强制转换成int类型)
//返回值:int
int ReShiftRows(unsigned int *CipherArray)
{
	int ret = 0;

	//第一行 不移位
	//CipherArray[0] = CipherArray[0];

	//第二行 右移8Bit
	CipherArray[1] = (CipherArray[1] << 8) | (CipherArray[1] >> 24);

	//第三行 右移16Bit
	CipherArray[2] = (CipherArray[2] << 16) | (CipherArray[2] >> 16);

	//第四行 右移24Bit
	CipherArray[3] = (CipherArray[3] << 24) | (CipherArray[3] >> 8);

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	对密文矩阵进行逆向S盒的字节代换
//参数:	CipherArray			输入的密文矩阵
//返回值:int
int Cipher_S_Substitution(unsigned char *CipherArray)
{
	int ret = 0;

	for (int i = 0; i < 16; i++)
	{
		CipherArray[i] = ReS_Table[CipherArray[i] >> 4][CipherArray[i] & 0x0F];
	}

	return ret;
}


///////////////////////////////////////////////////////////////
//功能:	逆向MixColum子层的列混淆
//参数:	CipherArray			输入的密文矩阵
//返回值:int
int ReMixColum(unsigned char(*CipherArray)[4])
{
	int ret = 0;

	//定义变量
	unsigned char ArrayTemp[4][4];

	//初始化变量
	memcpy(ArrayTemp, CipherArray, 16);

	//矩阵乘法 4*4
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			CipherArray[i][j] =
				GaloisMultiplication(ReMixArray[i][0], ArrayTemp[0][j]) ^
				GaloisMultiplication(ReMixArray[i][1], ArrayTemp[1][j]) ^
				GaloisMultiplication(ReMixArray[i][2], ArrayTemp[2][j]) ^
				GaloisMultiplication(ReMixArray[i][3], ArrayTemp[3][j]);
		}
	}

	return ret;
}

int AES_DeCipher(const unsigned char *CipherText, const unsigned char *PassWord, unsigned char *DeCipherText)
{
	int ret = 0;

	//1、定义变量
	unsigned char CipherArray[4][4];					//密文的4*4矩阵
	unsigned char PasswordArray[4][4];					//密码的4*4矩阵
	unsigned char ExtendKeyArray[4][44];				//存储密钥扩展后的矩阵4*44

	//2、初始化变量
	memset(CipherArray,		0, 16);
	memset(PasswordArray,	0, 16);
	memset(ExtendKeyArray,	0, 176);

	//3、将字符串转换成AES指定的数组排列
	StringToArray(CipherText, CipherArray);
	StringToArray(PassWord, PasswordArray);

	//4、计算扩展密钥
	CalculateExtendKeyArray(PasswordArray, ExtendKeyArray);

	//5、逆向最后一轮		最后四列扩展密钥与密文矩阵进行轮密钥加
	//(1)轮密钥加
	AddRoundKey(CipherArray, ExtendKeyArray, 4 * 10);

	//(2)逆向ShiftRow子层		行位移
	ReShiftRows((unsigned int *)CipherArray);
	
	//(3)字节代换层
	Cipher_S_Substitution((unsigned char *)CipherArray);

	//6、循环前n-1轮
	for (int i = 9; i > 0; i--)
	{
		//(1)密钥加法层
		AddRoundKey(CipherArray, ExtendKeyArray, 4 * i);

		//(2)逆向列混淆
		ReMixColum(CipherArray);

		//(3)逆向ShiftRows
		ReShiftRows((unsigned int *)CipherArray);

		//(4)逆向字节代换
		Cipher_S_Substitution((unsigned char *)CipherArray);
	}
	
	//7、轮密钥加
	AddRoundKey(CipherArray, ExtendKeyArray, 0);

	//8、传出输出的明文	注意明文的排列方式
	ArrayToString(CipherArray, DeCipherText);

	return ret;
}

int main()
{
	int ret = 0;
	//定义局部变量
	//unsigned char PlainText[17];				//要加密的明文(String)  128Bit
	//unsigned char PassWord[17];				//加密的密钥(String)	  128Bit
	unsigned char	 CipherText[17];			//密文(Hex)			  128Bit
	unsigned char	 DeCipherText[17];			//解密后的明文(String)  128Bit


	//变量初始化
	unsigned char PlainText[17] = "0123456789ABCDEF";				//要加密的明文(String)  128Bit
	unsigned char PassWord[17]  = "0123456789ABCDEF";				//要加密的明文(String)  128Bit

	//memset(PlainText, 0, sizeof(PlainText));
	//memset(PassWord, 0, sizeof(PassWord));
	memset(CipherText, 0, sizeof(CipherText));
	memset(DeCipherText, 0, sizeof(DeCipherText));

	//获取输入
	printf("Please input Plain Text	(128Bit)：");
	//gets(PlainText);
	printf("%s\n", PlainText);
	printf("Please input PassWord	(128Bit)：");
	//gets(PassWord);
	printf("%s\n", PassWord);


	//进行AES加密
	printf("PlainText       : %s\n", PlainText);
	printf("PassWord        : %s\n", PassWord);
	AES_EnCryption(PlainText, PassWord, CipherText);
	printf("CipherText	:\n");
	Print_Table(CipherText, 16);

	//进行AES解密
	AES_DeCipher(CipherText, PassWord, DeCipherText);
	printf("DeCipherText    : %s\n", DeCipherText);

	system("pause");
	return ret;
}