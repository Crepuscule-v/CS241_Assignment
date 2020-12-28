#ifndef __GREPH_H__
#define __GREPH_H__
#include "HeaderFile.h"

using namespace std;

template<class T>
struct Edge
{
    int point;
    T weight;
    Edge(int point = 0, T w = -1) : point(point), weight(w){}
};

template<class T>
class graph
{
    int NodeSize;
    double TotalWeight;
    map<int, vector<Edge<T>>> Undiedges;            // 无向边
    map<int, vector<Edge<T>>> edges;                // 有向边
    map<int, vector<Edge<T>>> Undigraph_mp;         // 原无向图
    map<int, vector<Edge<T>>> Digraph_mp;           // 原有向图
    map<pair<int, int>, vector<double>> Multiedge;  // 重边
    map<int, bool> IsIsolated;

public:
    set<int> nodes;
    graph(){}
    graph(const set<int>&, const map<int, vector<Edge<T>>> &, map<int, vector<Edge<T>>>, const map<pair<int, int>, vector<double>> &);
    void print() const;
    void SaveAsFile(const string &);
    int GetNodeSize() const {return NodeSize;}
    double GetWeight() const {return TotalWeight;} 
    void InsertNode(int);
    void EraseNode(int);
    void SaveAsFile2(const string &) const;
};

template<class T>
bool find_(vector<Edge<T>> vec, Edge<T> edge)
{
    for (auto i : vec)
    {
        if (i.point == edge.point && i.weight == edge.weight) return true;
    }
    return false;
}

template<class T>
void graph<T>::InsertNode(int p)
{
    // 注意需要建立双向边
    if (nodes.count(p)) return ;
    IsIsolated[p] = true;
    nodes.insert(p);
    vector<Edge<T>> newedges;
    vector<Edge<T>> Diedges;
    for (auto edge : Undigraph_mp[p])
    {
        int j = edge.point;
        if (nodes.count(j))
        {
            IsIsolated[j] = false;
            newedges.push_back(Edge<T>(j, edge.weight));
            TotalWeight += edge.weight;
            Undiedges[j].push_back(Edge<T>(p, edge.weight));
            if (find_(Digraph_mp[j], Edge<T>(p, edge.weight)))
            {
                edges[j].push_back(Edge<T>(p, edge.weight));
            }
            if (find_(Digraph_mp[p], Edge<T>(j, edge.weight)))
            {
                Diedges.push_back(Edge<T>(j, edge.weight));
            }
        }
    }
    edges[p] = Diedges;
    Undiedges[p] = newedges;
    NodeSize ++;
}

template<class T>
void graph<T>::EraseNode(int p)
{
    if (!nodes.count(p)) return;
    for (auto i : Undiedges[p])
    {
        int j = i.point;
        TotalWeight -= i.weight;
        for (int k = 0; k < Undiedges[j].size(); k ++)
        {
            if (Undiedges[j][k].point == p)
            {
                Undiedges[j].erase(Undiedges[j].begin() + k);
            }
        }
    }
    Undiedges.erase(p);
    edges.erase(p);
    nodes.erase(p);
    NodeSize --;
}

template<class T>
graph<T>::graph(const set<int> &node_0, const map<int, vector<Edge<T>>> &U_mp, map<int, vector<Edge<T>>> mp, const map<pair<int, int>, vector<double>> &mulmap): nodes(node_0), TotalWeight(0)
{
    Multiedge = mulmap;
    Digraph_mp = mp;
    Undigraph_mp = U_mp;
    NodeSize = node_0.size();
    for (auto node : nodes)
    {
        IsIsolated[node] = true;
        if (!U_mp.count(node)) continue;        // 孤立点
        else
        {
            for (auto edge : mp[node])
            {
                if (nodes.count(edge.point))
                {
                    Undiedges[node].push_back(Edge<T>(edge.point, edge.weight));
                    Undiedges[edge.point].push_back(Edge<T>(node, edge.weight));
                    edges[node].push_back(Edge<T>(edge.point, edge.weight));
                    TotalWeight += edge.weight;
                }
            }
        }
    }
    for (auto i : Undiedges) IsIsolated[i.first] = false;
}

template<class T>
void graph<T>::print() const
{
    cout << "结点总数为: " << NodeSize << endl;
    cout << "非孤立结点有: " << endl;
    for (auto i : Undiedges)
    {
        cout << i.first << " ---->" << endl;
        for (auto j : i.second)
        {
            cout << "to : " << j.point << "  weight:  " << j.weight << endl;
        }
    }
    cout << "孤立结点(包括自环)有: " << endl;
    for (auto i : IsIsolated)
    {
        if (i.second) cout << i.first << endl;
    }
    cout << "总权值为: " << TotalWeight << endl;
}

template<class T>
void graph<T>::SaveAsFile(const string &filename)
{
    fstream fs;
    fs.open(filename, ios::out);
    if (!fs) {cout << "fail to open the file\n"; exit(-1);}
    for (auto i : nodes)
    {
        if (Digraph_mp[i].size() == 0) 
        {
            fs << "<" << i << ">" << endl;
        }
        for (auto j : Digraph_mp[i])
        {
            int p = j.point;
            if (nodes.count(p))
            {
                for (auto t : Multiedge[{i, p}])
                {
                    fs << "<" << i << " " << p << " " << t << ">" << endl;
                }
            }
            else 
            {
                for (auto t : Multiedge[{i, p}])
                {
                    fs << "<" << i << " * " << p << " " << t << ">" << endl;
                }
            }
        }
    }
    fs.close();
}

template<class T>
void graph<T>::SaveAsFile2(const string &filename) const
{
    fstream fs;
    fs.open(filename, ios::app);
    if (!fs) {cout << "fail to open the file\n"; exit(-1);}
    for (auto i : nodes)
    {
        fs << i << " ";
    }
    fs << endl;
    fs.close();
}

#endif