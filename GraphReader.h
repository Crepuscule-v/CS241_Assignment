#ifndef __GRAPHREADER_H__
#define __GREPHREADER_H__

#include "Graph.h"
const char *SUBGRAPH_FILENAME = "SubGraph.txt";
const int ITERATION_TIMES = 500000;
const int UNUPDATED_TIMES = 1000;    // 小扰动
const int TABU_SETPSIZE = 20;
const int MOVE_FREQUENCE = 10;
const int DISTRIBUTE_TIMES = 7;
const int DELTA = 10;

template<class T>
class GraphReader
{
public:
    GraphReader(const string &);
    void ReadFromFile(const string &);               
    void SaveToMap();
    void ConvertToUndirectedGraph();
    void PartitionVersion1(int);                                                                // 划分版本1
    void PartitionVersion2(int);                                                                // 划分版本2
    void OptimizedVersion1();                                                                   // 优化版本1
    void OptimizedVersion2();                                                                   // 优化版本2
    void Initialize1(set<int>, int);                                                            // 初始化ConnectWeight -> OptimizedVersion1
    void Initialize2(set<int>, int n);                                                          // 初始化ConnectWeight -> OptimizedVersion2
    void NodeBuffInitialize(map<pair<int,int>, double> &, map<pair<int, int>, double>);         // 点对点 buff计算
    void BuffInitialize(map<pair<int, int>, double> &);                                         // 点对集合 buff计算
    void BuffUpdate(map<pair<int, int>, double> &);                                             // buff 更新 
    void ExchangeNode(vector<graph<T>> &, map<pair<int, int>, double> &, int, int, int, int);   // 交换两点
    void AddToTabuSet(set<pair<int, int>>&, map<pair<int, int>, int> &, int, int, int, int);
    void random_release_tabu_set(set<pair<int, int>> &, map<pair<int, int>, int> &);
    void Print();
    bool Judge(vector<graph<T>> &, vector<graph<T>> &, double &);                               // 判断是否更新最优解
    int GetNodeSize() const {return NodeSize;}
    double GetLoss(const vector<graph<T>> &) const;
    double GetTotalWeight() const {return TotalWeight;}
    map<int, vector<Edge<T>>> GetUndigraph_mp() const {return Undigraph_mp;}
    vector<graph<T>> GetGraph_vec() const {return graph_vec;}
   
    void Update();
    void SelectConnectedSubgraph(int n);
    void dfs(int, set<int> &);                                                  // 找出小于n的联通块 
    ~GraphReader(){}

private:
    int NodeSize;                                   // 节点数
    double SelfLoopWeight;                          // 自环权重
    double TotalWeight;                             // 总权重
    vector<int> NotIsolatedPoint_vec;               //方便随机取点
    vector<graph<T>> graph_vec;                     // 所有划分好的子图
    set<int> nodes;                                 // 结点编号
    set<int> NotIsolatedPoint;                      // 不孤立点
    set<int> IsolatedPoint;                         // 孤立点
    map<int, vector<Edge<T>>> Digraph_mp;           // 有向图 邻接表存图, 存某节点所有的出边
    map<pair<int, int>, T> edges;
    map<pair<int, int>, T> AllNodesConnect;
    map<pair<int, int>, T> Undiedges;
    map<int, bool> IsIsolated;
    map<pair<int, int >, vector<double>> Multiedge; // 记录重边
    map<int, map<int, double>> ConnectWeight;
    map<int, vector<Edge<T>>> Undigraph_mp;         // 存为无向图形式                                  
    map<int, double> NodeWeight;                    // 每个点的总权重
    map<int, int> SetOfNode;                        // 记录每个结点所在的子图编号
    map<int, int> SubSetSize;                       // 记录每张子图当前节点数

    vector<set<int>> ConnectedSubgraph;     //节点数小于n的联通子图
    map<int, bool> state; 
};

