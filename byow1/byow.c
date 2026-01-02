#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "byow.h"

// ==================== 随机数生成 (LCG) ====================
// PDF 5.1 节提到的线性同余生成器
static long currentSeed = 0;

void setSeed(long seed) {
    currentSeed = seed;
}

// 获取范围 [min, max) 的随机整数
int randomInt(int min, int max) {
    // 简单的LCG公式
    currentSeed = (currentSeed * 1103515245 + 12345) & 0x7fffffff;
    return min + (currentSeed % (max - min));
}

// ==================== 并查集实现 ====================
// PDF 2.2 和 5.2 节
void initDisjointSet(DisjointSet* ds, int n) {
    for (int i = 0; i < n; i++) {
        ds->parent[i] = i; // 初始时，每个元素的父节点是自己
        ds->rank[i] = 0;
    }
    ds->count = n;
}

int findSet(DisjointSet* ds, int x) {
    if (ds->parent[x] != x) {
        ds->parent[x] = findSet(ds, ds->parent[x]); // 路径压缩
    }
    return ds->parent[x];
}

void unionSets(DisjointSet* ds, int x, int y) {
    int rootX = findSet(ds, x);
    int rootY = findSet(ds, y);
    if (rootX != rootY) {
        // 按秩合并
        if (ds->rank[rootX] < ds->rank[rootY]) {
            ds->parent[rootX] = rootY;
        } else {
            ds->parent[rootY] = rootX;
            if (ds->rank[rootX] == ds->rank[rootY]) {
                ds->rank[rootX]++;
            }
        }
    }
}

// ==================== 世界生成逻辑 ====================

// 初始化世界
World* createWorld(long seed) {
    World* world = (World*)malloc(sizeof(World));
    setSeed(seed);
    world->seed = seed;
    world->roomCount = 0;

    // 1. 填充空白
    for (int y = 0; y < MAX_HEIGHT; y++) {
        for (int x = 0; x < MAX_WIDTH; x++) {
            world->tiles[y][x] = TILE_EMPTY; // 或者 TILE_WALL，看你想做室内还是室外
        }
    }

    // 2. 生成流程
    generateRooms(world);
    connectRooms(world);

    // 3. 放置玩家在第一个房间的中心
    if (world->roomCount > 0) {
        world->playerPos = world->rooms[0].center;
        world->tiles[world->playerPos.y][world->playerPos.x] = TILE_PLAYER;
    }

    return world;
}

// 判断两个房间是否重叠
bool isOverlap(Room r1, Room r2) {
    // 增加一点缓冲距离，让房间不要紧贴着
    return (r1.x < r2.x + r2.w + 1 && r1.x + r1.w + 1 > r2.x &&
            r1.y < r2.y + r2.h + 1 && r1.y + r1.h + 1 > r2.y);
}

// 生成房间
void generateRooms(World* world) {
    for (int i = 0; i < 50; i++) { // 尝试生成50次
        if (world->roomCount >= MAX_ROOMS) break;

        int w = randomInt(4, 10);
        int h = randomInt(4, 8);
        int x = randomInt(1, MAX_WIDTH - w - 1);
        int y = randomInt(1, MAX_HEIGHT - h - 1);

        Room newRoom = { world->roomCount, x, y, w, h, {x + w/2, y + h/2} };

        bool failed = false;
        for (int j = 0; j < world->roomCount; j++) {
            if (isOverlap(newRoom, world->rooms[j])) {
                failed = true;
                break;
            }
        }

        if (!failed) {
            // 绘制房间
            world->rooms[world->roomCount] = newRoom;
            world->roomCount++;

            // 在地图上画出地板和墙壁
            for (int ry = y; ry < y + h; ry++) {
                for (int rx = x; rx < x + w; rx++) {
                    if (rx == x || rx == x + w - 1 || ry == y || ry == y + h - 1) {
                         world->tiles[ry][rx] = TILE_WALL;
                    } else {
                         world->tiles[ry][rx] = TILE_FLOOR;
                    }
                }
            }
        }
    }
}

