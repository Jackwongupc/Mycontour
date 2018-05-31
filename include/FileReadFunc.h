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
<br> 类说明：文件信息读取类
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
	/*!获取文件的大小*/
	static long sizeOfFile(FILE* file)
	{
		long a,b;
		a = ftell(file);
		fseek(file, 0, 2);
		b = ftell(file);
		fseek(file, a, 0);

		return b;
	}

	/*!以BigEndian格式存储的64位或8个字节转换为double型*/
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

	/*!以BigEndian格式存储的32位或4个字节转换为int型*/
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

	/*!从txt文件中读取等值线的高程信息*/
	static void readContourHeightFromTxt(const std::string& fileName, 	std::vector<float>& pvalues);


	/*!读取dbf文件方法*/
	static void readDbfFunc(const std::string& fileName, std::vector<float>& pvalues);

};

#endif