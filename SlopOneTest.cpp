#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;
using std::sort;
const int SLOPUSERCOUNT = 943;
const int SLOPITEMCOUNT = 1682;
const int SLOPUSERMAX = 1000;
const int SLOPITEMMAX = 2000;
double slopRating[SLOPUSERMAX][SLOPITEMMAX];
int slopI[SLOPUSERMAX][SLOPITEMMAX];//indicate if the item is rated
double slopMean;

//引入外部函数定义
typedef pair<int, double> PAIR;
bool cmp_by_value(const PAIR& lhs, const PAIR& rhs){
	return rhs.second < lhs.second;
}
extern void writeRecommendResult(int userId, vector<int> recommendRes, string fileName);

double slopPredict(int u, int l)
{
	double total = 0;
	double totalCnt = 0;
	for (int i = 0; i < SLOPITEMMAX; i++)
	{
		if (l != i&&slopI[u][i])
		{
			double dev = 0;
			int cnt = 0;
			for (int j = 0; j <SLOPUSERMAX; j++)
			{
				if (slopI[j][l] && slopI[j][i])
				{
					dev += slopRating[j][i] - slopRating[j][l];
					cnt++;
				}
			}
			if (cnt)
			{
				dev /= cnt;
				total += (slopRating[u][i] - dev)*cnt;
				totalCnt += cnt;
			}
		}
	}
	if (totalCnt == 0)
		return slopMean;
	return total / totalCnt;
}
double slopCalMean()
{
	double total = 0;
	int cnt = 0;
	for (int i = 0; i < SLOPUSERMAX; i++)
		for (int j = 0; j < SLOPITEMMAX; j++)
		{
			total += slopI[i][j] * slopRating[i][j];
			cnt += slopI[i][j];
		}
	return total / cnt;
}

void slopTrain(string trainFileName)
{
	//read slopRating matrix
	memset(slopRating, 0, sizeof(slopRating));
	memset(slopI, 0, sizeof(slopI));
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
		slopRating[userId][itemId] = rate;
		slopI[userId][itemId] = 1;
	}
	slopMean = slopCalMean();
}

void slopTest(string testFileName)
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
		double r = slopPredict(userId, itemId);
		cout << "true: " << rate << " predict: " << r << endl;
		total += (r - rate)*(r - rate);
		cnt += 1;
		//cout << total << endl;
	}
	cout << "test rmse is " << pow(total / cnt, 0.5) << endl;
}

//
void writeSlopRecResult(string inFileName, string outFileName, int k = 10)
{
	//
	slopTrain(inFileName);
	ofstream outFile;
	outFile.open(outFileName, ios::trunc);
	outFile.close();
	outFile.open(outFileName);
	outFile << "userID" << "," << "itemId"<<"\n";
	outFile.close();
	for (int i = 1; i <= SLOPUSERCOUNT; i++)
	{
		//未评分电影预测评分
		map<int, double> predictRates;
		for (int j = 1; j <= SLOPITEMCOUNT; j++)
		{
			if (slopI[i][j] != 1)
			{
				double  rate = slopPredict(i, j);
				predictRates[j] = rate;
			}
		}
		vector<PAIR> recommendRateVec(predictRates.begin(), predictRates.end());
		sort(recommendRateVec.begin(), recommendRateVec.end(), cmp_by_value);//函数对象
		vector<int> topKItems;
		//选取最大的K个推荐Item作为输出
		for (int i = 0; i < k; i++)
		{
			topKItems.push_back(recommendRateVec[i].first - 1);
			//cout << recommendRateVec[i].first << " : " << recommendRateVec[i].second << "\n";
		}
		writeRecommendResult(i - 1, topKItems, outFileName);
	}
}