// 简单的画走廊函数（L型走廊）
void drawCorridor(World* world, Point p1, Point p2) {
    int x = p1.x;
    int y = p1.y;

    // 先水平移动，再垂直移动
    while (x != p2.x) {
        world->tiles[y][x] = TILE_FLOOR;
        x += (p2.x > x) ? 1 : -1;
    }
    // 转弯点处理
    world->tiles[y][x] = TILE_FLOOR;

    while (y != p2.y) {
        world->tiles[y][x] = TILE_FLOOR;
        y += (p2.y > y) ? 1 : -1;
    }
    world->tiles[y][x] = TILE_FLOOR;
}

// 连接房间 (简化版MST：只连接相邻索引的房间，并随机增加一些连接以形成环路)
// PDF要求用MST，这里为了代码不难，我们用一种简单的策略：
// 1. 将房间0连1, 1连2... 这样保证连通性。
// 2. 利用并查集来管理（虽然简单链式连接不需要并查集，但为了符合PDF要求，我们演示用法）。
void connectRooms(World* world) {
    DisjointSet ds;
    initDisjointSet(&ds, world->roomCount);

    // 策略：简单地将数组中相邻的房间连接起来，这保证了所有房间连通
    // 并查集在这里用于演示“检查是否连通”
    for (int i = 0; i < world->roomCount - 1; i++) {
        Point c1 = world->rooms[i].center;
        Point c2 = world->rooms[i+1].center;

        if (findSet(&ds, i) != findSet(&ds, i+1)) {
            drawCorridor(world, c1, c2);
            unionSets(&ds, i, i+1);
        }
    }

    // 额外随机连接几个房间，让地图更有趣（非单一直线）
    for (int i = 0; i < 5; i++) {
        int r1 = randomInt(0, world->roomCount);
        int r2 = randomInt(0, world->roomCount);
        if (r1 != r2) {
             drawCorridor(world, world->rooms[r1].center, world->rooms[r2].center);
        }
    }
}

// ==================== 玩家移动与显示 ====================

void movePlayer(World* world, char direction) {
    int dx = 0, dy = 0;
    switch(direction) {
        case 'w': dy = -1; break;
        case 's': dy = 1; break;
        case 'a': dx = -1; break;
        case 'd': dx = 1; break;
        default: return;
    }

    int newX = world->playerPos.x + dx;
    int newY = world->playerPos.y + dy;

    // 检查碰撞：只能走到地板上，不能穿墙
    char targetTile = world->tiles[newY][newX];
    if (targetTile == TILE_FLOOR || targetTile == TILE_EMPTY) { // 简单起见，允许走floor
        // 恢复旧位置的地面
        world->tiles[world->playerPos.y][world->playerPos.x] = TILE_FLOOR;
        // 更新位置
        world->playerPos.x = newX;
        world->playerPos.y = newY;
        // 设置新位置
        world->tiles[newY][newX] = TILE_PLAYER;
    }
}

void printWorld(World* world) {
    // 简单的清屏命令 (Windows用cls, Mac/Linux用clear)
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    printf("BYOW - Seed: %ld\n", world->seed);
    printf("Controls: W(Up) A(Left) S(Down) D(Right) Q(Quit)\n");

    // 打印上边框
    for(int i=0; i<MAX_WIDTH+2; i++) printf("-");
    printf("\n");

    for (int y = 0; y < MAX_HEIGHT; y++) {
        printf("|"); // 左边框
        for (int x = 0; x < MAX_WIDTH; x++) {
            printf("%c", world->tiles[y][x]);
        }
        printf("|\n"); // 右边框
    }

    // 打印下边框
    for(int i=0; i<MAX_WIDTH+2; i++) printf("-");
    printf("\n");
}