template<class T>
void GraphReader<T>::SelectConnectedSubgraph(int n)
{
    // 对每个没标记过的点做一遍dfs
    for (auto i : NotIsolatedPoint)    state[i] = false;
    for (auto i : NotIsolatedPoint)
    {
        set<int> subgraph;
        if (!state[i])  dfs(i, subgraph);
        if (subgraph.size() <= n && subgraph.size() != 0)
        {
            ConnectedSubgraph.push_back(subgraph);     //  这个小连通块可以被直接排除掉
        }
    }
    Update();
}

// 更新 NotIsolatedPoint NotIsolatedPointSize
template<class T>
void GraphReader<T>::Update()
{
    for (auto i : ConnectedSubgraph)
    {
        for (auto j : i)
        {
            if (NotIsolatedPoint.count(j))  NotIsolatedPoint.erase(j);
        }
    }
    for (auto i : NotIsolatedPoint)
    {
        NotIsolatedPoint_vec.push_back(i);
        for (auto j : NotIsolatedPoint)
        {
            if (Undiedges.count({i, j})) AllNodesConnect[{i,j}] = Undiedges[{i, j}];
            else AllNodesConnect[{i, j}] = 0;
        }
    }
}

template<class T>
void GraphReader<T>::dfs(int p, set<int> &subgraph)
{
    state[p] = true;
    for (auto i : Undigraph_mp[p])    
    {
        int pt = i.point;
        subgraph.insert(pt);
        if (!state[pt])
        {
            state[pt] = true;
            dfs(pt, subgraph);
        }
    }
    return ;
}

template<class T>
double GraphReader<T>::GetLoss(const vector<graph<T>> &g_vec) const
{
    double ans = 0;
    for (int i = 1; i < g_vec.size(); i ++) ans += g_vec[i].GetWeight();
    return TotalWeight - ans - SelfLoopWeight;
}

template<class T>
void GraphReader<T>::ConvertToUndirectedGraph()
{
    for (auto edge : edges)
    {
        Undigraph_mp[edge.first.first].push_back(Edge<T>(edge.first.second, edge.second));
        Undigraph_mp[edge.first.second].push_back(Edge<T>(edge.first.first, edge.second));
    }
}

template<class T>
GraphReader<T>::GraphReader(const string &filename) :TotalWeight(0)
{
    ReadFromFile(filename);
    SaveToMap();
    ConvertToUndirectedGraph();
}

template<class T>
void GraphReader<T>::ReadFromFile(const string& filename)
{
    string str;
    fstream fs;
    fs.open(filename, ios::in);
    if (!fs) {cout << "fail to open the file" << endl; exit(-1);}
    while (getline(fs, str))
    {
        int i = 0;
        string from_str = GetStr(str, i);
        int from = convert<string, int>(from_str);
        if (!IsIsolated.count(from)) IsIsolated[from] = true;
        if (str[i - 1] != '>')
        {
            string to_str = GetStr(str, i);
            string weight_str = GetStr(str, i);
            int to = convert<string, int>(to_str);
            T weight = convert<string, T>(weight_str);
            TotalWeight += weight;
            if(from == to) 
            {
                SelfLoopWeight += weight;
                nodes.insert(from);
                continue;
            }
            Multiedge[{from, to}].push_back(weight);
            edges[{from, to}] += weight;
            Undiedges[{from, to}] += weight;
            Undiedges[{to, from}] += weight;
            IsIsolated[from] = false;
            IsIsolated[to] = false;
            NodeWeight[from] += weight;
            NodeWeight[to] += weight;
            nodes.insert(to);
        }
        nodes.insert(from);
    }
    NodeSize = nodes.size();
    for (auto i : nodes)
    {
        if (!IsIsolated[i]) NotIsolatedPoint.insert(i);
        else IsolatedPoint.insert(i);
    }
}

template<class T>
void GraphReader<T>::SaveToMap()
{
    for(auto edge : edges)
    {
        Digraph_mp[edge.first.first].push_back(Edge<T>(edge.first.second, edge.second));
    }
}

