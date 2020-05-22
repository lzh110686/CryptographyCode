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
//����:	���ַ���ת����AES��׼����
//����:	String			������ַ���
//		Array			�����AES��׼����
//����ֵ:int
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
//����:	��AES��׼����ת�����ַ���
//����:	Array			�����AES��׼����
//		String			������ַ���
//����ֵ:int
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
//����:	��ָ������չ��Կ�����н���S���滻
//����:	ExtendKeyArray		�������չ��Կ����
//		nCol				������к�
//����ֵ:int
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
//����:	���к����ĵı�������Կ��չ����ִ��G����
//����:	ExtendKeyArray		�������չ��Կ����
//		nCol				������к�
//����ֵ:int
int G_Function(unsigned char(*ExtendKeyArray)[44], unsigned int nCol)
{
	int ret = 0;

	//1������չ��Կ�����nCol-1�и��Ƶ�nCol���ϣ�����nCol�е�һ�е�Ԫ���ƶ������һ�У�������������һ��
	for (int i = 0; i < 4; i++)
	{
		ExtendKeyArray[i][nCol] = ExtendKeyArray[(i + 1) % 4][nCol - 1];
	}

	//2����nCol�н���S���滻
	Key_S_Substitution(ExtendKeyArray, nCol);

	//3�������е�һ��Ԫ����Rcon�����������
	ExtendKeyArray[0][nCol] ^= Rcon[nCol / 4];

	return ret;
}

///////////////////////////////////////////////////////////////
//����:	������չ��Կ����
//����:	PasswordArray			�������Կ�ַ�������
//		ExtendKeyArray			�������չ��Կ����
//����ֵ:int
int CalculateExtendKeyArray(const unsigned char(*PasswordArray)[4], unsigned char(*ExtendKeyArray)[44])
{
	int ret = 0;

	//1������Կ�������ǰ������չ��Կ��
	for (int i = 0; i < 16; i++)
	{
		ExtendKeyArray[i & 0x03][i >> 2] = PasswordArray[i & 0x03][i >> 2];
	}

	//2��������չ����ĺ���ʮ��
	for (int i = 1; i < 11; i++)	//����ʮ��ѭ��
	{
		//(1)����к���4�ı�������ִ��G����  ����nCol-1�и��Ƶ�nCol����
		G_Function(ExtendKeyArray, 4*i);

		//(2)ÿһ���У����н����������
		//		�к���4�ı���
		for (int k = 0; k < 4; k++)//�к�
		{
			ExtendKeyArray[k][4 * i] = ExtendKeyArray[k][4 * i] ^ ExtendKeyArray[k][4 * (i - 1)];
		}

		//		��������
		for (int j = 1; j < 4; j++)//ÿһ�ֵ��к�
		{
			for (int k = 0; k < 4; k++)//�к�
			{
				ExtendKeyArray[k][4 * i + j] = ExtendKeyArray[k][4 * i + j - 1] ^ ExtendKeyArray[k][4 * (i - 1) + j];
			}
		}
	}

	return ret;
}


///////////////////////////////////////////////////////////////
//����:	����Կ��
//����:	PlainArray			����������������
//		ExtendKeyArray		�������չ��Կ����
//		MinCol				�������С�к�
//����ֵ:int
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
//����:	�����ľ������S�е��ֽڴ���
//����:	PlainArray			��������ľ���
//����ֵ:int
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
//����:	ShiftRows�Ӳ����λ��
//����:	PlainArray			��������ľ���(ǿ��ת����int����)
//����ֵ:int
int ShiftRows(unsigned int *PlainArray)
{
	int ret = 0;

	//��һ�� ����λ
	//PlainArray[0] = PlainArray[0];

	//�ڶ��� ����8Bit
	PlainArray[1] = (PlainArray[1] >> 8) | (PlainArray[1] << 24);

	//������ ����16Bit
	PlainArray[2] = (PlainArray[2] >> 16) | (PlainArray[2] << 16);

	//������ ����24Bit
	PlainArray[3] = (PlainArray[3] >> 24) | (PlainArray[3] << 8);

	return ret;
}

