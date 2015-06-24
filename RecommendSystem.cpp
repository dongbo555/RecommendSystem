#include "Eigen\Eigen"
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using Eigen::MatrixXd;

//对外部函数进行引用
extern void changeFormatToCSV(string inFileName, string outFileName);
extern MatrixXd readRateData(string dataFileName);
extern void slopOneRecResult(MatrixXd& m, int k, string fileName);
extern void writeSVDRecResult(string inFileName,string outFileName, int k = 10);
extern void formatItemToCSV(string sourceFileName, string targetFileName);
extern void slopTrain(string trainFileName);
extern void slopTest(string testFileName);
extern void writeSlopRecResult(string inFileName, string outFileName, int k = 10);
extern vector<int> topKMoviesFromKNN(int userId, int k, MatrixXd& m);
extern void writeKnnRecResult(MatrixXd& m, string outFileName, int k = 10);

int main(int argc,char* argv[])
{
	//writeSVDRecResult("u.data", "udatasvdrec.csv");
	//writeSlopRecResult("u.data", "udatasloprec.csv");
	//MatrixXd m = readRateData("u.data");
	//cout << m << endl;
	//(m, 10, "slopUDataRec.csv");
	//changeFormatToCSV("u.user", "user.csv");
	//formatItemToCSV("u.item", "uitem.csv");
	
	//
	//slop one 算法测试
	//slopTrain("ua.base");
	//slopTest("ua.test");
	
	//KNN算法测试
	//MatrixXd m = readRateData("u.data");
	//topKMoviesFromKNN(0, 10, m);
	//writeKnnRecResult(m, "udataknnrec.csv");

	//定义命令
	char command;
	command = argv[1][0];
	string sourceFileName = argv[2];
	string targetFileName = argv[3];
	MatrixXd m;
	switch (command)
	{
		//slot one 算法
	case 's':
	case 'S':
		writeSlopRecResult(sourceFileName, targetFileName);
		break;
	case 'k':
	case 'K':
		m = readRateData(sourceFileName);
		//topKMoviesFromKNN(0, 10, m);
		writeKnnRecResult(m, targetFileName);
		break;
	case 'v':
	case 'V':
		writeSVDRecResult(sourceFileName, targetFileName);
		break;
	case 'u':
	case 'U':
		changeFormatToCSV(sourceFileName, targetFileName);
		break;
	case 'I':
	case 'i':
		formatItemToCSV(sourceFileName, targetFileName);
		break;
	default:
		break;
	}
	return 0;
}