template<class T>
void GraphReader<T>::Print()
{
    cout << "结点总数为: " << NodeSize << endl;
    for (auto i : nodes)
    {
        cout << i << " ";
    }
    cout << endl;
    cout << "非孤立结点有: " << endl;
    for (auto i : Digraph_mp)
    {
        cout << i.first << " ----> : " << endl;
        for (auto j : i.second)
        {
            cout << "to : " << j.point << "  weight:  " << j.weight << endl;
        }
        cout << endl;
    }
    cout << "孤立结点(包括自环)有: " << endl;
    for (auto i : IsIsolated)
    {
        if (i.second) cout << i.first << endl;
    }
    cout << "总权重为: " << TotalWeight << endl; 
}

template<class T>
void GraphReader<T>::Initialize1(set<int> V, int SubsetCnt)
{
    for (int i = 1; i < SubsetCnt; i ++)
    {
        for (auto j : V)
        {
            ConnectWeight[j][i] = 0;
        }
    }
    for (auto i : V)
    {
        for (auto j : Undigraph_mp[i])
        {
            int p = j.point;
            if (V.count(p))
            {
                ConnectWeight[i][SubsetCnt] += Undiedges[{i, p}];
            }
        }
    }
}

template<class T>
void GraphReader<T>::PartitionVersion1(int n)
{
    SelectConnectedSubgraph(n);
    random_device rd;
	mt19937 mt(rd());
    set<int> V1 = NotIsolatedPoint;
    int NotIsolateNodeSize = NotIsolatedPoint.size();
    bool flag = NotIsolateNodeSize % n;
    int SubsetCnt = NotIsolateNodeSize / n + flag;
    vector<set<int>> SubgraphNodes(SubsetCnt + 1);              // 集合编号从 1 -> SubSetCnt
    Initialize1(V1, SubsetCnt);                                 // 初始化 ConnectWeight map
    int TempNum = 1;
    int lastcnt = SubsetCnt;                                    // V1对应的子图编号
    while (V1.size() >= n)
    {
        set<int> AlternativeSet;
        while(SubgraphNodes[TempNum].size() < n)
        {
            priority_queue<pair<int, double>, vector<pair<int, double>>, cmp1> heap; // 用于记录备选点的增益值
            if (AlternativeSet.size() == 0)
            {
                int MAX = V1.size();
                int pos_ = mt() % (MAX - 1);
                auto iter = V1.begin();
                while (pos_ --) iter ++;
                int point_0 = *(V1.begin());
                SubgraphNodes[TempNum].insert(point_0);
                V1.erase(point_0);
                vector<Edge<T>> outpoint = Undigraph_mp[point_0]; // 和v0相连的所有点都是备选项
                for (auto i : outpoint)
                {
                    if (V1.count(i.point)) AlternativeSet.insert(i.point);
                    ConnectWeight[i.point][lastcnt] -= i.weight;
                    ConnectWeight[i.point][TempNum] += i.weight;
                }
            }
            if (SubgraphNodes[TempNum].size() >= n) continue;
            for (auto i : AlternativeSet)
            {
                double gain = ConnectWeight[i][TempNum] - ConnectWeight[i][lastcnt];
                heap.push({i, gain});
            }
            if (!heap.empty())
            {
                int p0 = heap.top().first;
                AlternativeSet.erase(p0);
                V1.erase(p0);
                SubgraphNodes[TempNum].insert(p0);
                for (auto i : Undigraph_mp[p0])
                {
                    if (V1.count(i.point)) AlternativeSet.insert(i.point);
                    ConnectWeight[i.point][lastcnt] -= i.weight;
                    ConnectWeight[i.point][TempNum] += i.weight;
                }
            }
        }
        TempNum ++;
    }
    if (flag)  SubgraphNodes[lastcnt] = V1;
    if (IsolatedPoint.size() != 0)
    {
        if (SubgraphNodes.size() == 0)  SubgraphNodes.push_back(set<int>());
        if (IsolatedPoint.size() <= n)  SubgraphNodes.push_back(IsolatedPoint);
        else
        {
            int cnt = IsolatedPoint.size() / n + (IsolatedPoint.size() % n) == 0 ? 0 : 1;
            auto iter = IsolatedPoint.begin();
            while (cnt --)
            {
                set<int> temp;
                for (int i = 0; i < n && iter != IsolatedPoint.end(); i ++)
                {
                    temp.insert(*iter);
                    iter ++;
                }
                SubgraphNodes.push_back(temp);
            }
        }
    }
    if (ConnectedSubgraph.size() > 0)
    {
        if (SubgraphNodes.size() == 0)  SubgraphNodes.push_back(set<int>());
        for (auto i : ConnectedSubgraph)    SubgraphNodes.push_back(i);
    }
    // 1-base
    graph_vec.push_back(graph<T>());
    for (int i = 1; i < SubgraphNodes.size(); i ++)
    {
        graph_vec.push_back(graph<T>(SubgraphNodes[i], Undigraph_mp, Digraph_mp, Multiedge));
    }
    for (int i = 1; i < SubgraphNodes.size(); i ++)
    {
        for (auto j : SubgraphNodes[i]) SetOfNode[j] = i;
    }
}

