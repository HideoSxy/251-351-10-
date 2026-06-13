#include "genetic_path.h"

#include <QStringList>
#include <QVector>
#include <QQueue>

#include <algorithm>
#include <limits>
#include <random>
#include <vector>

namespace {

constexpr int kPopSize = 20;
constexpr int kGenerations = 50;
constexpr double kMutationRate = 0.3;
constexpr double kInf = std::numeric_limits<double>::infinity();

struct Graph {
    int n = 0;
    std::vector<std::vector<std::pair<int, int>>> adj;
};

void ensureSize(Graph &g, int v)
{
    if (v >= g.n) {
        g.adj.resize(v + 1);
        g.n = v + 1;
    }
}

bool parseGraph(const QString &payload, int &start, int &end, Graph &g, QString &err)
{
    const QStringList parts = payload.split(',', Qt::KeepEmptyParts);
    if (parts.size() < 3) {
        err = "Expected format: start,end,from:to:weight,...";
        return false;
    }

    bool okStart = false;
    bool okEnd = false;
    start = parts[0].trimmed().toInt(&okStart);
    end = parts[1].trimmed().toInt(&okEnd);
    if (!okStart || !okEnd || start < 0 || end < 0) {
        err = "Invalid start or end vertex";
        return false;
    }

    for (int i = 2; i < parts.size(); ++i) {
        const QStringList edge = parts[i].trimmed().split(':');
        if (edge.size() != 3) {
            err = "Invalid edge format, use from:to:weight";
            return false;
        }

        bool okFrom = false;
        bool okTo = false;
        bool okWeight = false;
        const int from = edge[0].trimmed().toInt(&okFrom);
        const int to = edge[1].trimmed().toInt(&okTo);
        const int weight = edge[2].trimmed().toInt(&okWeight);

        if (!okFrom || !okTo || !okWeight || from < 0 || to < 0 || weight < 0) {
            err = "Invalid edge values";
            return false;
        }

        ensureSize(g, std::max(from, to));
        g.adj[from].push_back({to, weight});
    }

    if (g.n == 0) {
        err = "Graph is empty";
        return false;
    }

    ensureSize(g, std::max(start, end));
    return true;
}

int edgeWeight(const Graph &g, int from, int to)
{
    for (const auto &e : g.adj[from]) {
        if (e.first == to)
            return e.second;
    }
    return -1;
}

bool isValidPath(const Graph &g, const QVector<int> &path, int start, int end)
{
    if (path.isEmpty() || path.front() != start || path.back() != end)
        return false;

    for (int i = 0; i + 1 < path.size(); ++i) {
        if (edgeWeight(g, path[i], path[i + 1]) < 0)
            return false;
    }
    return true;
}

double pathLength(const Graph &g, const QVector<int> &path)
{
    double len = 0.0;
    for (int i = 0; i + 1 < path.size(); ++i) {
        const int w = edgeWeight(g, path[i], path[i + 1]);
        if (w < 0)
            return kInf;
        len += w;
    }
    return len;
}

QVector<int> bfsPath(const Graph &g, int start, int end)
{
    QVector<int> parent(g.n, -1);
    QVector<bool> seen(g.n, false);
    QQueue<int> q;
    q.enqueue(start);
    seen[start] = true;

    while (!q.isEmpty()) {
        const int v = q.dequeue();
        if (v == end)
            break;

        for (const auto &e : g.adj[v]) {
            if (!seen[e.first]) {
                seen[e.first] = true;
                parent[e.first] = v;
                q.enqueue(e.first);
            }
        }
    }

    if (!seen[end])
        return {};

    QVector<int> path;
    for (int v = end; v != -1; v = parent[v])
        path.prepend(v);
    return path;
}

QVector<int> randomPath(const Graph &g, int start, int end, std::mt19937 &rng)
{
    QVector<int> path;
    path.append(start);

    QVector<bool> visited(g.n, false);
    visited[start] = true;

    int current = start;
    const int maxSteps = g.n * g.n;

    for (int step = 0; step < maxSteps && current != end; ++step) {
        std::vector<int> candidates;
        for (const auto &e : g.adj[current]) {
            if (!visited[e.first])
                candidates.push_back(e.first);
        }

        if (candidates.empty())
            return {};

        std::uniform_int_distribution<int> dist(0, static_cast<int>(candidates.size()) - 1);
        const int next = candidates[dist(rng)];
        path.append(next);
        visited[next] = true;
        current = next;
    }

    if (current != end)
        return {};

    return path;
}

QVector<int> crossover(const QVector<int> &p1, const QVector<int> &p2, const Graph &g, int start, int end)
{
    for (int i = 1; i < p1.size() - 1; ++i) {
        for (int j = 1; j < p2.size() - 1; ++j) {
            if (p1[i] != p2[j])
                continue;

            QVector<int> child;
            child.reserve(p1.size() + p2.size());
            for (int k = 0; k <= i; ++k)
                child.append(p1[k]);
            for (int k = j + 1; k < p2.size(); ++k)
                child.append(p2[k]);

            if (isValidPath(g, child, start, end))
                return child;
        }
    }
    return p1;
}

void mutate(QVector<int> &path, const Graph &g, std::mt19937 &rng)
{
    if (path.size() <= 2)
        return;

    std::uniform_int_distribution<int> idxDist(1, path.size() - 2);
    const int i = idxDist(rng);

    QVector<int> shorter;
    shorter.reserve(path.size() - 1);
    for (int k = 0; k < path.size(); ++k) {
        if (k == i)
            continue;
        shorter.append(path[k]);
    }

    if (isValidPath(g, shorter, path.front(), path.back()))
        path = shorter;
}

QString pathToString(const QVector<int> &path)
{
    QStringList items;
    for (int v : path)
        items.append(QString::number(v));
    return items.join(',');
}

} // namespace

