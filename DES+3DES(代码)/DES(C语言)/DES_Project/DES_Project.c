# define  _CRT_SECURE_NO_WARNINGS
# include "stdio.h"
# include "stdlib.h"
# include "string.h"
# include "DES_Table.h"

///////////////////////////////////////////////////////////////
//����:	��ӡ16�����ַ���
//����:	BitGroup			�����16�����ַ���
//		nBit				����Ĵ�ӡ����
//����ֵ:void
void Print_Table(const unsigned char* BitGroup, int nBit)
{
	for (int i = 0; i < nBit; i++)
	{
		printf("%X", BitGroup[i]);
	}
	printf("\n");
}


///////////////////////////////////////////////////////////////
//����:	����Byte�ַ������ȡ���Ӧ��Bit��   0x34 --> 00110100
//����:	Byte				������ַ�����
//		Bit					����Ķ�Ӧ��Bit��
//����ֵ:����0�ɹ�	
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
//����:	����Bit���ȡ���Ӧ���ַ���  00110100 --> 0x34
//����:	Bit				�����Bit��
//		Byte			������ַ�����
//����ֵ:����0�ɹ�	
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
//����:	����Bit���ȡ���Ӧ���ַ���  0011 --> 0x03
//����:	Bit_reIP_Table	���������Bit��
//		CipherText		�����16�����ַ���
//����ֵ:����0�ɹ�	
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
//����:	����Bit���ȡ���Ӧ���ַ���  0x03 -->> 0011
//����:	CipherText		�����16�����ַ���
//		BitGroup		���������Bit��
//����ֵ:����0�ɹ�	
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
//����:	����PC-1��ʼ��Կ�û� 64 ---> 56
//����:	BitKey			�������ԿBit��
//		BitKey_PC_1		������û����
//����ֵ:����0�ɹ�	
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
//����:	����PC-2��Կ�û� 56 ---> 48
//����:	BitKey			�����Bit��
//		SubKey			���������Կ
//����ֵ:����0�ɹ�	
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
//����:	��SrcBitGroup����nBit��DesBitGroup��
//����:	SrcBitGroup		�������תǰ�Ľ��
//		DesBitGroup		�������ת��Ľ��
//		nBit			���Ƶ�λ��
//����ֵ:����0�ɹ�	
int BitRound_L(const unsigned char* SrcBitGroup, unsigned char* DesBitGroup, int nBit)
{
	int ret = 0;

	memcpy(DesBitGroup,				&SrcBitGroup[nBit], 28 - nBit);
	memcpy(&DesBitGroup[28 - nBit], SrcBitGroup,		nBit);

	return ret;
}