template<class T>
void GraphReader<T>::Initialize2(set<int> V, int n)
{
    // n张子图
    for (auto i : V)
    {
        for (int j = 1; j <= n; j ++)   ConnectWeight[i][j] = 0;
    }
    for (auto i : V)
    {
        for (auto j : V)
        {
            if (Undiedges.count({i, j})) ConnectWeight[i][SetOfNode[j]] += Undiedges[{i, j}];
        }
    }
}

template<class T>
void GraphReader<T>::PartitionVersion2(int n)
{
    SelectConnectedSubgraph(n);
    int cnt = 0;
    random_device rd;
	mt19937 mt(rd());
    // 给预先设定的集合每个都塞一个点， 剩下多出来的点按照最大收益分配
    int NotIsolatePointSize = NotIsolatedPoint.size();
    vector<set<int>> SubgraphNodes;
    if (NotIsolatePointSize) 
    {
        set<int> EqualDistrubeSet;
        set<int> RandDistrubeSet;
        int delta = mt() % DELTA;
        int SubsetCnt = (NotIsolatePointSize % n == 0) ? NotIsolatePointSize / n : NotIsolatePointSize / n + 1;
        if (SubsetCnt + delta <= NotIsolatePointSize) SubsetCnt += delta;       // 避免集合数量大于点的数量
        SubgraphNodes.resize(SubsetCnt + 1);                          // 编号 1-base, 从 1 -> SubSetCnt
        while (EqualDistrubeSet.size() != SubsetCnt)
        {
            int pos_ = rand() % NotIsolatedPoint_vec.size();
            EqualDistrubeSet.insert(NotIsolatedPoint_vec[pos_]);
        }
        for (auto i : NotIsolatedPoint_vec)
        {
            if (!EqualDistrubeSet.count(i)) RandDistrubeSet.insert(i);
        }
        int p = 1;
        for (auto i : EqualDistrubeSet)
        {
            SubgraphNodes[p ++].insert(i);
            cnt ++;
        }
        Initialize2(EqualDistrubeSet, SubsetCnt);                    // 初始化 ConnectWeight
        // RandDistrubeSet中的点按最大收益分配
        // 计算每个点和其他子集的connect
        for (auto i : RandDistrubeSet)
        {
            for (int j = 1; j <= n; j ++)   ConnectWeight[i][j] = 0;
        }
        for (auto i : RandDistrubeSet)
        {
            for (auto j : EqualDistrubeSet)
            {
                if (Undiedges.count({i, j})) ConnectWeight[i][SetOfNode[j]] += Undiedges[{i, j}];
            }
        }
        // 根据最大收益原则，分配RandDistrubeSet中的点
        while(RandDistrubeSet.size())
        {
            priority_queue<pair<pair<int, int>, double>, vector<pair<pair<int, int>, double>>, cmp2> heap;
            for (auto i : RandDistrubeSet)
            {
                for (int j = 1; j <= SubsetCnt; j ++)   heap.push({{i, j}, ConnectWeight[i][j]});
            }
            while(SubgraphNodes[heap.top().first.second].size() >= n) heap.pop();
            auto temp = heap.top();
            SubgraphNodes[temp.first.second].insert(temp.first.first);
            for (auto i : Undigraph_mp[temp.first.first])
            {
                int p = i.point;
                ConnectWeight[p][temp.first.second] += Undiedges[{temp.first.first, p}];
            }
            RandDistrubeSet.erase(temp.first.first);
        }
    } 

    // 分配孤立点
    if (IsolatedPoint.size() != 0)
    {
        if (SubgraphNodes.size() == 0)  SubgraphNodes.push_back(set<int>());
        if (IsolatedPoint.size() <= n)  SubgraphNodes.push_back(IsolatedPoint);
        else
        {
            int cnt = IsolatedPoint.size() / n + ((IsolatedPoint.size() % n == 0) ? 0 : 1);
            auto iter = IsolatedPoint.begin();
            while (cnt --)
            {
                set<int> temp;
                for (int i = 0; i < n && iter != IsolatedPoint.end(); i ++)
                {
                    cout << *iter << endl;
                    temp.insert(*iter);
                    iter ++;
                }
                SubgraphNodes.push_back(temp);
            }
        }
    }
    if (ConnectedSubgraph.size() > 0)
    {
        if (SubgraphNodes.size() == 0)  SubgraphNodes.push_back(set<int>());
        for (auto i : ConnectedSubgraph)    SubgraphNodes.push_back(i);
    }
    // 1-base
    graph_vec.push_back(graph<T>());
    for (int i = 1; i < SubgraphNodes.size(); i ++)
    {
        graph_vec.push_back(graph<T>(SubgraphNodes[i], Undigraph_mp, Digraph_mp, Multiedge));
    }

    for (int i = 1; i < SubgraphNodes.size(); i ++)
    {
        for (auto j : SubgraphNodes[i]) SetOfNode[j] = i;
    }
}

