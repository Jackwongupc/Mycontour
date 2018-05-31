//#include <iostream>

#include "FileReadFunc.h"

void KLValueOfFile::readContourHeightFromTxt(const std::string& fileName, 	std::vector<float>& pvalues)
{
	size_t dotPosition = fileName.find_last_of('.');
	if(dotPosition == std::string::npos)
	{
		std::cerr<<"文件名字有错误"<<std::endl;
		return;
	}
	std::string fileBaseName = fileName.substr(0,dotPosition);
	std::string fileNameOfTxt = fileBaseName + ".txt";

	std::ifstream file(fileNameOfTxt.c_str());
	std::string line;
	unsigned int id;
	float height;
	std::getline(file, line);
	unsigned int numOfLine = 0;

	while(!file.eof())
	{
		std::getline(file, line);
		std::istringstream iss(line);
		iss >> id >>height;
		pvalues.push_back(height);
		numOfLine++;
	}

	file.close();
}


void KLValueOfFile::readDbfFunc(const std::string& fileName, std::vector<float>& pvalues)
{
	size_t dotPosition = fileName.find_last_of('.');
	if(dotPosition == std::string::npos)
	{
		std::cerr<<"文件名字有错误"<<std::endl;
		return;
	}
	std::string fileBaseName = fileName.substr(0,dotPosition);
	std::string fileNameOfDbf = fileBaseName + ".dbf";

	InfoOfDbfHead      Head;
	InfoOfDbfIndex     Index[48];
	int offset[48],n;
	FILE            *fdbf;
	unsigned int    rec,field,fieldnum,i;
	char buf[256];

	const char* tempbuf = fileNameOfDbf.c_str();
	strcpy(buf, tempbuf);
	if ((fdbf=fopen(buf,"rb"))==NULL) {
		fprintf(stderr,"Can not fopen file [%s]!",buf);
	}
	fseek(fdbf,0L,SEEK_SET);
	fread(&Head,sizeof(InfoOfDbfHead),1,fdbf);
	fieldnum=(Head.offset-1)/32-1;

	fseek(fdbf,32,SEEK_SET);
	n=0;
	printf("|");
	for(i=0;i<fieldnum;i++) {
		offset[i]=n;
		fread(&Index[i],sizeof(InfoOfDbfIndex),1,fdbf);
		printf("%-*s|",Index[i].width,Index[i].fname);
		n=n+Index[i].width;
	}
	int num = 0;
	for (rec=0;rec<Head.recnum;rec++) {
		printf("\n|");
		for (field=0;field<fieldnum;field++) {
			fseek(fdbf,(long)(Head.offset+1+rec*(long)Head.reclen+offset[field]),SEEK_SET);
			fread(buf,1,Index[field].width,fdbf);
			buf[Index[field].width]=0;
			printf("%s|",buf);
			int cnt = 0;
			float value = 0.0;
			if(num%2 == 0)
				cnt = atoi(buf);
			else
			{
				value = atof(buf);
				pvalues.push_back(value);
			}				
			num++;
		}
	}
	fclose(fdbf);
}