///////////////////////////////////////////////////////////////
//����:	٤�������ڵĳ˷�����	GF(128)
//����:	Num_L			����������
//		Num_R			������Ҳ���
//����ֵ:������
char GaloisMultiplication(unsigned char Num_L, unsigned char Num_R)
{
	//�������
	unsigned char Result = 0;		//٤�������ڳ˷�����Ľ��

	while (Num_L)
	{
		//���Num_L���λ��1�����Num_R���൱�ڼ���Num_R * 1
		if (Num_L & 0x01)
		{
			Result ^= Num_R;
		}

		//Num_L����һλ���൱�ڳ���2
		Num_L = Num_L >> 1;

		//���Num_R���λΪ1
		if (Num_R & 0x80)
		{
			//����һλ�൱�ڳ˶�
			Num_R = Num_R << 1;		//ע������ᶪʧ���λ�����ǲ��õ���

			Num_R ^= 0x1B;	//����٤�������ڳ���Num_R = Num_R / (x^8(�պö�ʧ���λ) + x^4 + x^3 + x^1 + 1)
		}
		else
		{
			//����һλ�൱�ڳ˶�
			Num_R = Num_R << 1;
		}
	}

	return Result;
}


///////////////////////////////////////////////////////////////
//����:	MixColum�Ӳ���л���
//����:	PlainArray			��������ľ���
//����ֵ:int
int MixColum(unsigned char(*PlainArray)[4])
{
	int ret = 0;

	//�������
	unsigned char ArrayTemp[4][4];

	//��ʼ������
	memcpy(ArrayTemp, PlainArray, 16);

	//����˷� 4*4
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

	//1���������
	unsigned char PlainArray[4][4];			//���ĵ�4*4����128Bit
	unsigned char PasswordArray[4][4];		//��Կ��չǰ��4*4����128Bit
	unsigned char ExtendKeyArray[4][44];	//�洢��Կ��չ��ľ���4*44

	//2����ʼ������
	memset(PlainArray,		0, 16);
	memset(PasswordArray,	0, 16);
	memset(ExtendKeyArray,	0, 176);

	//3�����ַ���ת����AESָ������������
	StringToArray(PlainText,		PlainArray);
	StringToArray(PassWord,			PasswordArray);

	//4��������չ��Կ
	CalculateExtendKeyArray(PasswordArray, ExtendKeyArray);

	//5������Կ��
	AddRoundKey(PlainArray, ExtendKeyArray, 0);

	//6��ѭ��ǰn-1��
	for (int i = 1; i < 10; i++)
	{
		//(1)�ֽڴ�����
		Plain_S_Substitution((unsigned char *)PlainArray);

		//(2)ShiftRow�Ӳ�	��λ��
		ShiftRows((unsigned int *)PlainArray);

		//(3)MixColum�Ӳ�	�л���
		MixColum(PlainArray);

		//(4)����Կ��
		AddRoundKey(PlainArray, ExtendKeyArray, 4 * i);
	}

	//7�����һ��
	//(1)�ֽڴ�����
	Plain_S_Substitution((unsigned char *)PlainArray);

	//(2)ShiftRow�Ӳ�	��λ��
	ShiftRows((unsigned int *)PlainArray);

	//(3)����Կ��
	AddRoundKey(PlainArray, ExtendKeyArray, 4 * 10);

	//8���������������	ע�����ĵ����з�ʽ
	ArrayToString(PlainArray, CipherText);

	return ret;
}



///////////////////////////////////////////////////////////////
//����:	����ShiftRows�Ӳ����λ��
//����:	CipherArray			��������ľ���(ǿ��ת����int����)
//����ֵ:int
int ReShiftRows(unsigned int *CipherArray)
{
	int ret = 0;

	//��һ�� ����λ
	//CipherArray[0] = CipherArray[0];

	//�ڶ��� ����8Bit
	CipherArray[1] = (CipherArray[1] << 8) | (CipherArray[1] >> 24);

	//������ ����16Bit
	CipherArray[2] = (CipherArray[2] << 16) | (CipherArray[2] >> 16);

	//������ ����24Bit
	CipherArray[3] = (CipherArray[3] << 24) | (CipherArray[3] >> 8);

	return ret;
}