template<class T>
void GraphReader<T>::BuffUpdate(map<pair<int, int>, double> &buff)
{
    for (auto i : NotIsolatedPoint)
    {
        for (int j = 1; j <= graph_vec.size() - 1; j ++)
        {
            if (j == SetOfNode[i]) buff[{i,j}] = 0;
            else buff[{i,j}] = ConnectWeight[i][j] - ConnectWeight[i][SetOfNode[i]];
        }
    }
}

template<class T>
void GraphReader<T>::ExchangeNode(vector<graph<T>> &current_version, map<pair<int, int>, double> &buff, int v0, int target_node, int v0_set, int target_set)
{
    if (v0 == target_node) return;
    current_version[target_set].EraseNode(target_node);
    current_version[v0_set].EraseNode(v0);
    current_version[v0_set].InsertNode(target_node);
    current_version[target_set].InsertNode(v0);
    SetOfNode[v0] = target_set;
    SetOfNode[target_node] = v0_set;
    // 两个点交换之后，对buff的改变不止 v0_set 和 target_set. 对其它集合也会有影响, 所以直接都更新一遍
    for (auto i : Undigraph_mp[v0])
    {
        int j = i.point;
        ConnectWeight[j][v0_set] -= i.weight;
        ConnectWeight[j][target_set] += i.weight;
    }
    for (auto i : Undigraph_mp[target_node])
    {
        int j = i.point;
        ConnectWeight[j][v0_set] += i.weight;
        ConnectWeight[j][target_set] -= i.weight;
    }
    BuffUpdate(buff);
}

