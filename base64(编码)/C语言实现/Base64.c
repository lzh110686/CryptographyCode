# define  _CRT_SECURE_NO_WARNINGS
# include "stdio.h"
# include "stdlib.h"
# include "string.h"
# include "Base64Table.h"


///////////////////////////////////////////////////////////////
//功能:	根据Base64下标获取Base64字符串
//参数:	CipherGroup				输出的Base64字符串
//		SixBitGroup				输入的明文6Bit组
//		SixBitGroupSize			输入的明文6Bit组的长度
//返回值:返回0成功												
int GetBase64String(unsigned char *CipherGroup, unsigned char *SixBitGroup, unsigned int SixBitGroupSize)
{
	int ret = 0;

	for (int i = 0; i < SixBitGroupSize; i++)
	{
		CipherGroup[i] = Base64Table[SixBitGroup[i]];
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	将明文字符串转换为6Bit组
//参数:	BitPlainText	输入的补全的明文字符串
//		SixBitGroup		输出的明文6Bit组
//		SixBitGroupSize	输入的6Bit组的长度
//返回值:返回0成功
int TransitionSixBitGroup(unsigned char *BitPlainText, unsigned char* SixBitGroup, unsigned int SixBitGroupSize)
{
	int ret = 0;

	//1、每4个6Bit组一个循环
	for (int i = 0, j = 0; i < SixBitGroupSize; i += 4, j += 3)
	{
		SixBitGroup[i]		= ((BitPlainText[j] & 0xFC) >> 2);
		SixBitGroup[i + 1]	= ((BitPlainText[j] & 0x03) << 4) + ((BitPlainText[j + 1] & 0xF0) >> 4);
		SixBitGroup[i + 2]	= ((BitPlainText[j + 1] & 0x0F) << 2) + ((BitPlainText[j + 2] & 0xC0) >> 6);
		SixBitGroup[i + 3]	= (BitPlainText[j + 2] & 0x3F);
	}

	return ret;
}



///////////////////////////////////////////////////////////////
//功能:	Base64加密
//参数:	PlainText		输入的明文字符串
//		CipherText	输出的密文字符串
//返回值:返回0成功
int Base64EnCryption(unsigned char *PlainText, unsigned char **CipherText)
{
	int ret = 0;

	//1、定义变量
	unsigned char*			BitPlainText;			//明文Bit组			动态分配	每一个字节表示一个8Bit位，用于将不是3的倍数的字符串补全
	unsigned int			BitPlainSize;			//明文Bit组大小
	unsigned char*			SixBitGroup;			//明文6Bit组			动态分配 将明文Bit组按6Bit分组，最高位补0，形成8Bite大小,用1字节表示
	unsigned int			SixBitGroupSize;		//明文6Bit组大小
	unsigned char*			CipherGroup;			//Base64编码的字符串	动态分配

	//2、判断明文是否是3的倍数，并获取各Bit组的大小
	if (strlen(PlainText) % 3)	//不是3的倍数
	{
		BitPlainSize		= (strlen(PlainText) / 3 + 1) * 3;
		SixBitGroupSize		= (strlen(PlainText) / 3 + 1) * 4;
	}
	else	//是3的倍数
	{
		BitPlainSize = strlen(PlainText);
		SixBitGroupSize = strlen(PlainText) / 3 * 4;
	}

	//3、变量初始化
	BitPlainText = (unsigned char*)malloc(BitPlainSize);
	if (BitPlainText == NULL)
	{
		ret = -1;
		printf("Base64EnCryption malloc error:%d\n", ret);
		return ret;
	}
	memset(BitPlainText, 0, BitPlainSize);
	memcpy(BitPlainText, PlainText, strlen(PlainText));

	SixBitGroup = (unsigned char*)malloc(SixBitGroupSize);
	if (SixBitGroup == NULL)
	{
		ret = -1;
		printf("Base64EnCryption malloc error:%d\n", ret);
		return ret;
	}
	memset(SixBitGroup, 0, SixBitGroupSize);

	CipherGroup = (unsigned char*)malloc(SixBitGroupSize + 1);
	if (CipherGroup == NULL)
	{
		ret = -1;
		printf("Base64EnCryption malloc error:%d\n", ret);
		return ret;
	}
	memset(CipherGroup, 0, SixBitGroupSize + 1);

	//4、将明文的字符串转换成6Bit组		8bit --> 6bit
	ret = TransitionSixBitGroup(BitPlainText, SixBitGroup, SixBitGroupSize);
	if (ret != 0)
	{
		ret = -2;
		printf("Base64EnCryption TransitionSixBitGroup error:%d\n", ret);
		return ret;
	}

	//5、根据6Bit组获取字符串
	ret = GetBase64String(CipherGroup, SixBitGroup, SixBitGroupSize);
	if (ret != 0)
	{
		ret = -4;
		printf("Base64EnCryption GetBase64String error:%d\n", ret);
		return ret;
	}

	//6、将最后补充的字符串转换成'='
	for (int i = SixBitGroupSize - 1; i > SixBitGroupSize - 3; i--)
	{
		if (CipherGroup[i] == 'A')
		{
			CipherGroup[i] = '=';
		}
	}

	//7、改变指针指向
	*CipherText = CipherGroup;

	free(BitPlainText);
	free(SixBitGroup);
	return ret;
}


///////////////////////////////////////////////////////////////
//功能:	根据Base64字符串获取Base64下标
//参数:	CipherText				输入的Base64字符串
//		Base64Index				输出的Base64下标
//		Base64IndexSize			输出的Base64下标的长度
//返回值:返回0成功												
int GetBase64Index(unsigned char *CipherText, unsigned char *Base64Index, unsigned int Base64IndexSize)
{
	int ret = 0;

	for (int i = 0; i < Base64IndexSize; i++)
	{
		//计算下标
		if (CipherText[i] >= 'A' && CipherText[i] <= 'Z')
		{
			Base64Index[i] = CipherText[i] - 'A';
		}
		else if (CipherText[i] >= 'a' && CipherText[i] <= 'z')
		{
			Base64Index[i] = CipherText[i] - 'a' + 26;
		}
		else if (CipherText[i] >= '0' && CipherText[i] <= '9')
		{
			Base64Index[i] = CipherText[i] - '0' + 52;
		}
		else if (CipherText[i] == '+')
		{
			Base64Index[i] = 62;
		}
		else if (CipherText[i] == '/')
		{
			Base64Index[i] = 63;
		}
		else	//处理字符串末尾是'='的情况
		{
			Base64Index[i] = 0;
		}
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//功能:	将6Bit组的Base64下标转换为明文字符串的8Bit组形式  6 --> 8
//参数:	BitPlainText	输出的明文8Bit组
//		Base64Index		输入的6Bit组的Base64下标
//		BitPlainSize	输入的6Bit组的Base64下标的长度
//返回值:返回0成功
int TransitionEightBitGroup(unsigned char *BitPlainText, unsigned char *Base64Index, unsigned int Base64IndexSize)
{
	int ret = 0;

	for (int i = 0, j = 0; j < Base64IndexSize; i += 3, j += 4)
	{
		BitPlainText[i]		= (Base64Index[j] << 2) + ((Base64Index[j + 1] & 0xF0) >> 4);
		BitPlainText[i + 1] = ((Base64Index[j + 1] & 0x0F) << 4) + ((Base64Index[j + 2] & 0x3C) >> 2);
		BitPlainText[i + 2] = ((Base64Index[j + 2] & 0x03) << 6) + Base64Index[j + 3];
	}

	return ret;
}


///////////////////////////////////////////////////////////////
//功能:	Base64解密
//参数:	PlainText		输入的明文字符串
//		CipherText		输出的密文字符串
//返回值:返回0成功
int Base64DeCipher(unsigned char *CipherText, unsigned char *DeCipherText)
{
	int ret = 0;

	//1、定义变量
	unsigned char *		Base64Index;		//Base64对应的下标的数组	(也就是6Bit组)		动态分配
	unsigned int		Base64IndexSize;	//Base64对应的下标的数组的大小
	unsigned char*		BitPlainText;		//明文8Bit组			动态分配	每一个字节表示一个8Bit位，用于将不是3的倍数的字符串补全
	unsigned int		BitPlainSize;		//明文8Bit组大小	

	//2、变量初始化
	Base64IndexSize = strlen(CipherText);
	Base64Index = (unsigned char*)malloc(Base64IndexSize);
	if (Base64Index == NULL)
	{
		ret = -1;
		printf("Base64DeCipher malloc error:%d\n", ret);
		return ret;
	}
	memset(Base64Index, 0, Base64IndexSize);

	BitPlainSize = strlen(CipherText);
	BitPlainText = (unsigned char*)malloc(BitPlainSize);
	if (BitPlainText == NULL)
	{
		ret = -1;
		printf("Base64DeCipher malloc error:%d\n", ret);
		return ret;
	}
	memset(BitPlainText, 0, BitPlainSize);

	//3、将Base64密文转换成Base64下标
	ret = GetBase64Index(CipherText, Base64Index, Base64IndexSize);
	if (ret != 0)
	{
		ret = -2;
		printf("Base64DeCipher GetBase64Index error:%d\n", ret);
		return ret;
	}

	//4、将Base64下标转换成8Bit组的明文字符串
	ret = TransitionEightBitGroup(BitPlainText, Base64Index, Base64IndexSize);
	if (ret != 0)
	{
		ret = -3;
		printf("Base64DeCipher TransitionEightBitGroup error:%d\n", ret);
		return ret;
	}

	//5、copy字符串
	memcpy(DeCipherText, BitPlainText, Base64IndexSize);
	return ret;
}

int main()
{
	int ret = 0;

	//1、定义变量
	unsigned char PlainText[MAX_DATA];			//输入的明文字符串
	unsigned char *CipherText;					//输出的密文字符串
	unsigned char DeCipherText[MAX_DATA];		//输出的解密字符串

	//2、变量初始化
	memset(PlainText, 0, MAX_DATA);
	CipherText = NULL;
	memset(DeCipherText, 0, MAX_DATA);

	//3、获取明文字符串赋值
	printf("请输入明文字符串: \n");
	scanf("%s", PlainText);

	//4、进行Base64加密
	Base64EnCryption(PlainText, &CipherText);
	printf("Base64字符串: \n");
	printf("%s\n", CipherText);

	//5、进行Base64解密
	Base64DeCipher(CipherText, DeCipherText);
	printf("明文字符串: \n");
	printf("%s\n", DeCipherText);

	system("pause");
	return ret;
}