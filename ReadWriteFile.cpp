#ifndef READWRITEFILE_H
#define READWRITEFILE_H
#include <string>
#include <vector>
#include "Eigen\Eigen"
#include <fstream>
#include <set>
#include <iostream>

using Eigen::MatrixXd;
using namespace std;
extern void setMatrixValue(MatrixXd& m, int value);
// 对string 进行split,默认以逗号进行分割
int split(const string& str, vector<string>& ret_, string sep = "\t")
{
	if (str.empty())
	{
		return 0;
	}

	string tmp;
	string::size_type pos_begin = str.find_first_not_of(sep);
	string::size_type comma_pos = 0;

	while (pos_begin != string::npos)
	{
		comma_pos = str.find(sep, pos_begin);
		if (comma_pos != string::npos)
		{
			tmp = str.substr(pos_begin, comma_pos - pos_begin);
			pos_begin = comma_pos + sep.length();
		}
		else
		{
			tmp = str.substr(pos_begin);
			pos_begin = comma_pos;
		}

		if (!tmp.empty())
		{
			ret_.push_back(tmp);
			tmp.clear();
		}
	}
	return 0;
}

//读取u.data文件得到评分矩阵

MatrixXd readRateData(string dataFileName)
{
	ifstream dataFile;
	dataFile.open(dataFileName);
	//const int line_length = 1024;

	//读取userId ItemId 至set中
	set<int> userIds;
	set<int> itemIds;
	vector<vector<string> > fileData;
	if (dataFile.is_open())
	{
		//char eachLine[line_length];
		string eachLineStr;
		while (getline(dataFile, eachLineStr))
		{
			vector<string> splitStrRes;
			split(eachLineStr, splitStrRes, "\t");
			if (splitStrRes.size() == 4)
			{
				//user id | item id | rating | timestamp
				userIds.insert(std::atoi(splitStrRes[0].c_str()));
				itemIds.insert(std::atoi(splitStrRes[1].c_str()));
				fileData.push_back(splitStrRes);

			}
		}
	}

	//设置评分矩阵
	MatrixXd m;
	//int userIdCount = userIds.size();
	//int itemIdCount = itemIds.size();
	int userIdCount = 943;
	int itemIdCount = 1682;
	m.resize(userIdCount, itemIdCount);
	setMatrixValue(m, 0);
	for (int i = 0; i < fileData.size(); i++)
	{
		int row = atoi(fileData[i][0].c_str());
		int col = atoi(fileData[i][1].c_str());
		int rate = atoi(fileData[i][2].c_str());
		//cout << row << " " << col << " " << rate << "\n";
		m(row - 1, col - 1) = rate;
	}
	return m;
}

//Change Format to CSV
void changeFormatToCSV(string inFileName, string outFileName)
{
	ifstream inFile;
	inFile.open(inFileName);
	string eachLineStr;

	ofstream outFile;
	outFile.open(outFileName, ios::out);
	if (inFile.is_open())
	{
		outFile << "userId" << "," << "age" << "," << "gender" << "," << "occupation" << "\n";
		while (getline(inFile, eachLineStr))
		{
			vector<string> splitStrRes;
			split(eachLineStr, splitStrRes, "|");
			for (int i = 0; i < splitStrRes.size() - 1; i++)
			{
				if (i == splitStrRes.size() - 2)
				{
					outFile << splitStrRes[i];
				}
				else
				{
					outFile << splitStrRes[i] << ",";
				}
			}
			outFile << "\n";
		}
	}
	else
	{
		cout << inFileName << "open file error！";
	}
	inFile.close();
	outFile.close();
}

#endif