template<class T>
void GraphReader<T>::AddToTabuSet(set<pair<int, int>>& Tabuset, map<pair<int, int>, int> &Tabumap, int v0, int v0_set, int target_node, int target_set)
{
    Tabuset.insert({v0, v0_set});
    Tabuset.insert({target_node, target_set});
    Tabumap[{v0, v0_set}] = TABU_SETPSIZE;
    Tabumap[{target_node, target_set}] = TABU_SETPSIZE;
}

template<class T>
void GraphReader<T>::BuffInitialize(map<pair<int, int>, double> &buff)
{
    for (auto i : NotIsolatedPoint)
    {
        int temp_set = SetOfNode[i];
        for (int j = 1; j <= graph_vec.size() - 1; j ++)
        {
            if (temp_set == j) buff[{i, j}] = 0;
            else 
            {
                buff[{i, j}] = ConnectWeight[i][j] - ConnectWeight[i][temp_set];
            }
        }
    }
}

template<class T>
void GraphReader<T>::NodeBuffInitialize(map<pair<int, int>, double> &node_change_buff, map<pair<int, int>, double> buff)
{
    // 一个点一个点的处理， 处理它的所有出边
    for (auto i : NotIsolatedPoint)
    {
        for (auto j : NotIsolatedPoint)
        {
            if (SetOfNode[j] == SetOfNode[i])
            {
                node_change_buff[{i, j}] = 0;
            }
            else 
            {
                node_change_buff[{i, j}] = buff[{i, SetOfNode[j]}] + buff[{j, SetOfNode[i]}] - 2 * AllNodesConnect[{i, j}];
            }
        }
    }
}

template<class T>
void GraphReader<T>::random_release_tabu_set(set<pair<int, int>> &Tabuset, map<pair<int, int>, int> &Tabumap)
{
    // 随机释放一些点
    int times = rand() % ((Tabuset.size() / 3) + 3);
    while (times --)
    {
        int steps = rand() % Tabuset.size() + 1;
        auto temp = Tabuset.begin();
        while (steps --) temp ++;
        if (temp != Tabuset.end())
        {
            Tabumap.erase(*temp);
            Tabuset.erase(temp);
        }
    }
}

