#include <stdio.h>   // 用于输入输出（printf, scanf, fgets等）
#include <stdlib.h>  // 用于动态内存分配（malloc, free）
#include <string.h>  // 用于字符串操作（strcpy, strcmp, strncpy等）
#include <stdbool.h> // 用于布尔类型（bool, true, false）
#define MAX_USERS 1000      // 最大用户数量
#define MAX_NAME_LEN 64    // 用户名称最大长度
// 用户节点
typedef struct User {
    int id;                      // 用户ID（唯一标识）
    char name[MAX_NAME_LEN];     // 用户名称
    bool exists;                 // 用户是否存在（用于数组索引）
} User;
// 图的邻接表节点（链表节点）
typedef struct AdjListNode {
    int userId;                  // 好友的用户ID
    struct AdjListNode* next;    // 指向下一个好友节点
} AdjListNode;
// 图的邻接表（每个用户的好友列表）
typedef struct AdjList {
    AdjListNode* head;  // 链表头指针
} AdjList;
// 图结构
typedef struct Graph {
    User users[MAX_USERS];      // 用户数组
    AdjList adjLists[MAX_USERS]; // 邻接表数组
    int userCount;               // 当前用户数量
    int nextId;                  // 下一个可用的用户ID
} Graph;
// 在主函数中测试结构体
/*int main() {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    printf("图结构大小: %zu 字节\n", sizeof(Graph));
    free(graph);
    return 0;
}*/
// 初始化图
Graph* initGraph() {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (graph == NULL) {
        printf("内存分配失败！\n");
        return NULL;
    }

    graph->userCount = 0;
    graph->nextId = 1;

    // 初始化所有用户和邻接表
    for (int i = 0; i < MAX_USERS; i++) {
        graph->users[i].exists = false;
        graph->adjLists[i].head = NULL;
    }

    return graph;
}
// 创建邻接表节点
AdjListNode* createAdjListNode(int userId) {
    AdjListNode* node = (AdjListNode*)malloc(sizeof(AdjListNode));
    if (node == NULL) {
        printf("内存分配失败！\n");
        return NULL;
    }

    node->userId = userId;
    node->next = NULL;

    return node;
}
// 添加用户
int addUser(Graph* graph, const char* name) {
    if (graph->userCount >= MAX_USERS) {
        printf("用户数量已达上限！\n");
        return -1;
    }

    int id = graph->nextId++;
    graph->users[id].id = id;
    strncpy(graph->users[id].name, name, MAX_NAME_LEN - 1);
    graph->users[id].name[MAX_NAME_LEN - 1] = '\0';  // 确保字符串结束
    graph->users[id].exists = true;
    graph->userCount++;

    printf("用户 \"%s\" 添加成功！用户ID: %d\n", name, id);
    return id;
}
// 在主函数中测试
/*int main() {
    Graph* graph = initGraph();
    int id1 = addUser(graph, "张三");
    int id2 = addUser(graph, "李四");
    printf("用户1 ID: %d, 用户2 ID: %d\n", id1, id2);
    free(graph);
    return 0;
}*/
// 添加好友关系（无向图，双向添加）
int addFriend(Graph* graph, int userId1, int userId2) {
    // 检查用户是否存在
    if (!graph->users[userId1].exists || !graph->users[userId2].exists) {
        printf("用户不存在！\n");
        return 0;
    }

    // 不能添加自己为好友
    if (userId1 == userId2) {
        printf("不能添加自己为好友！\n");
        return 0;
    }

    // 检查是否已经是好友
    AdjListNode* current = graph->adjLists[userId1].head;
    while (current != NULL) {
        if (current->userId == userId2) {
            printf("用户 %d 和用户 %d 已经是好友！\n", userId1, userId2);
            return 0;
        }
        current = current->next;
    }

    // 添加 userId2 到 userId1 的邻接表（头插法）
    AdjListNode* node1 = createAdjListNode(userId2);
    if (node1 == NULL) return 0;
    node1->next = graph->adjLists[userId1].head;
    graph->adjLists[userId1].head = node1;

    // 添加 userId1 到 userId2 的邻接表（无向图，双向）
    AdjListNode* node2 = createAdjListNode(userId1);
    if (node2 == NULL) {
        // 如果第二个节点创建失败，需要回滚第一个节点
        graph->adjLists[userId1].head = node1->next;
        free(node1);
        return 0;
    }
    node2->next = graph->adjLists[userId2].head;
    graph->adjLists[userId2].head = node2;

    printf("好友关系添加成功！用户 %d 和用户 %d 现在是好友。\n", userId1, userId2);
    return 1;
}
// 测试添加好友关系
/*int main() {
    Graph* graph = initGraph();
    addUser(graph, "张三");
    addUser(graph, "李四");
    addFriend(graph, 1, 2);
    return 0;
}*/
// 删除好友关系
int removeFriend(Graph* graph, int userId1, int userId2) {
    if (!graph->users[userId1].exists || !graph->users[userId2].exists) {
        printf("用户不存在！\n");
        return 0;
    }

    // 从 userId1 的邻接表中删除 userId2
    AdjListNode* prev = NULL;
    AdjListNode* current = graph->adjLists[userId1].head;
    while (current != NULL && current->userId != userId2) {
        prev = current;
        current = current->next;
    }
    if (current != NULL) {
        if (prev == NULL) {
            // 删除头节点
            graph->adjLists[userId1].head = current->next;
        } else {
            // 删除中间或尾节点
            prev->next = current->next;
        }
        free(current);
    }

    // 从 userId2 的邻接表中删除 userId1（无向图，双向删除）
    prev = NULL;
    current = graph->adjLists[userId2].head;
    while (current != NULL && current->userId != userId1) {
        prev = current;
        current = current->next;
    }
    if (current != NULL) {
        if (prev == NULL) {
            graph->adjLists[userId2].head = current->next;
        } else {
            prev->next = current->next;
        }
        free(current);
    }

    printf("好友关系删除成功！\n");
    return 1;
}
// 获取用户的所有好友
void getFriends(Graph* graph, int userId, int* friends, int* count) {
    *count = 0;
    AdjListNode* current = graph->adjLists[userId].head;
    while (current != NULL) {
        friends[(*count)++] = current->userId;
        current = current->next;
    }
}
// BFS查找最短路径（几度好友）
int findShortestPath(Graph* graph, int fromUserId, int toUserId,
                     int* path, int* pathLength) {
    if (!graph->users[fromUserId].exists || !graph->users[toUserId].exists) {
        return -1;  // 用户不存在
    }

    if (fromUserId == toUserId) {
        path[0] = fromUserId;
        *pathLength = 1;
        return 0;  // 0度（自己）
    }

    // BFS队列
    int queue[MAX_USERS];
    int front = 0, rear = 0;
    bool visited[MAX_USERS] = {false};
    int parent[MAX_USERS];
    int distance[MAX_USERS];

    // 初始化
    for (int i = 0; i < MAX_USERS; i++) {
        parent[i] = -1;
        distance[i] = -1;
    }

    // 从起始节点开始BFS
    queue[rear++] = fromUserId;
    visited[fromUserId] = true;
    distance[fromUserId] = 0;

    while (front < rear) {
        int current = queue[front++];

        // 找到目标节点
        if (current == toUserId) {
            // 回溯路径
            *pathLength = 0;
            int node = toUserId;
            while (node != -1) {
                path[(*pathLength)++] = node;
                node = parent[node];
            }

            // 反转路径（因为是从目标回溯到起点）
            for (int i = 0; i < *pathLength / 2; i++) {
                int temp = path[i];
                path[i] = path[*pathLength - 1 - i];
                path[*pathLength - 1 - i] = temp;
            }

            return distance[toUserId];
        }

        // 遍历邻接节点
        AdjListNode* adjNode = graph->adjLists[current].head;
        while (adjNode != NULL) {
            int neighbor = adjNode->userId;
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                queue[rear++] = neighbor;
                parent[neighbor] = current;
                distance[neighbor] = distance[current] + 1;
            }
            adjNode = adjNode->next;
        }
    }

    return -1;  // 不可达
}
// 查找共同好友
void findCommonFriends(Graph* graph, int userId1, int userId2,
                       int* commonFriends, int* count) {
    *count = 0;

    // 获取两个用户的好友列表
    int friends1[MAX_USERS], count1;
    int friends2[MAX_USERS], count2;

    getFriends(graph, userId1, friends1, &count1);
    getFriends(graph, userId2, friends2, &count2);

    // 查找交集（共同好友）
    for (int i = 0; i < count1; i++) {
        for (int j = 0; j < count2; j++) {
            if (friends1[i] == friends2[j]) {
                commonFriends[(*count)++] = friends1[i];
                break;
            }
        }
    }
}
// 推荐好友（好友的好友，排除已经是好友的）
void recommendFriends(Graph* graph, int userId,
                      int* recommendations, int* count) {
    *count = 0;
    bool isFriend[MAX_USERS] = {false};
    bool visited[MAX_USERS] = {false};

    // 标记直接好友
    int friends[MAX_USERS], friendCount;
    getFriends(graph, userId, friends, &friendCount);
    for (int i = 0; i < friendCount; i++) {
        isFriend[friends[i]] = true;
    }
    isFriend[userId] = true;  // 排除自己

    // 遍历好友的好友
    for (int i = 0; i < friendCount; i++) {
        int friendId = friends[i];
        AdjListNode* adjNode = graph->adjLists[friendId].head;
        while (adjNode != NULL) {
            int friendOfFriend = adjNode->userId;
            // 排除已经是好友的和自己
            if (!isFriend[friendOfFriend] && !visited[friendOfFriend]) {
                recommendations[(*count)++] = friendOfFriend;
                visited[friendOfFriend] = true;  // 避免重复
            }
            adjNode = adjNode->next;
        }
    }
}
// 查找朋友圈（连通分量）- 使用DFS
void findConnectedComponent(Graph* graph, int userId,
                           bool* visited, int* component, int* count) {
    visited[userId] = true;
    component[(*count)++] = userId;

    // 递归遍历所有邻接节点
    AdjListNode* current = graph->adjLists[userId].head;
    while (current != NULL) {
        if (!visited[current->userId]) {
            findConnectedComponent(graph, current->userId, visited, component, count);
        }
        current = current->next;
    }
}
// 显示所有用户
void displayAllUsers(Graph* graph) {
    if (graph->userCount == 0) {
        printf("暂无用户！\n");
        return;
    }

    printf("\n========== 用户列表 ==========\n");
    printf("%-6s %-30s %-10s\n", "ID", "用户名", "好友数");
    printf("------------------------------------------------------------\n");

    for (int i = 1; i < MAX_USERS; i++) {
        if (graph->users[i].exists) {
            int friends[MAX_USERS], count;
            getFriends(graph, i, friends, &count);
            printf("%-6d %-30s %-10d\n", i, graph->users[i].name, count);
        }
    }
    printf("================================\n\n");
}
// 显示用户的好友列表
void displayFriends(Graph* graph, int userId) {
    if (!graph->users[userId].exists) {
        printf("用户不存在！\n");
        return;
    }

    int friends[MAX_USERS], count;
    getFriends(graph, userId, friends, &count);

    printf("\n用户 %d (%s) 的好友列表：\n", userId, graph->users[userId].name);
    if (count == 0) {
        printf("  暂无好友\n");
    } else {
        for (int i = 0; i < count; i++) {
            printf("  - 用户 %d: %s\n", friends[i], graph->users[friends[i]].name);
        }
    }
    printf("\n");
}
// 显示最短路径
void displayPath(Graph* graph, int* path, int pathLength, int degree) {
    if (degree < 0) {
        printf("用户之间不可达！\n");
        return;
    }

    printf("\n最短路径（%d度好友）：\n", degree);
    for (int i = 0; i < pathLength; i++) {
        printf("  用户 %d: %s", path[i], graph->users[path[i]].name);
        if (i < pathLength - 1) {
            printf(" → ");
        }
    }
    printf("\n\n");
}
// 主菜单
void showMainMenu() {
    printf("\n========== 社交网络系统 ==========\n");
    printf("1. 添加用户\n");
    printf("2. 添加好友关系\n");
    printf("3. 删除好友关系\n");
    printf("4. 显示所有用户\n");
    printf("5. 查看好友列表\n");
    printf("6. 查找最短路径（几度好友）\n");
    printf("7. 查找共同好友\n");
    printf("8. 推荐好友\n");
    printf("9. 查找朋友圈\n");
    printf("10. 统计信息\n");
    printf("0. 退出程序\n");
    printf("==================================\n");
    printf("请选择操作：");
}
// 输入用户信息
void inputUserInfo(char* name) {
    printf("请输入用户名称：");
    fgets(name, MAX_NAME_LEN, stdin);
    name[strcspn(name, "\n")] = 0;  // 移除换行符
}
// 统计信息
void displayStatistics(Graph* graph) {
    int totalFriends = 0;
    int maxFriends = 0;
    int minFriends = MAX_USERS;
    int userWithMaxFriends = -1;

    // 遍历所有用户，统计信息
    for (int i = 1; i < MAX_USERS; i++) {
        if (graph->users[i].exists) {
            int friends[MAX_USERS], count;
            getFriends(graph, i, friends, &count);
            totalFriends += count;
            if (count > maxFriends) {
                maxFriends = count;
                userWithMaxFriends = i;
            }
            if (count < minFriends) {
                minFriends = count;
            }
        }
    }

    printf("\n========== 统计信息 ==========\n");
    printf("用户总数：%d\n", graph->userCount);
    printf("好友关系总数：%d\n", totalFriends / 2);  // 无向图，每条边算两次
    if (graph->userCount > 0) {
        printf("平均好友数：%.2f\n", (float)totalFriends / graph->userCount);
        printf("最多好友数：%d（用户 %d: %s）\n",
               maxFriends, userWithMaxFriends, graph->users[userWithMaxFriends].name);
        printf("最少好友数：%d\n", minFriends);
    }
    printf("==============================\n\n");
}
int main() {
    // 1. 初始化图
    Graph* graph = initGraph();

    // 2. 声明变量
    int choice;
    int userId1, userId2, userId;
    char name[MAX_NAME_LEN];
    int path[MAX_USERS], pathLength;
    int commonFriends[MAX_USERS], commonCount;
    int recommendations[MAX_USERS], recCount;

    printf("欢迎使用社交网络好友关系系统！\n");

    // 3. 添加测试数据（可选）
    addUser(graph, "张三");
    addUser(graph, "李四");
    addUser(graph, "王五");
    addUser(graph, "赵六");
    addFriend(graph, 1, 2);
    addFriend(graph, 1, 3);
    addFriend(graph, 2, 4);
    addFriend(graph, 3, 4);

    // 4. 主循环
    while (1) {
        showMainMenu();
        scanf("%d", &choice);
        getchar(); // 清除输入缓冲区

        switch (choice) {
            case 1: // 添加用户
                inputUserInfo(name);
                addUser(graph, name);
                break;

            case 2: // 添加好友关系
                printf("请输入用户1 ID：");
                scanf("%d", &userId1);
                getchar();
                printf("请输入用户2 ID：");
                scanf("%d", &userId2);
                getchar();
                addFriend(graph, userId1, userId2);
                break;

            case 3: // 删除好友关系
                printf("请输入用户1 ID：");
                scanf("%d", &userId1);
                getchar();
                printf("请输入用户2 ID：");
                scanf("%d", &userId2);
                getchar();
                removeFriend(graph, userId1, userId2);
                break;

            case 4: // 显示所有用户
                displayAllUsers(graph);
                break;

            case 5: // 查看好友列表
                printf("请输入用户ID：");
                scanf("%d", &userId);
                getchar();
                displayFriends(graph, userId);
                break;

            case 6: // 查找最短路径
                printf("请输入起始用户ID：");
                scanf("%d", &userId1);
                getchar();
                printf("请输入目标用户ID：");
                scanf("%d", &userId2);
                getchar();
                {
                    int degree = findShortestPath(graph, userId1, userId2, path, &pathLength);
                    displayPath(graph, path, pathLength, degree);
                }
                break;

            case 7: // 查找共同好友
                printf("请输入用户1 ID：");
                scanf("%d", &userId1);
                getchar();
                printf("请输入用户2 ID：");
                scanf("%d", &userId2);
                getchar();
                {
                    findCommonFriends(graph, userId1, userId2, commonFriends, &commonCount);
                    if (commonCount == 0) {
                        printf("\n用户 %d 和用户 %d 没有共同好友。\n\n", userId1, userId2);
                    } else {
                        printf("\n用户 %d 和用户 %d 的共同好友：\n", userId1, userId2);
                        for (int i = 0; i < commonCount; i++) {
                            printf("  - 用户 %d: %s\n",
                                   commonFriends[i],
                                   graph->users[commonFriends[i]].name);
                        }
                        printf("\n");
                    }
                }
                break;

            case 8: // 推荐好友
                printf("请输入用户ID：");
                scanf("%d", &userId);
                getchar();
                {
                    recommendFriends(graph, userId, recommendations, &recCount);
                    if (recCount == 0) {
                        printf("\n暂无推荐好友。\n\n");
                    } else {
                        printf("\n推荐好友（好友的好友）：\n");
                        for (int i = 0; i < recCount; i++) {
                            printf("  - 用户 %d: %s\n",
                                   recommendations[i],
                                   graph->users[recommendations[i]].name);
                        }
                        printf("\n");
                    }
                }
                break;

            case 9: // 查找朋友圈
                printf("请输入用户ID：");
                scanf("%d", &userId);
                getchar();
                {
                    bool visited[MAX_USERS] = {false};
                    int component[MAX_USERS], count = 0;
                    findConnectedComponent(graph, userId, visited, component, &count);

                    printf("\n用户 %d 的朋友圈（连通分量）：\n", userId);
                    for (int i = 0; i < count; i++) {
                        printf("  - 用户 %d: %s\n", component[i], graph->users[component[i]].name);
                    }
                    printf("朋友圈共有 %d 个用户\n\n", count);
                }
                break;

            case 10: // 统计信息
                displayStatistics(graph);
                break;

            case 0: // 退出程序
                printf("感谢使用，再见！\n");
                // 释放图的内存
                for (int i = 0; i < MAX_USERS; i++) {
                    AdjListNode* current = graph->adjLists[i].head;
                    while (current != NULL) {
                        AdjListNode* temp = current;
                        current = current->next;
                        free(temp);
                    }
                }
                free(graph);
                return 0;

            default:
                printf("无效的选择，请重新输入！\n");
        }
    }

    return 0;
}
