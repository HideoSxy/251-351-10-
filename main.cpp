#include <iostream>
#include <vector>

bool dfs(const std::vector<std::vector<int>>& adj, int v, int end,
         std::vector<bool>& visited) {
    if (v == end) return true;
    visited[v] = true;

    for (int u = 0; u < (int)adj.size(); ++u) {
        if (adj[v][u] && !visited[u]) {
            if (dfs(adj, u, end, visited)) return true;
        }
    }
    return false;
}

int main() {
    int n;
    std::cout << "Введите число вершин: ";
    std::cin >> n;

    std::vector<std::vector<int>> adj(n, std::vector<int>(n));
    std::cout << "Введите матрицу смежности (" << n << "x" << n << "):\n";
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            std::cin >> adj[i][j];

    int start, end;
    std::cout << "Начальная вершина: "; std::cin >> start;
    std::cout << "Конечная вершина:  "; std::cin >> end;

    std::vector<bool> visited(n, false);
    bool foundDFS = dfs(adj, start, end, visited);
    std::cout << "DFS: путь " << start << " → " << end
              << (foundDFS ? " существует" : " не существует") << "\n";

    return 0;
}