template<class T>
void GraphReader<T>::OptimizedVersion1()
{
    random_device rd;
	mt19937 mt(rd());
    int iteration_time = 0;
    int unupdated_time = 0;
    map<pair<int, int>, double> buff;                            // 记录把 node A -> set B 时的增益 
    set<pair<int, int>> Tabuset;                                 // 记录被禁结点
    map<pair<int, int>, int> Tabumap;                            // 记录禁忌次数
    map<int, int> TabuNode;                                      // 对移动频率较高的node作出惩罚
    map<int, int> MoveFrequence;                                 // 用于惩罚那些移动频繁的点
    vector<graph<T>> best_version = graph_vec;
    vector<graph<T>> current_version = graph_vec;
    double least_loss = GetLoss(best_version);
    int Timer = 0;                                               // 用于统计多久没有更新过答案了
    BuffInitialize(buff);
    for (auto i : NotIsolatedPoint_vec)     TabuNode[i] = 0;
    // while (iteration_time <= ITERATION_TIMES)
    while(true)
    {
        priority_queue<pair<pair<int, int>, double>, vector<pair<pair<int, int>, double>>, cmp2> heap;
        for (map<pair<int, int>, double>::iterator iter = buff.begin(); iter != buff.end(); iter ++)
        {
            if ((*iter).first.second != 0)  heap.push((*iter));
        }
        while (!heap.empty() && (Tabuset.count(heap.top().first) || TabuNode[heap.top().first.first] > 0))   heap.pop();
        if (heap.empty()) 
        {
            iteration_time ++;
            random_release_tabu_set(Tabuset, Tabumap);
            continue;
        }
        auto temp = heap.top(); heap.pop();          // 把 v1 -> 集合P 时增益最大
        int v0 = temp.first.first;
        int target_set = temp.first.second;
        int v0_set = SetOfNode[v0];
        priority_queue<pair<int, double>, vector<pair<int, double>>, cmp1> heap2;
        // heap2 直接存 v0 和 target_set 中的点交换之后的收益
        for (auto i : current_version[target_set].nodes)    
        {
            int exchange_buff = buff[{i, v0_set}] + temp.second - 2 * AllNodesConnect[{i, v0}];
            heap2.push({i, exchange_buff});
        }
        while (!heap2.empty() && (Tabuset.count({heap2.top().first, v0_set}) || TabuNode[heap2.top().first] > 0))  
        {
            heap2.pop();
        }
        if (heap2.empty())
        {
            iteration_time ++;
            random_release_tabu_set(Tabuset, Tabumap);
            continue;
        }
        int target_node = heap2.top().first;
        if (heap2.top().second > 0)
        {
            Timer --;
            ExchangeNode(current_version, buff, v0, target_node, v0_set, target_set);
            double curr_loss = GetLoss(current_version);
            cout << "Current Loss : " << curr_loss << " || Least Loss : " << least_loss  << " || TotalWeight : " << TotalWeight <<  endl;
            Judge(current_version, best_version, least_loss);
        }
        Timer ++;
        
        // 更新 Tabumap 与 Tabuset 
        for (set<pair<int, int>>::iterator iter = Tabuset.begin(); iter != Tabuset.end();)
        {
            Tabumap[*iter] --;
            if (Tabumap[*iter] <= 0)
            {
                pair<int, int> temp = *iter;
                Tabuset.erase(iter ++);
                Tabumap.erase(temp);
            }
            else iter ++;
        }
        for (auto i : TabuNode)
        {
            if (TabuNode[i.first] > 0) TabuNode[i.first] --; 
        }

        for (map<int, int>::iterator iter = MoveFrequence.begin(); iter != MoveFrequence.end(); iter++)
        {
            if ((*iter).second >= MOVE_FREQUENCE)
            {
                TabuNode[(*iter).first] = TABU_SETPSIZE;
                iter = MoveFrequence.erase(iter);
            }
        }
        MoveFrequence[v0] ++;
        MoveFrequence[target_node] ++;
        AddToTabuSet(Tabuset, Tabumap, v0, v0_set, target_node, target_set);
        // 长时间不更新的话, 随机给与扰动
        if (Timer >= UNUPDATED_TIMES)
        {
            int p1, p2;
            int MAX = NotIsolatedPoint_vec.size();
            int n = DISTRIBUTE_TIMES;
            set<int> temp_tabu_set;
            for (int i = 1; i <= n; i ++)
            {
                int t = 0;
                p1 = mt() % MAX;
                do
                {
                    t ++;
                    p2 = mt() % MAX;
                    p1 = NotIsolatedPoint_vec[p1];
                    p2 = NotIsolatedPoint_vec[p2];
                }
                while(t <= 20 && SetOfNode[p1] == SetOfNode[p2] || temp_tabu_set.count(p1) || temp_tabu_set.count(p2));
                ExchangeNode(current_version, buff, p1, p2, SetOfNode[p1], SetOfNode[p2]);
                temp_tabu_set.insert(p1);
                temp_tabu_set.insert(p2);
                AddToTabuSet(Tabuset, Tabumap, p1, SetOfNode[p1], p2, SetOfNode[p2]);
            }
            Judge(current_version, best_version, least_loss);
            Timer = 0;
        }
        iteration_time ++;
    }
    graph_vec = best_version;
}

