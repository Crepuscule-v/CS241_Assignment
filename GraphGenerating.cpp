#include "HeaderFile.h"

using namespace std;
// 边权范围
const int MAX_WEIGHT = 100;
const int MIN_WEIGHT = 1;
// 结点编号上下限 
const int MAX_NODE = 200;
const int MIN_NODE = 1;
// TYPE == 1 表示权重为int, TYPE == 2表示权重为double
const int TYPE = 2;

void write(std::fstream &fs, int n)
{
    int from, to;
    while (n --)
    {
        fs << "<";
        // type : 随机生成[0, 4]，0表示生成孤立点或一条边
        int type = rand() % 5;
        if (!type)
        {
            int node = rand() % (MAX_NODE - MIN_NODE) + MIN_NODE;
            fs << node;
        }
        else
        {
            // 可以存在自环
            from = rand() % (MAX_NODE - MIN_NODE) + MIN_NODE;
            to = rand() % (MAX_NODE - MIN_NODE) + MIN_NODE;
            fs << from << " " << to << " ";
            if (TYPE == 1)
            {
                int weight = MIN_WEIGHT + rand() % (MAX_WEIGHT - MIN_WEIGHT);
                fs << weight;
            }
            else
            {
                double weight = MIN_WEIGHT + (double)rand() / ((double)RAND_MAX / (MAX_WEIGHT - MIN_WEIGHT));
                fs << std::fixed << std::setprecision(4) << weight;
            }
        }
        fs << ">" << endl;
    }
}

void CreateNewFile(string filename, int n)
{
    std::fstream fs;
    fs.open(filename, std::ios::out);
    if (!fs) {cout << "fail to open the file" << endl; exit(-1);}
    write(fs, n);
    fs.close();
}

void AppendToExistingFile(const string &filename, int n)
{
    std::fstream fs;
    fs.open(filename, ios::app);
    if (!fs) {cout << "fail to open the file" << endl; exit(-1);}
    write(fs, n);
    fs.close();
};

int main()
{
    std::srand(time(nullptr));
    int type, n;
    string filename;
    string prefix = "./TestGraph/";
    while(true)
    {
        cout << "<----------------------------- \n"
             << "输入模式: \n" 
             << "[0] 表示退出 \n" 
             << "[1] 表示新建图文件 \n"
             << "[2] 表示向已有图文件追加 \n"
             << "请输入: [0] | [1] | [2] \n";
        cin >> type;
        if (type == 0) break;
        else 
        {
            cout << "Please enter the file name :\n";
            cin >> filename;
            cout << "Please enter the number of edges :\n";
            cin >> n;
            if (type == 1) CreateNewFile(prefix + filename, n);
            else if (type == 2) AppendToExistingFile(prefix + filename, n);
            else cout << "Incorrect input format, please retype !";
        }
        cout << "---------------------------->\n\n";
    }
    cout << "Thanks for using!\n";
    return 0;
}
