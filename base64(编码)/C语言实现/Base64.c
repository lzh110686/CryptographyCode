# define  _CRT_SECURE_NO_WARNINGS
# include "stdio.h"
# include "stdlib.h"
# include "string.h"
# include "Base64Table.h"


///////////////////////////////////////////////////////////////
//����:	����Base64�±��ȡBase64�ַ���
//����:	CipherGroup				�����Base64�ַ���
//		SixBitGroup				���������6Bit��
//		SixBitGroupSize			���������6Bit��ĳ���
//����ֵ:����0�ɹ�												
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
//����:	�������ַ���ת��Ϊ6Bit��
//����:	BitPlainText	����Ĳ�ȫ�������ַ���
//		SixBitGroup		���������6Bit��
//		SixBitGroupSize	�����6Bit��ĳ���
//����ֵ:����0�ɹ�
int TransitionSixBitGroup(unsigned char *BitPlainText, unsigned char* SixBitGroup, unsigned int SixBitGroupSize)
{
	int ret = 0;

	//1��ÿ4��6Bit��һ��ѭ��
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
//����:	Base64����
//����:	PlainText		����������ַ���
//		CipherText	����������ַ���
//����ֵ:����0�ɹ�
int Base64EnCryption(unsigned char *PlainText, unsigned char **CipherText)
{
	int ret = 0;

	//1���������
	unsigned char*			BitPlainText;			//����Bit��			��̬����	ÿһ���ֽڱ�ʾһ��8Bitλ�����ڽ�����3�ı������ַ�����ȫ
	unsigned int			BitPlainSize;			//����Bit���С
	unsigned char*			SixBitGroup;			//����6Bit��			��̬���� ������Bit�鰴6Bit���飬���λ��0���γ�8Bite��С,��1�ֽڱ�ʾ
	unsigned int			SixBitGroupSize;		//����6Bit���С
	unsigned char*			CipherGroup;			//Base64������ַ���	��̬����

	//2���ж������Ƿ���3�ı���������ȡ��Bit��Ĵ�С
	if (strlen(PlainText) % 3)	//����3�ı���
	{
		BitPlainSize		= (strlen(PlainText) / 3 + 1) * 3;
		SixBitGroupSize		= (strlen(PlainText) / 3 + 1) * 4;
	}
	else	//��3�ı���
	{
		BitPlainSize = strlen(PlainText);
		SixBitGroupSize = strlen(PlainText) / 3 * 4;
	}

	//3��������ʼ��
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

	//4�������ĵ��ַ���ת����6Bit��		8bit --> 6bit
	ret = TransitionSixBitGroup(BitPlainText, SixBitGroup, SixBitGroupSize);
	if (ret != 0)
	{
		ret = -2;
		printf("Base64EnCryption TransitionSixBitGroup error:%d\n", ret);
		return ret;
	}

	//5������6Bit���ȡ�ַ���
	ret = GetBase64String(CipherGroup, SixBitGroup, SixBitGroupSize);
	if (ret != 0)
	{
		ret = -4;
		printf("Base64EnCryption GetBase64String error:%d\n", ret);
		return ret;
	}

	//6������󲹳���ַ���ת����'='
	for (int i = SixBitGroupSize - 1; i > SixBitGroupSize - 3; i--)
	{
		if (CipherGroup[i] == 'A')
		{
			CipherGroup[i] = '=';
		}
	}

	//7���ı�ָ��ָ��
	*CipherText = CipherGroup;

	free(BitPlainText);
	free(SixBitGroup);
	return ret;
}


///////////////////////////////////////////////////////////////
//����:	����Base64�ַ�����ȡBase64�±�
//����:	CipherText				�����Base64�ַ���
//		Base64Index				�����Base64�±�
//		Base64IndexSize			�����Base64�±�ĳ���
//����ֵ:����0�ɹ�												
int GetBase64Index(unsigned char *CipherText, unsigned char *Base64Index, unsigned int Base64IndexSize)
{
	int ret = 0;

	for (int i = 0; i < Base64IndexSize; i++)
	{
		//�����±�
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
		else	//�����ַ���ĩβ��'='�����
		{
			Base64Index[i] = 0;
		}
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//����:	��6Bit���Base64�±�ת��Ϊ�����ַ�����8Bit����ʽ  6 --> 8
//����:	BitPlainText	���������8Bit��
//		Base64Index		�����6Bit���Base64�±�
//		BitPlainSize	�����6Bit���Base64�±�ĳ���
//����ֵ:����0�ɹ�
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
//����:	Base64����
//����:	PlainText		����������ַ���
//		CipherText		����������ַ���
//����ֵ:����0�ɹ�
int Base64DeCipher(unsigned char *CipherText, unsigned char *DeCipherText)
{
	int ret = 0;

	//1���������
	unsigned char *		Base64Index;		//Base64��Ӧ���±������	(Ҳ����6Bit��)		��̬����
	unsigned int		Base64IndexSize;	//Base64��Ӧ���±������Ĵ�С
	unsigned char*		BitPlainText;		//����8Bit��			��̬����	ÿһ���ֽڱ�ʾһ��8Bitλ�����ڽ�����3�ı������ַ�����ȫ
	unsigned int		BitPlainSize;		//����8Bit���С	

	//2��������ʼ��
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

	//3����Base64����ת����Base64�±�
	ret = GetBase64Index(CipherText, Base64Index, Base64IndexSize);
	if (ret != 0)
	{
		ret = -2;
		printf("Base64DeCipher GetBase64Index error:%d\n", ret);
		return ret;
	}

	//4����Base64�±�ת����8Bit��������ַ���
	ret = TransitionEightBitGroup(BitPlainText, Base64Index, Base64IndexSize);
	if (ret != 0)
	{
		ret = -3;
		printf("Base64DeCipher TransitionEightBitGroup error:%d\n", ret);
		return ret;
	}

	//5��copy�ַ���
	memcpy(DeCipherText, BitPlainText, Base64IndexSize);
	return ret;
}

int main()
{
	int ret = 0;

	//1���������
	unsigned char PlainText[MAX_DATA];			//����������ַ���
	unsigned char *CipherText;					//����������ַ���
	unsigned char DeCipherText[MAX_DATA];		//����Ľ����ַ���

	//2��������ʼ��
	memset(PlainText, 0, MAX_DATA);
	CipherText = NULL;
	memset(DeCipherText, 0, MAX_DATA);

	//3����ȡ�����ַ�����ֵ
	printf("�����������ַ���: \n");
	scanf("%s", PlainText);

	//4������Base64����
	Base64EnCryption(PlainText, &CipherText);
	printf("Base64�ַ���: \n");
	printf("%s\n", CipherText);

	//5������Base64����
	Base64DeCipher(CipherText, DeCipherText);
	printf("�����ַ���: \n");
	printf("%s\n", DeCipherText);

	system("pause");
	return ret;
}