template<class T>
void GraphReader<T>::OptimizedVersion2()
{
    if (NotIsolatedPoint.size() == 0) return;
    random_device rd;
	mt19937 mt(rd());
    int iteration_time = 0;
    int unupdated_time = 0;
    map<pair<int, int>, double> buff;                            // 记录把 node A -> set B 时的增益 
    map<pair<int, int>, double> node_change_buff;                // 记录把 nodeA <-> node B 交换的收益
    vector<graph<T>> best_version = graph_vec;
    vector<graph<T>> current_version = graph_vec;
    double least_loss = GetLoss(best_version);
    int UpdateSteps = 0;
    int UnUpdatedTimes = 0;
    int cnt = 0;
    bool flag;
    BuffInitialize(buff);
    NodeBuffInitialize(node_change_buff, buff);
    while(true)
    {
        flag = false;
        priority_queue<pair<pair<int, int>, double>, vector<pair<pair<int, int>, double>>, cmp2> heap;
        for (auto i : node_change_buff)  heap.push(i);
        auto temp = heap.top();                                 // 把 v1 -> 集合P 时增益最大
        int v0 = temp.first.first;
        int target_node = temp.first.second;
        int target_set = SetOfNode[target_node];
        int v0_set = SetOfNode[v0];
        if (temp.second > 0)
        {
            flag = true;
            double curr_loss = GetLoss(current_version);
            ExchangeNode(current_version, buff, v0, target_node, v0_set, target_set);
            NodeBuffInitialize(node_change_buff, buff);
            curr_loss = GetLoss(current_version);
            cout << "Current Loss : " << curr_loss << " || Least Loss : " << least_loss  << " || TotalWeight : " << TotalWeight <<  endl;
            UpdateSteps = least_loss - curr_loss;
            if(Judge(current_version, best_version, least_loss))
            {
                UnUpdatedTimes = 0;
                if (UpdateSteps < 5) cnt ++;
                else cnt = 0;
                if (cnt >= 30) break;
            }
            else
            {
                UnUpdatedTimes ++;
                if (UnUpdatedTimes >= 10000) break;
            }
        }
        //不更新的话, 随机给与扰动
        if (!flag)
        {
            int p1, p2;
            int MAX = NotIsolatedPoint_vec.size();
            int n = DISTRIBUTE_TIMES;
            set<int> temp_tabu_set;
            for (int i = 1; i <= n; i ++)
            {
                int t = 0;
                p1 = mt() % MAX;
                do
                {
                    t ++;
                    p2 = mt() % MAX;
                    p1 = NotIsolatedPoint_vec[p1];
                    p2 = NotIsolatedPoint_vec[p2];
                }
                while(t <= 20 && SetOfNode[p1] == SetOfNode[p2] && (temp_tabu_set.count(p1) || temp_tabu_set.count(p2)));
                ExchangeNode(current_version, buff, p1, p2, SetOfNode[p1], SetOfNode[p2]);    
                NodeBuffInitialize(node_change_buff, buff);
                temp_tabu_set.insert(p1);
                temp_tabu_set.insert(p2);
            }
            Judge(current_version, best_version, least_loss);
            cnt = 0;
        }
        iteration_time ++;
    }
    graph_vec = best_version;
}

template<class T>
bool GraphReader<T>::Judge(vector<graph<T>> &current_version, vector<graph<T>> &best_version, double &least_loss)
{
    double curr_loss = GetLoss(current_version);
    if (curr_loss <= least_loss) 
    {
        least_loss = curr_loss;
        best_version = current_version;
        // 清空子图文件 并把最新的版本保存到其中中
        if (system(string("./clean").c_str()) == -1)
        {
            cerr << "error : Failed to call external program! ";
            exit(-1);
        }
        int num = 0;
        for (auto i : best_version) 
        {
            if (num ++ == 0) continue;
            i.SaveAsFile2(SUBGRAPH_FILENAME);
        }
        int j = 0;
        vector<string> filename_str;
        for (int i = 0; i < graph_vec.size(); i ++)   filename_str.push_back("SubGraph/" + to_string(i) + ".txt");
        for (auto i : graph_vec)   i.SaveAsFile(filename_str[j ++]);
        remove("./SubGraph/0.txt");
        return true;
    }
    return false;
}

#endif