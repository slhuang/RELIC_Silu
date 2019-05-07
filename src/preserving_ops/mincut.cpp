#include "../../include/preserving_ops/mincut.h"

void printRgraph(vector<unordered_map<int, int>> &rGraph)
{
    ofstream fout("/home/slhuang/Public/2018-fall/lineage_inference/case_study/retail/result/rGraph.txt", ios::app);
    fout << "************** " << rGraph.size() << " ***************" << endl;
    for (int i = 0; i < rGraph.size(); ++i)
    {
        fout << i << " " << rGraph[i].size() << " ";
        for (unordered_map<int, int>::iterator it = rGraph[i].begin(); it != rGraph[i].end(); ++it)
        {
            fout << " " << it->first << " " << it->second;
        }
        fout << endl;
    }
    cout << "Finish printing rGraph" << endl;
}

/* Returns true if there is a path from source 's' to sink 't' in
  residual graph. Also fills parent[] to store the path */
bool bfs(vector<unordered_map<int, int>> &rGraph, int s, int t, vector<int> &parent)
{
    vector<bool> visited(rGraph.size(), false);
    queue<int> q;
    q.push(s);
    visited[s] = true;
    parent[s] = -1;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        unordered_map<int, int> neig = rGraph[u];
        for (unordered_map<int, int>::iterator it = neig.begin(); it != neig.end(); ++it)
        {
            int v = it->first, w = it->second;
            if (w > 0 && !visited[v])
            {
                q.push(v);
                visited[v] = true;
                parent[v] = u;
                if (v == t)
                    return true;
            }
        }
    }
    cout << "------finish bfs------" << endl;
    // printRgraph(rGraph);
    return false; // there is no s-t path in the residual graph
}

// A DFS based function to find all reachable vertices from s.  The function
// marks visited[i] as true if i is reachable from s.  The initial values in
// visited[] must be false. We can also use BFS to find reachable vertices
void dfs(vector<unordered_map<int, int>> &rGraph, int s, vector<bool> &visited, int &reached)
{
    visited[s] = true;
    reached++;
    unordered_map<int, int> neig = rGraph[s];
    for (unordered_map<int, int>::iterator it = neig.begin(); it != neig.end(); ++it)
    {
        int v = it->first, w = it->second;
        if ((w > 0) && (!visited[v]))
            dfs(rGraph, v, visited, reached);
    }
}

vector<bool> minCut(vector<unordered_map<int, int>> rGraph, int s, int t)
{
    clock_t tt = clock();
    vector<int> parent(rGraph.size(), -1); // used in BFS and to store path
    int maxFlow = 0, iter = 0;
    // Augment the flow while there is path from source to sink
    while (bfs(rGraph, s, t, parent))
    {
        //cout << "*************************comining in mincut****************" << endl;
        clock_t tsub = clock();
        // compute the bottleneck capacity along the path filled by BFS
        int augmentFlow = INT_MAX;
        for (int v = t; v != s; v = parent[v])
        {
            int u = parent[v];
            augmentFlow = min(augmentFlow, rGraph[u][v]);
        }
        if (iter % 1000 == 0)
            cout << "augmentFlow in this iteration: " << augmentFlow << endl;
        //update the residual capacities of the edges and reverse edge along the path in the residual graph
        for (int v = t; v != s; v = parent[v])
        {
            int u = parent[v];
            rGraph[u][v] -= augmentFlow;
            rGraph[v][u] += augmentFlow;
            if (rGraph[u][v] < 0 || rGraph[v][u] < 0)
            {
                cout << "=============== Error " << rGraph[u][v] << ", " << rGraph[v][u] << ", " << augmentFlow << endl;
                exit(1);
            }
        }
        maxFlow += augmentFlow;
        iter++;
        if (iter % 1000 == 0)
        {
            cout << "Min cut, in " << iter << " iteration: current flow is " << maxFlow << "." << endl;
            cout << "Taking " << ((double)(clock() - tsub) * 1000 / CLOCKS_PER_SEC) << " ms in this iteration." << endl;
        }
    }
    cout << "Finish finding max-flow: " << maxFlow << " after " << iter << "iterations." << endl;
    // now we have the max flow, find the vertices reachable from s in the residual graph
    vector<bool> visited(rGraph.size(), false);
    int reached = 0;
    dfs(rGraph, s, visited, reached);
    cout << "-------------Finish minCut. Total number of nodeds Reached from s: " << reached << ". Taking " << ((double)(clock() - tt) * 1000 / CLOCKS_PER_SEC) << " ms. -----------" << endl;
    return visited; // return set S, where visited[] is true
}