QString func_genetic_path(const QString &payload)
{
    int start = 0;
    int end = 0;
    Graph g;
    QString err;

    if (!parseGraph(payload, start, end, g, err))
        return "ERR: " + err;

    const QVector<int> bfs = bfsPath(g, start, end);
    if (bfs.isEmpty())
        return "ERR: No path between start and end";

    std::mt19937 rng(42);
    std::vector<QVector<int>> population;
    population.push_back(bfs);

    for (int attempt = 0; attempt < 500 && static_cast<int>(population.size()) < kPopSize; ++attempt) {
        const QVector<int> candidate = randomPath(g, start, end, rng);
        if (candidate.isEmpty())
            continue;

        bool exists = false;
        for (const auto &p : population) {
            if (p == candidate) {
                exists = true;
                break;
            }
        }
        if (!exists)
            population.push_back(candidate);
    }

    while (static_cast<int>(population.size()) < kPopSize)
        population.push_back(bfs);

    int generationsDone = 0;
    for (int gen = 0; gen < kGenerations; ++gen) {
        std::sort(population.begin(), population.end(),
                  [&](const QVector<int> &a, const QVector<int> &b) {
                      return pathLength(g, a) < pathLength(g, b);
                  });

        std::vector<QVector<int>> nextGen;
        nextGen.push_back(population.front());

        std::uniform_real_distribution<double> chance(0.0, 1.0);
        std::uniform_int_distribution<int> parentDist(0, static_cast<int>(population.size() / 2));

        while (static_cast<int>(nextGen.size()) < kPopSize) {
            const QVector<int> &p1 = population[parentDist(rng)];
            const QVector<int> &p2 = population[parentDist(rng)];
            QVector<int> child = crossover(p1, p2, g, start, end);

            if (chance(rng) < kMutationRate)
                mutate(child, g, rng);

            nextGen.push_back(child);
        }

        population = std::move(nextGen);
        generationsDone = gen + 1;
    }

    std::sort(population.begin(), population.end(),
              [&](const QVector<int> &a, const QVector<int> &b) {
                  return pathLength(g, a) < pathLength(g, b);
              });

    const QVector<int> &best = population.front();
    const double length = pathLength(g, best);

    return QString("path=%1,length=%2,generations=%3")
        .arg(pathToString(best))
        .arg(length, 0, 'f', 0)
        .arg(generationsDone);
}
