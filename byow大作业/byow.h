#ifndef BYOW_H
#define BYOW_H

#include <stdbool.h>

// ==================== 常量定义 ====================
#define MAX_WIDTH 80      // 地图宽度 (控制台通常一行80字符)
#define MAX_HEIGHT 25     // 地图高度
#define MAX_ROOMS 20      // 最大房间数
#define TILE_WALL '#'     // 墙壁字符
#define TILE_FLOOR '.'    // 地板字符
#define TILE_EMPTY ' '    // 空白字符
#define TILE_PLAYER '@'   // 玩家字符

// ==================== 数据结构定义 ====================

// 坐标点
typedef struct {
    int x;
    int y;
} Point;

// 房间结构
typedef struct {
    int id;
    int x, y;       // 左上角坐标
    int w, h;       // 宽和高
    Point center;   // 中心点
} Room;

// 并查集结构 (用于Kruskal算法生成MST)
typedef struct {
    int parent[MAX_ROOMS];
    int rank[MAX_ROOMS]; // 用于优化
    int count;
} DisjointSet;

// 边的结构 (用于MST，连接两个房间)
typedef struct {
    int roomA_id;
    int roomB_id;
    int distance; // 权重
} Edge;

// 整个世界的数据结构
typedef struct {
    char tiles[MAX_HEIGHT][MAX_WIDTH]; // 地图瓦片
    Room rooms[MAX_ROOMS];             // 房间数组
    int roomCount;                     // 当前房间数
    Point playerPos;                   // 玩家位置
    long seed;                         // 随机种子
} World;

// ==================== 函数声明 ====================

// 随机数生成 (LCG算法)
void setSeed(long seed);
int randomInt(int min, int max);

// 并查集操作
void initDisjointSet(DisjointSet* ds, int n);
int findSet(DisjointSet* ds, int x);
void unionSets(DisjointSet* ds, int x, int y);

// 世界生成核心函数
World* createWorld(long seed);
void generateRooms(World* world);
void connectRooms(World* world);

// 游戏操作
void movePlayer(World* world, char direction);
void printWorld(World* world);

#endif
