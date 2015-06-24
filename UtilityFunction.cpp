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

//外部函数引入
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

//设置Matrix 全部为某个值
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

//把推荐结果写入到文件中,以CSV格式
void writeRecommendResult(int userId, vector<int> recommendRes, string fileName)
{
	ofstream outFile;
	outFile.open(fileName,ios::app);
	for (int i = 0; i < recommendRes.size(); i++)
	{
		outFile << std::to_string(userId+1) << "," << std::to_string(recommendRes[i]+1) << "\n";
	}
}

//把电影信息转换为CSV格式
void formatItemToCSV(string sourceFileName, string targetFileName)
{
	ifstream sourceFile;
	sourceFile.open(sourceFileName);
	ofstream targetFile;
	targetFile.open(targetFileName);
	//定义电影类型的数组
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