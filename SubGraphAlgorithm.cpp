#include "HeaderFile.h"
using namespace std;

struct Edge;
class subgraph;

map<int, int> SetOfNode;
map<int, bool> flag;
set<int> vertex;
vector<subgraph> subgraphs;

pair<vector<string>, bool> split(const string&);
void init();
void dfs(int, set<int> &);
void dijkstra(int, int);
struct Edge
{
    int point;
    double weight;
    Edge(int point = 0, double w = -1) : point(point), weight(w){}
};
class subgraph {
public:
    map<int, vector<Edge>> real_edges;
    map<int, vector<Edge>> virtual_edges;
    set<int> nodes;
    subgraph(const string &filename)
    {
        map<pair<int, int>, double> real_edge;       // 解决重边的问题 
        map<pair<int, int>, double> virtual_edge;    
        string str;
        fstream fs;
        fs.open(filename, ios::in);
        if (!fs) {cout << "fail to open the file" << endl; exit(-1);}
        while (getline(fs, str))
        {
            auto str_split = split(str);
            if (str_split.first.empty()) continue;
            if (str_split.first.size() == 1) 
            {
                int node = convert<string, int>(str_split.first[0]);
                nodes.insert(node);
                vertex.insert(node);
            }
            else if (str_split.first.size() == 3)
            {
                int from = convert<string, int>(str_split.first[0]);
                int to = convert<string, int>(str_split.first[1]);
                double weight = convert<string, double>(str_split.first[2]);
                nodes.insert(from);
                vertex.insert(from);
                vertex.insert(to);
                if (!str_split.second)
                {
                    if (real_edge.count({from, to})) {
                        real_edge[{from, to}] = min(weight, real_edge[{from, to}]);
                    }
                    else {
                        real_edge[{from, to}] = weight;
                    }
                }
                else 
                {
                    if (virtual_edge.count({from, to})) {
                        virtual_edge[{from, to}] = min(weight, virtual_edge[{from, to}]);
                    }
                    else {
                        virtual_edge[{from, to}] = weight;
                    }
                }
            }
        }
        for (auto i : real_edge)
        {
            auto temp = i.first;
            real_edges[temp.first].push_back(Edge(temp.second, i.second));
        }
        for (auto i : virtual_edge)
        {
            auto temp = i.first;
            virtual_edges[temp.first].push_back(Edge(temp.second, i.second));
        }
    }
    set<int> GetNodes() const {return nodes;}
};

int main()
{
    // 获取所有子图的文件名
    vector<string> filename_vec;
    DIR* d = opendir("./SubGraph");
    if (d == NULL)
    {
        cerr << "error : This directory does not exist!" << endl;
        exit(-1);
    }
    struct dirent* entry;
    while ((entry = readdir(d)) != NULL)
    {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
        {
            string temp = "./SubGraph/" + string(entry->d_name);
            filename_vec.push_back(temp);
        }
    }
    closedir(d);

    for (int i = 0; i < filename_vec.size(); i ++)
    {
        subgraph temp(filename_vec[i]);
        for (auto j : temp.GetNodes()) 
        {
            SetOfNode[j] = i;
        }
        subgraphs.push_back(temp);
    }
    // Test2
    cout << "Part 2 Test : \n\n";
    while (true)
    {
        int op;
        int A, B;
        cout << "请输入测试类型 : [1] | [2] | [3]: \n";
        cout << "[1] 表示指定一个点，列出所有经有向边可达的结点 \n";
        cout << "[2] 表示指定两点，输出最短路径 \n";
        cout << "[3] 表示退出 \n"; 
        cin >> op;
        cout << endl;
        if (op == 1)
        {
            set<int> AccessibleNodes;
            cout << "请输入结点编号 : \n";
            cin >> A;
            if (!vertex.count(A)) {cout << "节点不存在" << endl; continue;}
            init();
            dfs(A, AccessibleNodes);
            if (AccessibleNodes.size()) 
            {
                cout << "可达结点有 : \n";
                for (auto i : AccessibleNodes)  cout << i << endl;
            }
            else cout << "无可达结点 \n";
        }
        else if (op == 2)
        {
            cout << "请输入两被测结点的编号 : \n";
            cin >> A >> B;
            if (!vertex.count(A) || !vertex.count(B)) {cout << "节点不存在" << endl; continue;}
            init();
            dijkstra(A, B);
        }
        else if (op == 3) break;
        else cout << "输入有误， 请重新输入\n";
    }
}

pair<vector<string>, bool> split(const string& str)
{
	vector<string> ans;
    bool flag = false;
    for (int i = 0; i < str.size(); i ++) 
    {
        if (str[i] == '*') flag = true;
    }
	for(int i = 0; i < str.length(); i++)
    {
		while(i < str.length() && is_split(str[i])) i++;
		int prev_i = i;
		while(i < str.length() && is_num(str[i])) i++;
		if(i > prev_i)
		{
            ans.push_back(str.substr(prev_i, i - prev_i));
        }
	}
	return {ans, flag};
}
void dijkstra(int st, int ed)
{
    map<int, int> prev;
    map<int, double> dist;
    priority_queue<pair<int, double>, vector<pair<int, double>>, cmp> heap;
    for (auto i : vertex) dist[i] = 0x3f3f3f3f;
    prev[st] = -1;
    dist[st] = 0;
    heap.push({st, 0});
    while (heap.size())
    {
        auto temp = heap.top(); heap.pop();
        int t = temp.first;
        double dis = temp.second;
        if (flag[t]) continue;
        double min_length = 0x3f3f3f3f;
        flag[t] = true;
        int pos_ = SetOfNode[t];
        for (auto j : subgraphs[pos_].real_edges[t])
        {
            int p = j.point;
            if (dist[p] > dis + j.weight)
            {
                dist[p] = dis + j.weight;
                prev[p] = t;
                heap.push({p, dist[p]});
            }
        }
        for (auto j : subgraphs[pos_].virtual_edges[t])
        {
            int p = j.point;
            if (dist[p] > dis + j.weight)
            {
                dist[p] = dis + j.weight;
                prev[p] = t;
                heap.push({p, dist[p]});
            }
        }
    }
    
    if (dist[ed] == 0x3f3f3f3f)  
    {
        cout << "从结点 " << st << " 无法到达结点 " << ed << ". \n" << endl; return ;}
    else
    {
        cout << "\n最短距离为 : " << fixed <<  setprecision(4) << dist[ed];
        cout << "\n对应路线为 : " << endl;
        vector<int> route;
        int node = ed;
        route.push_back(node);
        while (prev[node] != -1) 
        {
            route.push_back(prev[node]);
            node = prev[node];
        }
        cout << st;
        for (int i = route.size() - 2; i >= 0; i --)
        {
            cout << " -> " << route[i];
        }
        cout << endl;
        return ;
    }
}
void init()
{
    for (auto i : vertex)   flag[i] = false;
}
void dfs(int node, set<int> &AccessibleNodes)
{
    int pos_ = SetOfNode[node];
    flag[node] = true;
    // 实边
    for (auto i : subgraphs[pos_].real_edges[node])
    {
        int p = i.point;
        if (!flag[p]) 
        {
            AccessibleNodes.insert(p);
            dfs(p, AccessibleNodes);
        }
    }
    // 虚边
    for (auto i : subgraphs[pos_].virtual_edges[node])
    {
        int p = i.point;
        if (!flag[p])
        {
            AccessibleNodes.insert(p);
            dfs(p, AccessibleNodes);
        }
    }
    return ;
}