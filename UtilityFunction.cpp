#ifndef UTILITY_FUNC_H
#define UTILITY_FUNC_H
#include "Eigen\Eigen"
#include <math.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;
using Eigen::MatrixXd;

//�ⲿ��������
extern int split(const string& str, vector<string>& ret_, string sep = "\t");

//Cal RMSE
double calRMSEUtil(MatrixXd &m, MatrixXd &n)
{
	assert(m.rows() == n.rows());
	assert(m.cols() == n.cols());
	double sumMinRates = 0.0;
	double count = 0.0;
	for (int i = 0; i < m.rows(); i++)
	{
		for (int j = 0; j < m.cols(); j++)
		{
			if (m(i, j) != 0.0)
			{
				sumMinRates += (m(i, j) - n(i, j))*(m(i, j) - n(i, j));
				cout << "sumMinRates" << sumMinRates << "\n";
				cout << "count" << count << "\n";
				count++;
			}
		}
	}
	assert(count > 0);
	return sqrt(sumMinRates / count);
}

//����Matrix ȫ��Ϊĳ��ֵ
void setMatrixValue(MatrixXd& m, int value)
{
	int rowCount = m.rows();
	int colCount = m.cols();

	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < colCount; j++)
		{
			m(i, j) = value;
		}
	}

}

//���Ƽ����д�뵽�ļ���,��CSV��ʽ
void writeRecommendResult(int userId, vector<int> recommendRes, string fileName)
{
	ofstream outFile;
	outFile.open(fileName,ios::app);
	for (int i = 0; i < recommendRes.size(); i++)
	{
		outFile << std::to_string(userId+1) << "," << std::to_string(recommendRes[i]+1) << "\n";
	}
}

//�ѵ�Ӱ��Ϣת��ΪCSV��ʽ
void formatItemToCSV(string sourceFileName, string targetFileName)
{
	ifstream sourceFile;
	sourceFile.open(sourceFileName);
	ofstream targetFile;
	targetFile.open(targetFileName);
	//�����Ӱ���͵�����
	const int movieTypeCount = 19;
	string itemTypeArr[19] = { "Unknown", "Action", "Adventure", "Animation", "Children's", "Comedy",
		"Crime", "Documentray", "Drama", "Fantasy", "Film-Noir", "Horror", "Musical", "Mystery", "Romance",
		"Sci-Fi", "Thriller", "War", "Western" };
	//
	string eachLineStr;
	if (sourceFile.is_open())
	{
		targetFile << "movieId" << "," << "movieTitle" << "," << "realeaseDate" <<","<<"IMDbURL"<<","<<"movieType"<< "\n";
		while (getline(sourceFile, eachLineStr))
		{
			vector<string> splitStrRes;
			split(eachLineStr, splitStrRes, "|");
			for (int i = 0; i <4; i++)
			{
				targetFile << splitStrRes[i] << ",";
			}
			string curType;
			for (int i = 4; i < splitStrRes.size(); i++)
			{
				if (atoi(splitStrRes[i].c_str()) == 1)
				{
					curType += itemTypeArr[i - 4];
					curType += " ";
				}
			}
			targetFile << curType;
			targetFile << "\n";
		}
	}
}
#endif