///////////////////////////////////////////////////////////////
//����:	����BitKey��Ӧ��16������Կ
//����:	BitKey			�������ԿBit��
//		BitSubKey		�����16������Կ  16*48
//����ֵ:����0�ɹ�	
int CalculateSubKey(const unsigned char *BitKey, unsigned char(*BitSubKey)[48])
{
	int ret = 0;

	//1���������
	unsigned char BitKey_PC_1[56];		//56λPC-1�û������Կ
	unsigned char BitC_Table[17][28];	//17��28λC��
	unsigned char BitD_Table[17][28];	//17��28λD��
	unsigned char BitCD_Table[56];		//�ϲ���CD��

	//2��������ʼ��
	memset(BitKey_PC_1, 0, sizeof(BitKey_PC_1));
	memset(BitC_Table,	0, sizeof(BitC_Table));
	memset(BitD_Table,	0, sizeof(BitD_Table));
	memset(BitCD_Table, 0, sizeof(BitCD_Table));

	//3������PC-1��ʼ��Կ�û�	64λ ---> 56λ
	PC_1_Substitution(BitKey, BitKey_PC_1);

	//4�����û���Ľ���ֳ�C��D����
	memcpy(BitC_Table[0], BitKey_PC_1,		28);
	memcpy(BitD_Table[0], &BitKey_PC_1[28], 28);

	//5��ѭ������ ѭ��16��
	for (int i = 0; i < 16; i++)
	{
		//(1)����C��D���������ת��λ����	����
		BitRound_L(BitC_Table[i], BitC_Table[i + 1], Bit_Round[i]);
		BitRound_L(BitD_Table[i], BitD_Table[i + 1], Bit_Round[i]);

		//(2)������ת��Ľ���ϲ�
		memcpy(BitCD_Table,			BitC_Table[i + 1], 28);
		memcpy(&BitCD_Table[28],	BitD_Table[i + 1], 28);

		//(3)������PC-2��Կ�û� �õ�����һ������Կ 56λ ---> 48λ
		PC_2_Substitution(BitCD_Table, BitSubKey[i]);
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//����:	������Bit����г�ʼIP�û�
//����:	BitPlain		���������Bit��
//		Bit_IP_Table	�����IP�û�����
//����ֵ:����0�ɹ�	
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
//����:	��BitR_Table Bit�������չ�û�  32 --->> 48
//����:	BitR_Table		�����R�� Bit��
//		BitE_Table		�������չ�û�����
//����ֵ:����0�ɹ�	
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
//����:	�����������
//����:	Bit1_Table		����ĵ�һ�� Bit��
//		Bit2_Table		����ĵڶ��� Bit��
//		Bit_Xor			������������Ľ��
//		nBit			�������������λ��
//����ֵ:����0�ɹ�	
int DES_XOR(const unsigned char* Bit1_Table, const unsigned char* Bit2_Table, unsigned char* Bit_Xor/*�������Ľ��*/, int nBit/*��������λ��*/)
{
	int ret = 0;
	for (int i = 0; i < nBit; i++)
	{
		Bit_Xor[i] = Bit1_Table[i] ^ Bit2_Table[i];
	}

	return ret;
}

///////////////////////////////////////////////////////////////
//����:	��Bit_Integer Bit�����P�û�  
//����:	Bit_Integer		�����S�н������Bit��
//		BitP_Table		�����P�û�����
//����ֵ:����0�ɹ�	
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
//����:	�����ݽ���F��������
//����:	BitR_Table		�����R Bit��
//		SubKey			���������Կ Bit��
//		BitP_Table		�����P���û��Ľ��(F�������մ�����)
//����ֵ:����0�ɹ�	
int DES_F_Function(const unsigned char* BitR_Table, const unsigned char* SubKey, unsigned char* BitP_Table/*���P���û��Ľ��*/)
{
	int ret = 0;

	//1���������
	unsigned char BitE_Table[48];		//E(R)��չ�û����
	unsigned char Bit_Xor[8][6];		//����������Ľ��
	unsigned char Row;					//S�е��к�
	unsigned char Col;					//S�е��к�
	unsigned char Integer;				//��S����ȡ�õ�32λ����
	unsigned char Bit_Integer[8][4];	//���������Bitλ

	//2��������ʼ��
	memset(BitE_Table, 0, sizeof(BitE_Table));
	memset(Bit_Xor, 0, sizeof(Bit_Xor));
	Row = 0;
	Col = 0;
	Integer = 0;
	memset(Bit_Integer, 0, sizeof(Bit_Integer));

	//3��������չ�û�		32 -->> 48
	E_Substitution(BitR_Table, BitE_Table);

	//4�����û���Ľ���봫�������Կ�����������
	DES_XOR(BitE_Table, SubKey, (unsigned char*)Bit_Xor, 48);

	//5���������Ľ������8��S��ѹ���û�
	for (int i = 0; i < 8; i++)
	{
		//(1)������S�е��кź��к�
		Row = (Bit_Xor[i][0] << 1) + Bit_Xor[i][5];
		Col = (Bit_Xor[i][1] << 3) + (Bit_Xor[i][2] << 2) + (Bit_Xor[i][3] << 1) + Bit_Xor[i][4];

		//(2)����S����ȡ������
		Integer = S_Table[i][Row][Col];
		
		//(3)����ȡ�õ�4Bit��ת����Bit��
		for (int j = 0; j < 4; j++)
		{
			Bit_Integer[i][j] = Integer >> (3 - j) & 1;
		}
	}

	//6������P�û�
	P_Substitution((unsigned char *)Bit_Integer, BitP_Table);

	return ret;
}

///////////////////////////////////////////////////////////////
//����:	��L��R����ϲ���Ľ���������ʼֵ�û�
//����:	BitRL_Table		�����Bit��
//		Bit_reIP_Table	��������ʼֵ�û��Ľ��
//����ֵ:����0�ɹ�	
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
//����:	���ַ�������DES����
//����:	PlainText		����������ַ���
//		PlainText		�������Կ�ַ���
//		CipherText		���������
//����ֵ:����0�ɹ�	
int DES_EnCryption(unsigned char *PlainText, unsigned char *PassWord, unsigned char *CipherText)
{
	int ret = 0;

	//1���������
	unsigned char BitPlain[64];			//����Bit��
	unsigned char BitKey[64];		    //��ԿBit��
	unsigned char BitSubKey[16][48];	//����ԿBit��
	unsigned char Bit_IP_Table[64];		//��ʼ�û�������ı�
	unsigned char BitL_Table[17][32];	//L��Bit��
	unsigned char BitR_Table[17][32];	//R��Bit��
	unsigned char BitRL_Table[64];		//�����ϲ����LR��
	unsigned char Bit_F_Out[32];		//F����������Ľ��Bit��
	unsigned char Bit_reIP_Table[64];	//���ʼ�û�������ı�

	//2��������ʼ��
	memset(BitPlain,		0, sizeof(BitPlain));
	memset(BitKey,			0, sizeof(BitKey));
	memset(BitSubKey,		0, sizeof(BitSubKey));
	memset(Bit_IP_Table,	0, sizeof(Bit_IP_Table));
	memset(BitL_Table,		0, sizeof(BitL_Table));
	memset(BitR_Table,		0, sizeof(BitR_Table));
	memset(BitRL_Table,		0, sizeof(BitRL_Table));
	memset(Bit_F_Out,		0, sizeof(Bit_F_Out));
	memset(Bit_reIP_Table,	0, sizeof(Bit_reIP_Table));

	//3�����ַ���ת����Bit��
	ByteToBit(PlainText, BitPlain);
	ByteToBit(PassWord , BitKey);

	//4������Password Bit����ɵ�16������Կ
	CalculateSubKey(BitKey, BitSubKey);

	//5��������Bit����г�ʼIP�û�
	IP_Substitution(BitPlain, Bit_IP_Table);

	//6�����û���Ľ����λ L��R����		����ǰ�����
	memcpy(BitL_Table[0], Bit_IP_Table,			32);
	memcpy(BitR_Table[0], &Bit_IP_Table[32],	32);

	//7��16�ֵ�ѭ������
	for (int i = 0; i < 16; i++)
	{
		//(1)����R�������Կ�����F��������
		DES_F_Function(BitR_Table[i], BitSubKey[i], Bit_F_Out);

		//(2)��L���F�����������������������
		DES_XOR(BitL_Table[i], Bit_F_Out, BitR_Table[i + 1], 32);

		//(3)��Li+1 = Ri
		memcpy(BitL_Table[i + 1], BitR_Table[i], 32);
	}

	//8��L[16]��R[16]���н���ϲ�
	memcpy(BitRL_Table,			BitR_Table[16], 32);
	memcpy(&BitRL_Table[32],	BitL_Table[16], 32);

	//9��������ϲ���Ľ���������ʼֵ�û�	��������Bit��
	reIP_Substitution(BitRL_Table, Bit_reIP_Table);

	//10��������Bit��ת��������16�����ַ���
	BitToHex(Bit_reIP_Table, CipherText);

	return ret;
}


///////////////////////////////////////////////////////////////
//����:	��SrcBitGroup����nBit��DesBitGroup��
//����:	SrcBitGroup		�������תǰ�Ľ��
//		DesBitGroup		�������ת��Ľ��
//		nBit			���Ƶ�λ��
//����ֵ:����0�ɹ�	
int BitRound_R(const unsigned char* SrcBitGroup, unsigned char* DesBitGroup, int nBit)
{
	memcpy(&DesBitGroup[nBit],	SrcBitGroup,				(28 - nBit));
	memcpy(DesBitGroup,			&SrcBitGroup[28 - nBit],	nBit);

	return 0;
}


///////////////////////////////////////////////////////////////
//����:	����BitKey��Ӧ��16��������Կ----���ڽ���	
//����:	BitKey			�������ԿBit��
//		BitSubKey		�����16��������Կ  16*48
//����ֵ:����0�ɹ�	
int CalculateReSubKey(const unsigned char *BitKey, unsigned char(*BitReSubKey)[48])
{
	int ret = 0;

	//1���������
	unsigned char BitKey_PC_1[56];		//56λPC-1�û������Կ
	unsigned char BitC_Table[16][28];	//16��28λC��	  ���ڵ�һ�鲻��Ҫ�ƶ������Աȼ���ʱ����һ��
	unsigned char BitD_Table[16][28];	//16��28λD��	  ���ڵ�һ�鲻��Ҫ�ƶ������Աȼ���ʱ����һ��
	unsigned char BitCD_Table[56];		//�ϲ���CD��

	//2��������ʼ��
	memset(BitKey_PC_1,	0, sizeof(BitKey_PC_1));
	memset(BitC_Table,	0, sizeof(BitC_Table));
	memset(BitD_Table,	0, sizeof(BitD_Table));
	memset(BitCD_Table, 0, sizeof(BitCD_Table));

	//3������PC-1��ʼ��Կ�û�
	PC_1_Substitution(BitKey, BitKey_PC_1);

	//4�����û���Ľ���ֳ�C��D����
	memcpy(BitC_Table[0], BitKey_PC_1,		28);
	memcpy(BitD_Table[0], &BitKey_PC_1[28], 28);

	//5���ڲ�������ת�������ֱ�ӽ�PC-1�Ľ������PC-2�û������ɵ�һ��������Կ
	PC_2_Substitution(BitKey_PC_1, BitReSubKey[0]);

	//6������15��ѭ������
	for (int i = 0; i < 15; i++)
	{
		//(1)����C��D����������Ʋ���		�γ�����������Կ������ͬ˳���෴����Կ
		BitRound_R(BitC_Table[i], BitC_Table[i + 1], Bit_Round[16 - (i + 1)]);
		BitRound_R(BitD_Table[i], BitD_Table[i + 1], Bit_Round[16 - (i + 1)]);


		//(2)������ת��Ľ���ϲ�
		memcpy(BitCD_Table,			BitC_Table[i + 1], 28);
		memcpy(BitCD_Table + 28,	BitD_Table[i + 1], 28);

		//(3)�����ϲ����Bit�����PC-2�û�
		PC_2_Substitution(BitCD_Table, BitReSubKey[i + 1]);
	}

	return ret;
}


///////////////////////////////////////////////////////////////
//����:	���ַ�������DES����
//����:	CipherText		����������ַ���
//		PlainText		�������Կ�ַ���
//		DeCipherText	��������� 
//����ֵ:����0�ɹ�	
int DES_DeCipher(const unsigned char *CipherText, const unsigned char* PassWord, unsigned char *DeCipherText)
{
	int ret = 0;

	//1���������
	unsigned char BitCipher[64];		//�����ܵ�����Bit��
	unsigned char BitKey[64];			//��Կ��Bit��
	unsigned char BitReSubKey[16][48];	//������Կ��
	unsigned char Bit_IP_Table[64];		//��ʼ�û�������ı�
	unsigned char BitL_Table[17][32];	//L��Bit��
	unsigned char BitR_Table[17][32];	//R��Bit��
	unsigned char BitRL_Table[64];		//�����ϲ����LR��
	unsigned char Bit_F_Out[32];		//F����������Ľ��Bit��
	unsigned char Bit_reIP_Table[64];	//���ʼ�û�������ı�

	//2��������ʼ��
	memset(BitCipher,		0, sizeof(BitCipher));
	memset(BitKey,			0, sizeof(BitKey));
	memset(BitReSubKey,		0, sizeof(BitReSubKey));
	memset(Bit_IP_Table,	0, sizeof(Bit_IP_Table));
	memset(BitL_Table,		0, sizeof(BitL_Table));
	memset(BitR_Table,		0, sizeof(BitR_Table));
	memset(BitRL_Table,		0, sizeof(BitRL_Table));
	memset(Bit_F_Out,		0, sizeof(Bit_F_Out));
	memset(Bit_reIP_Table,	0, sizeof(Bit_reIP_Table));

	//3�������ĺ���Կת����Bit��
	HexToBit(CipherText, BitCipher);
	ByteToBit(PassWord,  BitKey);

	//4������16��������Կ	--->>Ҳ���ǽ�������Կ��˳�򷴹�����Ϊ�˱��ֽ��ܸ�ʽһ��
	CalculateReSubKey(BitKey, BitReSubKey);	//����������

	//5��������Bit����г�ʼIP�û�
	IP_Substitution(BitCipher, Bit_IP_Table);

	//6�����û���Ľ����λ L��R����		����ǰ�����
	memcpy(BitL_Table[0], Bit_IP_Table,		 32);
	memcpy(BitR_Table[0], &Bit_IP_Table[32], 32);

	//7��16�ֵ�ѭ������
	for (int i = 0; i < 16; i++)
	{
		//(1)����R���������Կ�����F��������
		DES_F_Function(BitR_Table[i], BitReSubKey[i], Bit_F_Out);

		//(2)��L���F�����������������������
		DES_XOR(BitL_Table[i], Bit_F_Out, BitR_Table[i + 1], 32);

		//(3)��Li+1 = Ri
		memcpy(BitL_Table[i + 1], BitR_Table[i], 32);
	}

	//8��L[16]��R[16]���н���ϲ�
	memcpy(BitRL_Table, BitR_Table[16], 32);
	memcpy(&BitRL_Table[32], BitL_Table[16], 32);

	//9��������ϲ���Ľ���������ʼֵ�û�	��������Bit��
	reIP_Substitution(BitRL_Table, Bit_reIP_Table);

	//10��������Bit��ת���������ַ���
	BitToByte(Bit_reIP_Table, DeCipherText);

	return ret;
}

int main()
{
	int ret = 0; 
	
	//1���������
	unsigned char PlainText[9];				//Ҫ���ܵ�����(String)
	unsigned char PassWord[9];				//���ܵ���Կ(String)
	unsigned char CipherText[16];			//����(Hex)
	unsigned char DeCipherText[9];			//���ܺ������(String)

	//2����ʼ������
	memset(PlainText, 0, sizeof(PlainText));
	memset(PassWord, 0, sizeof(PassWord));
	memset(CipherText, 0, sizeof(CipherText));
	memset(DeCipherText, 0, sizeof(DeCipherText));

	printf("Please input Plain Text   ��");
	gets(PlainText);
	printf("Please input PassWord     ��");
	gets(PassWord);

	//3������DES����
	DES_EnCryption(PlainText, PassWord, CipherText);

	printf("Plain Text      :%s\n", PlainText);
	printf("PassWord        :%s\n", PassWord);
	printf("CipherText      :");
	Print_Table(CipherText, 16);
	
	//4������DES����
	DES_DeCipher(CipherText, PassWord, DeCipherText);
	printf("DeCipherText    :%s\n", DeCipherText);

	system("pause");
	return ret;
}