///////////////////////////////////////////////////////////////
//����:	�����ľ����������S�е��ֽڴ���
//����:	CipherArray			��������ľ���
//����ֵ:int
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
//����:	����MixColum�Ӳ���л���
//����:	CipherArray			��������ľ���
//����ֵ:int
int ReMixColum(unsigned char(*CipherArray)[4])
{
	int ret = 0;

	//�������
	unsigned char ArrayTemp[4][4];

	//��ʼ������
	memcpy(ArrayTemp, CipherArray, 16);

	//����˷� 4*4
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

	//1���������
	unsigned char CipherArray[4][4];					//���ĵ�4*4����
	unsigned char PasswordArray[4][4];					//�����4*4����
	unsigned char ExtendKeyArray[4][44];				//�洢��Կ��չ��ľ���4*44

	//2����ʼ������
	memset(CipherArray,		0, 16);
	memset(PasswordArray,	0, 16);
	memset(ExtendKeyArray,	0, 176);

	//3�����ַ���ת����AESָ������������
	StringToArray(CipherText, CipherArray);
	StringToArray(PassWord, PasswordArray);

	//4��������չ��Կ
	CalculateExtendKeyArray(PasswordArray, ExtendKeyArray);

	//5���������һ��		���������չ��Կ�����ľ����������Կ��
	//(1)����Կ��
	AddRoundKey(CipherArray, ExtendKeyArray, 4 * 10);

	//(2)����ShiftRow�Ӳ�		��λ��
	ReShiftRows((unsigned int *)CipherArray);
	
	//(3)�ֽڴ�����
	Cipher_S_Substitution((unsigned char *)CipherArray);

	//6��ѭ��ǰn-1��
	for (int i = 9; i > 0; i--)
	{
		//(1)��Կ�ӷ���
		AddRoundKey(CipherArray, ExtendKeyArray, 4 * i);

		//(2)�����л���
		ReMixColum(CipherArray);

		//(3)����ShiftRows
		ReShiftRows((unsigned int *)CipherArray);

		//(4)�����ֽڴ���
		Cipher_S_Substitution((unsigned char *)CipherArray);
	}
	
	//7������Կ��
	AddRoundKey(CipherArray, ExtendKeyArray, 0);

	//8���������������	ע�����ĵ����з�ʽ
	ArrayToString(CipherArray, DeCipherText);

	return ret;
}

int main()
{
	int ret = 0;
	//����ֲ�����
	//unsigned char PlainText[17];				//Ҫ���ܵ�����(String)  128Bit
	//unsigned char PassWord[17];				//���ܵ���Կ(String)	  128Bit
	unsigned char	 CipherText[17];			//����(Hex)			  128Bit
	unsigned char	 DeCipherText[17];			//���ܺ������(String)  128Bit


	//������ʼ��
	unsigned char PlainText[17] = "0123456789ABCDEF";				//Ҫ���ܵ�����(String)  128Bit
	unsigned char PassWord[17]  = "0123456789ABCDEF";				//Ҫ���ܵ�����(String)  128Bit

	//memset(PlainText, 0, sizeof(PlainText));
	//memset(PassWord, 0, sizeof(PassWord));
	memset(CipherText, 0, sizeof(CipherText));
	memset(DeCipherText, 0, sizeof(DeCipherText));

	//��ȡ����
	printf("Please input Plain Text	(128Bit)��");
	//gets(PlainText);
	printf("%s\n", PlainText);
	printf("Please input PassWord	(128Bit)��");
	//gets(PassWord);
	printf("%s\n", PassWord);


	//����AES����
	printf("PlainText       : %s\n", PlainText);
	printf("PassWord        : %s\n", PassWord);
	AES_EnCryption(PlainText, PassWord, CipherText);
	printf("CipherText	:\n");
	Print_Table(CipherText, 16);

	//����AES����
	AES_DeCipher(CipherText, PassWord, DeCipherText);
	printf("DeCipherText    : %s\n", DeCipherText);

	system("pause");
	return ret;
}