#ifndef __FILEREADFUNC_H__
#define __FILEREADFUNC_H__

/*
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>*/
#include <osgDB/ReadFile>

/*!*********************************************************
<br> ��˵�����ļ���Ϣ��ȡ��
*************************************************************/
#pragma pack(push,1)
typedef struct HeadOfDbf {
	unsigned char  tab;
	unsigned char  year;
	unsigned char  month;
	unsigned char  day;
	unsigned int   recnum;
	unsigned short offset;
	unsigned short reclen;
	char  save[20];
} InfoOfDbfHead;
typedef struct HeadOfDbfIndex {
	unsigned char fname[11];
	unsigned char type;
	int  addr;
	unsigned char width;
	unsigned char doc;
	char save[14];
} InfoOfDbfIndex;
#pragma pack(pop)
/*InfoOfDbfHead      Head;
InfoOfDbfIndex     Index[48];
int offset[48],n;
FILE            *fdbf;
unsigned int    rec,field,fieldnum,i;
char buf[256];*/

class KLValueOfFile
{
public:
	/*!��ȡ�ļ��Ĵ�С*/
	static long sizeOfFile(FILE* file)
	{
		long a,b;
		a = ftell(file);
		fseek(file, 0, 2);
		b = ftell(file);
		fseek(file, a, 0);

		return b;
	}

	/*!��BigEndian��ʽ�洢��64λ��8���ֽ�ת��Ϊdouble��*/
	static double littleEndianDoubleRead(char* fileBuf, int startIndex)
	{
		double convert;
		char* add = new char;
		int j = -1;
		for(int i = startIndex; i < startIndex + 8; i++)
		{
			j++;
			add[j] = fileBuf[i];
		}

		convert = *reinterpret_cast<double* const>(add);

		return convert;
	}

	/*!��BigEndian��ʽ�洢��32λ��4���ֽ�ת��Ϊint��*/
	static int littleEndianIntRead(char* fileBuf, int startIndex)
	{
		int convert;
		char* add = new char;
		int j = -1;
		for(int i = startIndex; i < startIndex + 4; i++)
		{
			j++;
			add[j] = fileBuf[i];
		}

		convert = *reinterpret_cast<int* const>(add);

		return convert;
	}

	/*!��txt�ļ��ж�ȡ��ֵ�ߵĸ߳���Ϣ*/
	static void readContourHeightFromTxt(const std::string& fileName, 	std::vector<float>& pvalues);


	/*!��ȡdbf�ļ�����*/
	static void readDbfFunc(const std::string& fileName, std::vector<float>& pvalues);

};

#endif