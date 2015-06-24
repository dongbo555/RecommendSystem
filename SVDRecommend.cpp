#ifndef SVD_RECOMMEND
#define SVD_RECOMMEND

#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;
using std::sort;
//引入外部变量定义
typedef pair<int, double> PAIR;
extern bool cmp_by_value(const PAIR& lhs, const PAIR& rhs);
extern void writeRecommendResult(int userId, vector<int> recommendRes, string fileName);
//

//943 users
//1682 items
//100000 ratings
const int USERCOUNT = 943;
const int ITEMCOUNT = 1682;
const int USERMAX = 1000;
const int ITEMMAX = 2000;
const int FEATURE = 50;//特征
const int ITERMAX = 20;
double rating[USERMAX][ITEMMAX];
int I[USERMAX][ITEMMAX];//indicate if the item is rated
double UserF[USERMAX][FEATURE];
double ItemF[ITEMMAX][FEATURE];
double BIASU[USERMAX];
double BIASI[ITEMMAX];
double lamda = 0.15;
double gamma = 0.04;
double mean;

double svdPredict(int i, int j)
{
	double rate = mean + BIASU[i] + BIASI[j];
	for (int f = 0; f < FEATURE; f++)
		rate += UserF[i][f] * ItemF[j][f];

	if (rate < 1)
		rate = 1;
	//else if (rate>5)
		//rate = 5;
	return rate;
}

double calRMSE()
{
	int cnt = 0;
	double total = 0;
	for (int i = 0; i < USERMAX; i++)
	{
		for (int j = 0; j < ITEMMAX; j++)
		{
			double rate = svdPredict(i, j);
			total += I[i][j] * (rating[i][j] - rate)*(rating[i][j] - rate);
			cnt += I[i][j];
		}
	}
	double rmse = pow(total / cnt, 0.5);
	return rmse;
}
double calMean()
{
	double total = 0;
	int cnt = 0;
	for (int i = 0; i < USERMAX; i++)
		for (int j = 0; j < ITEMMAX; j++)
		{
			total += I[i][j] * rating[i][j];
			cnt += I[i][j];
		}
	return total / cnt;
}
void initBias()
{
	memset(BIASU, 0, sizeof(BIASU));
	memset(BIASI, 0, sizeof(BIASI));
	mean = calMean();
	for (int i = 0; i < USERMAX; i++)
	{
		double total = 0;
		int cnt = 0;
		for (int j = 0; j < ITEMMAX; j++)
		{
			if (I[i][j])
			{
				total += rating[i][j] - mean;
				cnt++;
			}
		}
		if (cnt > 0)
			BIASU[i] = total / (cnt);
		else
			BIASU[i] = 0;
	}
	for (int j = 0; j < ITEMMAX; j++)
	{
		double total = 0;
		int cnt = 0;
		for (int i = 0; i < USERMAX; i++)
		{
			if (I[i][j])
			{
				total += rating[i][j] - mean;
				cnt++;
			}
		}
		if (cnt > 0)
			BIASI[j] = total / (cnt);
		else
			BIASI[j] = 0;
	}
}
void svdTrain(string trainFileName)
{
	//read rating matrix
	memset(rating, 0, sizeof(rating));
	memset(I, 0, sizeof(I));
	ifstream in(trainFileName);
	if (!in)
	{
		cout << "file not exist" << endl;
		exit(1);
	}
	int userId, itemId, rate;
	string timeStamp;
	while (in >> userId >> itemId >> rate >> timeStamp)
	{
		rating[userId][itemId] = rate;
		I[userId][itemId] = 1;
	}
	initBias();

	//train matrix decomposation
	for (int i = 0; i < USERMAX; i++)
		for (int f = 0; f < FEATURE; f++)
			UserF[i][f] = (rand() % 10) / 10.0;
	for (int j = 0; j < ITEMMAX; j++)
		for (int f = 0; f < FEATURE; f++)
			ItemF[j][f] = (rand() % 10) / 10.0;

	int iterCnt = 0;
	while (iterCnt < ITERMAX)
	{
		for (int i = 0; i < USERMAX; i++)
		{

			for (int j = 0; j < ITEMMAX; j++)
			{
				if (I[i][j])
				{

					double predictRate = svdPredict(i, j);
					double eui = rating[i][j] - predictRate;
					BIASU[i] += gamma*(eui - lamda*BIASU[i]);
					BIASI[j] += gamma*(eui - lamda*BIASI[j]);
					for (int f = 0; f < FEATURE; f++)
					{
						UserF[i][f] += gamma*(eui*ItemF[j][f] - lamda*UserF[i][f]);
						ItemF[j][f] += gamma*(eui*UserF[i][f] - lamda*ItemF[j][f]);
					}
				}

			}

		}
		double rmse = calRMSE();
		cout << "Loop " << iterCnt << " : rmse is " << rmse << endl;
		iterCnt++;
	}

}

void svdTest(string testFileName)
{
	ifstream in(testFileName);
	if (!in)
	{
		cout << "file not exist" << endl;
		exit(1);
	}
	int userId, itemId, rate;
	string timeStamp;
	double total = 0;
	double cnt = 0;
	while (in >> userId >> itemId >> rate >> timeStamp)
	{
		double r = svdPredict(userId, itemId);
		total += (r - rate)*(r - rate);
		cnt += 1;
	}
	cout << "test rmse is " << pow(total / cnt, 0.5) << endl;
}

//svd推荐结果写入
//
void writeSVDRecResult(string inFileName,string outFileName, int k = 10)
{
	//
	svdTrain(inFileName);
	ofstream outFile;
	outFile.open(outFileName, ios::trunc);
	outFile.close();
	outFile.open(outFileName);
	outFile << "userID" << "," << "itemId"<<"\n";
	outFile.close();
	for (int i = 1; i <= USERCOUNT; i++)
	{
		//未评分电影预测评分
		map<int, double> predictRates;
		for (int j = 1; j <= ITEMCOUNT; j++)
		{
			if (I[i][j] != 1)
			{
				double  rate = svdPredict(i, j);
				predictRates[j] = rate;
			}
		}
		vector<PAIR> recommendRateVec(predictRates.begin(), predictRates.end());
		sort(recommendRateVec.begin(), recommendRateVec.end(), cmp_by_value);//函数对象
		vector<int> topKItems;
		//选取最大的K个推荐Item作为输出
		for (int i = 0; i < k; i++)
		{
			topKItems.push_back(recommendRateVec[i].first-1);
			//cout << recommendRateVec[i].first << " : " << recommendRateVec[i].second << "\n";
		}
		writeRecommendResult(i-1, topKItems, outFileName);
	}
}

#endif