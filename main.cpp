#include "GraphReader.h"

const string ORIGIN_FILENAME = "大数据/10-500.txt";
const string CLEAN = "./clean";

int main()
{
    srand(time(nullptr));
    int n, j = 0;
    cin >> n;
    vector<string> filename_str;
    struct timeval t1, t2;
    double runtime;
    gettimeofday(&t1, nullptr);

    GraphReader<double> graph(ORIGIN_FILENAME);
    GraphReader<double> graph2(ORIGIN_FILENAME);
    GraphReader<double> graph3(ORIGIN_FILENAME);
    graph.PartitionVersion1(n);
    graph.OptimizedVersion1();

    // graph.PartitionVersion2(n);
    // graph.OptimizedVersion2();

    // graph3.PartitionVersion2(n);
    // graph3.OptimizedVersion2();

    // cout << graph2.GetLoss(graph2.GetGraph_vec()) << endl;
    // cout << graph3.GetLoss(graph3.GetGraph_vec()) << endl;
    // GraphReader<double> graph = (graph2.GetLoss(graph2.GetGraph_vec()) > graph3.GetLoss(graph3.GetGraph_vec())) ? graph3 : graph2;
    // cout << graph.GetLoss(graph.GetGraph_vec()) << endl;
    /*
     * Clean ".txt" file;
     */
    if (system(CLEAN.c_str()) == -1)
    {
        cerr << "error : Failed to call external program! ";
        exit(-1);
    }
    /*
     * Save subgraphs 
     */
    for (int i = 0; i < graph.GetGraph_vec().size(); i ++)   filename_str.push_back("SubGraph/" + to_string(i) + ".txt");
    for (auto i : graph.GetGraph_vec())   i.SaveAsFile(filename_str[j ++]);
    remove("./SubGraph/0.txt");
    /*
     * Check the correctness of partition
    */
    remove(SUBGRAPH_FILENAME);
    int num = 0;
    for (auto i : graph.GetGraph_vec()) 
    {
        if (num ++ == 0) continue;
        i.SaveAsFile2(SUBGRAPH_FILENAME);
    }
    try {
        cout << "\nChecking ..." << endl;
        Check::Checker<double> Checker(ORIGIN_FILENAME);
        Checker.check(SUBGRAPH_FILENAME);
        cout << "Congratulations! Graph is properly divided! \n" << endl;
    }
    catch(Check::Error error) {
        error.print_error();
        exit(-1);
    }

    gettimeofday(&t2, nullptr);
    runtime = (t2.tv_sec - t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec) / 1000000.0;
    cout << "The total running time is : " << runtime << "s" << endl;
    return 0;
}