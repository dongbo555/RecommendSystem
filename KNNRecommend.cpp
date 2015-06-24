#ifndef KNN_RECOMMEND
#define KNN_RECOMMEND
//KNN 推荐算法
#include "Eigen\Eigen"
#include <vector>
#include <math.h>
#include <map>
#include <iostream>
#include <fstream>
using namespace std;
using Eigen::MatrixXd;
typedef pair<int, double> PAIR;
extern bool cmp_by_value(const PAIR& lhs, const PAIR& rhs);
extern double calRMSEUtil(MatrixXd &m, MatrixXd &n);
extern void setMatrixValue(MatrixXd& m, int value);
extern void writeRecommendResult(int userId, vector<int> recommendRes, string fileName);

//获取对某个电影的评分均值
double getAvgRateForMovie(int itemId, MatrixXd& m)
{
	int userCount = m.rows();
	double rateSum = 0.0;
	double rateCount = 0.0;
	for (int i = 0; i < userCount; i++)
	{
		if (m(i, itemId) != 0.0)
		{
			rateSum += m(i, itemId);
			rateCount++;
		}
	}
	return rateSum / rateCount;
}

// 获取对两个电影都评过分的用户集合
vector<int> getUserIdForTwoMovies(int c1, int c2, MatrixXd& m)
{
	int userCount = m.rows();
	vector<int> userVec;
	for (int i = 0; i < userCount; i++)
	{
		if (m(i, c1) != 0.0 && m(i, c2) != 0.0)
			userVec.push_back(i);
	}
	return userVec;
}

//获取用户评过分的电影集合

// step 1
double pearsonCorrMovies(int c1, int c2, MatrixXd& m)
{
	vector<int> userVec = getUserIdForTwoMovies(c1, c2, m);
	double avgRateForC1 = getAvgRateForMovie(c1, m);
	double avgRateForC2 = getAvgRateForMovie(c2, m);

	double pearsonNume = 0.0;
	double pearsonDeno = 0.0;
	double pearSonDenoFirst = 0.0;
	double pearsonDenoSecond = 0.0;
	//对c1 和 c2两个电影都评过分的用户进行遍历
	for (int i = 0; i < userVec.size(); i++)
	{
		int curUserId = userVec[i];
		double t1 = m(curUserId, c1) - avgRateForC1;
		double t2 = m(curUserId, c2) - avgRateForC2;
		pearsonNume += t1*t2;
		pearSonDenoFirst += t1*t1;
		pearsonDenoSecond += t2*t2;
	}
	pearsonDeno = sqrt(pearSonDenoFirst*pearsonDenoSecond);
	//debug info
	//cout << pearsonNume << " " << pearsonDeno << "\n";
	double pearsonCorr = pearsonNume / pearsonDeno;
	//cout << pearsonCorr << "\n";
	return pearsonCorr;
}

//step 2
//在用户u评过分的所有电影中,找出K个与电影m相似度最高的电影
typedef pair<int, double> PAIR;

void getTopKCorrMovies(int userId, int itemId, MatrixXd & m, int k, vector<int>& topKItems, vector<double>& topKRates)
{
	vector<int> ralatedKMovies;
	//电影相似度
	map<int, double> corrMap;

	for (int i = 0; i < m.cols(); i++)
	{
		//用户评过分的电影集合
		if (m(userId, i) != 0.0 && i != itemId)
		{
			double corrVal = pearsonCorrMovies(i, itemId, m);
			corrMap[i] = corrVal;
		}
	}

	//
	extern bool cmp_by_value(const PAIR& lhs, const PAIR& rhs);
	vector<PAIR> corrSortVec(corrMap.begin(), corrMap.end());
	sort(corrSortVec.begin(), corrSortVec.end(), cmp_by_value);//函数对象
	//vector<int> topKItems;
	if (k>corrSortVec.size())
		k = corrSortVec.size();
	for (int i = 0; i < k; i++)
	{
		topKItems.push_back(corrSortVec[i].first);
		topKRates.push_back(corrSortVec[i].second);
	}
	//return topKItems;
}

//根据选择得到的K个相似电影进行预测评分
double predictRateForMovieByKnn(int userId, int itemId, int k, MatrixXd & m)
{

	vector<int> topKItems;
	vector<double> topKCorrVals;
	getTopKCorrMovies(userId, itemId, m, k, topKItems, topKCorrVals);

	double predictNume = 0.0;
	double predictDeno = 0.0;
	for (int i = 0; i < topKItems.size(); i++)
	{
		int curItem = topKItems[i];
		predictNume += topKCorrVals[i] * m(userId, curItem);
		predictDeno += topKCorrVals[i];
	}
	//cout << predictNume / predictDeno;
	return predictNume / predictDeno;
}

//预测评分最高的K个Movies
vector<int> topKMoviesFromKNN(int userId, int k, MatrixXd& m)
{
	//电影预测评分
	map<int, double> movieRates;
	for (int i = 0; i < m.cols(); i++)
	{
		//如果还未评分
		if (m(userId, i) == 0.0)
		{
			double predictRate = predictRateForMovieByKnn(userId, i, k, m);
			movieRates[i] = predictRate;
		}
	}
	//
	vector<PAIR> movieRatesVec(movieRates.begin(), movieRates.end());
	sort(movieRatesVec.begin(), movieRatesVec.end(), cmp_by_value);
	if (k>movieRatesVec.size())
		k = movieRatesVec.size();
	//
	vector<int> topKItems;
	for (int i = 0; i < k; i++)
	{
		topKItems.push_back(movieRatesVec[i].first);
	}
	return topKItems;
}

//对KNN评分预测算法进行测试,计算RMSE
double rmseKNN(MatrixXd& m, int k)
{
	MatrixXd n = m;
	setMatrixValue(n, 0.0);
	for (int i = 0; i < m.rows(); i++)
	{
		for (int j = 0; j < m.cols(); j++)
		{
			if (m(i, j) != 0.0)
			{
				n(i, j) = predictRateForMovieByKnn(i, j, k, m);
				//debug out
				//cout << i << " " << j << " " << m(i, j) << " " << n(i, j) << "\n";
			}
		}
	}
	double rmseVal = calRMSEUtil(m, n);
	//cout << "rmseVal:" << rmseVal << "\n";
	return rmseVal;
}

void writeKnnRecResult(MatrixXd& m, string outFileName, int k = 10)
{
	//
	
	ofstream outFile;
	outFile.open(outFileName, ios::trunc);
	outFile.close();
	outFile.open(outFileName);
	outFile << "userID" << "," << "itemId" << "\n";
	outFile.close();
	for (int i = 0; i <m.rows() ; i++)
	{
		//未评分电影预测评分
		vector<int> topKItems=topKMoviesFromKNN(i,k,m);
		writeRecommendResult(i , topKItems, outFileName);
	}
}


#endif