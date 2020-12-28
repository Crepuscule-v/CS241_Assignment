#include "HeaderFile.h"
using namespace std;

int main()
{
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
            filename_vec.push_back(entry->d_name);
        }
    }
    for (auto i : filename_vec)
    {
        string temp = "./SubGraph/" + i;
        remove(temp.c_str());
    }
    closedir(d);
    remove("SubGraph.txt");
    return 0;
}