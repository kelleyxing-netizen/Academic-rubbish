#include <stdio.h>   // 用于输入输出（printf, scanf, fgets等）
#include <stdlib.h>  // 用于动态内存分配（malloc, free）
#include <string.h>  // 用于字符串操作（strcpy, strcmp, strlen等）
#include <time.h>    // 用于随机数生成（time, srand, rand）

#define MAX_NAME_LEN 100      // 歌曲名称最大长度
#define MAX_ARTIST_LEN 100    // 歌手名称最大长度
#define MAX_ALBUM_LEN 100     // 专辑名称最大长度

// 歌曲信息结构体
typedef struct {
    char name[50];      // 歌曲名称
    char artist[50];  // 歌手
    char album[50];    // 专辑
} Song;

// 在主函数中测试结构体
/*int main() {
    Song song;
    strcpy(song.name, "天使");
    strcpy(song.artist, "五月天");
    strcpy(song.album, "测试专辑");
    printf("歌曲：%s, 歌手：%s, 专辑：%s\n", song.name, song.artist, song.album);
    return 0;
}*/

// 链表节点（用于存储歌曲列表）
typedef struct ListNode {
    Song song;              // 存储歌曲信息
    struct ListNode* next;  // 指向下一个节点的指针
} ListNode;

// 队列节点（用于播放队列）
typedef struct QueueNode {
    Song song;
    struct QueueNode* next;
} QueueNode;

// 队列结构
typedef struct {
    QueueNode* front;  // 队头指针
    QueueNode* rear;   // 队尾指针
    int size;          // 队列大小
} Queue;

// 栈节点（用于播放历史）
typedef struct StackNode {
    Song song;
    struct StackNode* next;
} StackNode;

// 栈结构
typedef struct {
    StackNode* top;  // 栈顶指针
    int size;        // 栈大小
} Stack;

// 创建链表节点
ListNode* createListNode(Song song) {
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    if (node == NULL) {
        printf("内存分配失败！\n");
        return NULL;
    }
    node->song = song;
    node->next = NULL;

    return node;
}

// 在链表末尾添加歌曲
void addSongToList(ListNode** head, Song song) {
    ListNode* newNode = createListNode(song);
    if (newNode == NULL) return;

    if (*head == NULL) {
        // 如果链表为空，新节点成为头节点
        *head = newNode;
    } else {
        // 如果链表不为空，找到最后一个节点
        ListNode* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        // 将新节点连接到链表末尾
        current->next = newNode;
    }
    printf("歌曲 \"%s\" 添加成功！\n", song.name);
}

// 在主函数中测试
/*int main() {
    ListNode* head = NULL;
    Song song1, song2;

    strcpy(song1.name, "歌曲1");
    strcpy(song1.artist, "歌手1");
    strcpy(song1.album, "专辑1");

    addSongToList(&head, song1);
    // 验证歌曲是否添加成功
    return 0;
}*/

// 根据名称查找歌曲
ListNode* findSongInList(ListNode* head, char* name) {
    ListNode* current = head;
    while (current != NULL) {
        if (strcmp(current->song.name, name) == 0) {
            return current;  // 找到匹配的歌曲
        }
        current = current->next;
    }
    return NULL;  // 未找到
}

// 根据名称删除歌曲. 返回0：代表删除失败；返回1：删除成功
int deleteSongFromList(ListNode** head, char* name) {
    if (*head == NULL) {
        printf("歌曲列表为空！\n");
        return 0;
    }

    ListNode* current = *head;
    ListNode* prev = NULL;

    // 如果删除的是头节点
    if (strcmp(current->song.name, name) == 0) {
        *head = current->next;
        free(current);
        printf("歌曲 \"%s\" 删除成功！\n", name);
        return 1;
    }

    // 查找要删除的节点
    while (current != NULL && strcmp(current->song.name, name) != 0) {
        prev = current;
        current = current->next;
    }
    // 异常判断，当前的歌是最后一个空指针
    if (current == NULL) {
        printf("未找到歌曲 \"%s\"！\n", name);
        return 0;
    }

    // 删除节点：将前一个节点的next指向当前节点的next
    prev->next = current->next;
    free(current);
    printf("歌曲 \"%s\" 删除成功！\n", name);

    return 1;
}

// 显示所有歌曲
void displayAllSongs(ListNode* head) {
    if (head == NULL) {
        printf("歌曲列表为空！\n");
        return;
    }

    printf("\n========== 歌曲列表 ==========\n");
    printf("%-4s %-30s %-20s %-30s\n", "序号", "歌曲名称", "歌手", "专辑");
    printf("------------------------------------------------------------\n");

    int index = 1;
    ListNode* current = head;
    // 遍历链表
    while (current != NULL) {
        printf("%-4d %-30s %-20s %-30s\n",
               index++,
               current->song.name,
               current->song.artist,
               current->song.album);
        current = current->next;
    }
    printf("================================\n\n");
}

// 修改歌曲信息
int modifySongInList(ListNode* head, char* name) {
    ListNode* node = findSongInList(head, name);
    if (node == NULL) {
        printf("未找到歌曲 \"%s\"！\n", name);
        return 0;
    }

    printf("当前歌曲信息：\n");
    printf("  名称：%s\n", node->song.name);
    printf("  歌手：%s\n", node->song.artist);
    printf("  专辑：%s\n", node->song.album);
    printf("\n请输入新的信息：\n");

    // 读取新名称
    printf("新名称（直接回车保持原值）：");
    char newName[MAX_NAME_LEN];
    fgets(newName, MAX_NAME_LEN, stdin);
    newName[strcspn(newName, "\n")] = 0;  // 移除换行符
    if (strlen(newName) > 0) {
        strcpy(node->song.name, newName);
    }

    // 类似地处理歌手和专辑...

    printf("修改成功！\n");
    return 1;
}

// 按名称排序（冒泡排序）
void sortSongsByName(ListNode** head) {
    if (*head == NULL || (*head)->next == NULL) {
        return;  // 空链表或只有一个节点，无需排序
    }

    int swapped;
    ListNode* ptr1;
    ListNode* lptr = NULL;

    do {
        swapped = 0;
        ptr1 = *head;

        while (ptr1->next != lptr) {
            if (strcmp(ptr1->song.name, ptr1->next->song.name) > 0) {
                // 交换歌曲数据（不交换节点，只交换数据）
                Song temp = ptr1->song;
                ptr1->song = ptr1->next->song;
                ptr1->next->song = temp;
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);

    printf("歌曲列表已按名称排序！\n");
}

// 初始化队列
void initQueue(Queue* queue) {
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

// 入队（添加歌曲到播放队列）
void enqueue(Queue* queue, Song song) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (newNode == NULL) {
        printf("内存分配失败！\n");
        return;
    }

    newNode->song = song;
    newNode->next = NULL;

    if (queue->rear == NULL) {
        // 队列为空，新节点既是队头也是队尾
        queue->front = queue->rear = newNode;
    } else {
        // 队列不为空，将新节点连接到队尾
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
    printf("歌曲 \"%s\" 已添加到播放队列！\n", song.name);
}

// 出队（播放下一首）
int dequeue(Queue* queue, Song* song) {
    if (queue->front == NULL) {
        printf("播放队列为空！\n");
        return 0;
    }

    QueueNode* temp = queue->front;
    *song = temp->song;  // 将歌曲信息复制到输出参数
    queue->front = queue->front->next;

    // 如果队列变空，rear也要设为NULL
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    queue->size--;
    return 1;
}

// 显示播放队列
void displayQueue(Queue* queue) {
    if (queue->front == NULL) {
        printf("播放队列为空！\n");
        return;
    }

    printf("\n========== 播放队列 ==========\n");
    printf("%-4s %-30s %-20s %-30s\n", "序号", "歌曲名称", "歌手", "专辑");
    printf("------------------------------------------------------------\n");

    int index = 1;
    QueueNode* current = queue->front;
    while (current != NULL) {
        printf("%-4d %-30s %-20s %-30s\n",
               index++,
               current->song.name,
               current->song.artist,
               current->song.album);
        current = current->next;
    }
    printf("================================\n");
    printf("队列中共有 %d 首歌曲\n\n", queue->size);
}

// 清空队列。 判断入参
void clearQueue(Queue* queue) {
    while (queue->front != NULL) {
        QueueNode* temp = queue->front;
        queue->front = queue->front->next;
        free(temp);
    }
    queue->rear = NULL;
    queue->size = 0;
}

// 在主函数中测试队列
/*int main() {
    Queue queue;
    initQueue(&queue);

    Song song;
    strcpy(song.name, "测试歌曲");
    // ... 设置其他字段

    enqueue(&queue, song);
    displayQueue(&queue);

    Song played;
    if (dequeue(&queue, &played)) {
        printf("播放：%s\n", played.name);
    }

    clearQueue(&queue);
    return 0;
}*/

// 初始化栈
void initStack(Stack* stack) {
    stack->top = NULL;
    stack->size = 0;
}

// 入栈（记录播放历史）
void push(Stack* stack, Song song) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (newNode == NULL) {
        printf("内存分配失败！\n");
        return;
    }

    newNode->song = song;
    newNode->next = stack->top;  // 新节点指向原来的栈顶
    stack->top = newNode;        // 新节点成为新的栈顶
    stack->size++;
}

// 出栈（查看最近播放）
int pop(Stack* stack, Song* song) {
    if (stack->top == NULL) {
        printf("播放历史为空！\n");
        return 0;
    }

    StackNode* temp = stack->top;
    *song = temp->song;
    stack->top = stack->top->next;  // 栈顶指向下一个节点
    free(temp);
    stack->size--;
    return 1;
}

// 显示播放历史（不删除）
void displayStack(Stack* stack) {
    if (stack->top == NULL) {
        printf("播放历史为空！\n");
        return;
    }

    printf("\n========== 播放历史 ==========\n");
    printf("%-4s %-30s %-20s %-30s\n", "序号", "歌曲名称", "歌手", "专辑");
    printf("------------------------------------------------------------\n");

    int index = 1;
    StackNode* current = stack->top;
    while (current != NULL) {
        printf("%-4d %-30s %-20s %-30s\n",
               index++,
               current->song.name,
               current->song.artist,
               current->song.album);
        current = current->next;
    }
    printf("================================\n");
    printf("历史记录中共有 %d 首歌曲\n\n", stack->size);
}

// 清空栈
void clearStack(Stack* stack) {
    while (stack->top != NULL) {
        StackNode* temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
    stack->size = 0;
}

// 输入歌曲信息
Song inputSong() {
    Song song;
    printf("请输入歌曲名称：");
    fgets(song.name, MAX_NAME_LEN, stdin);
    song.name[strcspn(song.name, "\n")] = 0;

    printf("请输入歌手：");
    fgets(song.artist, MAX_ARTIST_LEN, stdin);
    song.artist[strcspn(song.artist, "\n")] = 0;

    printf("请输入专辑：");
    fgets(song.album, MAX_ALBUM_LEN, stdin);
    song.album[strcspn(song.album, "\n")] = 0;

    return song;
}

// 获取链表长度
int getListLength(ListNode* head) {
    int count = 0;
    ListNode* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// 随机打乱播放队列（洗牌算法 - Fisher-Yates算法）
void shuffleQueue(Queue* queue) {
    if (queue->front == NULL || queue->size <= 1) {
        printf("播放队列为空或只有一首歌曲，无需打乱！\n");
        return;
    }

    // 将队列转换为数组（方便随机打乱）
    int size = queue->size;
    Song* songs = (Song*)malloc(size * sizeof(Song));
    if (songs == NULL) {
        printf("内存分配失败！\n");
        return;
    }

    // 将队列中的歌曲复制到数组
    QueueNode* current = queue->front;
    for (int i = 0; i < size; i++) {
        songs[i] = current->song;
        current = current->next;
    }

    // Fisher-Yates洗牌算法：从后往前，每个位置与前面的随机位置交换
    for (int i = size - 1; i > 0; i--) {
        // 生成0到i之间的随机索引
        int j = rand() % (i + 1);

        // 交换songs[i]和songs[j]
        Song temp = songs[i];
        songs[i] = songs[j];
        songs[j] = temp;
    }

    // 清空原队列
    clearQueue(queue);

    // 将打乱后的歌曲重新入队（静默模式，不打印消息）
    for (int i = 0; i < size; i++) {
        QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
        if (newNode == NULL) {
            printf("内存分配失败！\n");
            free(songs);
            return;
        }
        newNode->song = songs[i];
        newNode->next = NULL;

        if (queue->rear == NULL) {
            queue->front = queue->rear = newNode;
        } else {
            queue->rear->next = newNode;
            queue->rear = newNode;
        }
        queue->size++;
    }

    free(songs);
    printf("播放队列已随机打乱！\n");
}

// 主菜单
void showMainMenu() {
    printf("\n========== 音乐播放器 ==========\n");
    printf("1. 歌曲管理\n");
    printf("2. 播放队列管理\n");
    printf("3. 播放历史查看\n");
    printf("4. 播放歌曲（从队列中播放）\n");
    printf("5. 随机播放（从歌曲列表中随机选择）\n");
    printf("0. 退出程序\n");
    printf("================================\n");
    printf("请选择操作：");
}

// 歌曲管理子菜单
void showSongMenu() {
    printf("\n========== 歌曲管理 ==========\n");
    printf("1. 添加歌曲\n");
    printf("2. 删除歌曲\n");
    printf("3. 修改歌曲\n");
    printf("4. 查询歌曲\n");
    printf("5. 显示所有歌曲\n");
    printf("6. 按名称排序\n");
    printf("0. 返回主菜单\n");
    printf("================================\n");
    printf("请选择操作：");
}

// 播放队列管理子菜单
void showQueueMenu() {
    printf("\n========== 播放队列管理 ==========\n");
    printf("1. 添加歌曲到播放队列\n");
    printf("2. 显示播放队列\n");
    printf("3. 清空播放队列\n");
    printf("4. 随机打乱播放队列\n");
    printf("0. 返回主菜单\n");
    printf("================================\n");
    printf("请选择操作：");
}

int main()
{
    // 1. 声明变量
    ListNode *songList = NULL; // 歌曲列表（链表）
    Queue playQueue;           // 播放队列
    Stack playHistory;         // 播放历史

    // 2. 初始化
    initQueue(&playQueue);
    initStack(&playHistory);
    srand((unsigned int)time(NULL)); // 初始化随机数种子

    // 3. 变量声明
    int choice, subChoice;
    char name[MAX_NAME_LEN];
    Song song;

    printf("欢迎使用音乐播放器！\n");

    // 4. 主循环
    while (1)
    {
        showMainMenu();
        scanf("%d", &choice);
        getchar(); // 清除输入缓冲区

        switch (choice)
        {
        case 1: // 歌曲管理
            while (1)
            {
                showSongMenu();
                scanf("%d", &subChoice);
                getchar();

                switch (subChoice)
                {
                case 1: // 添加歌曲
                    song = inputSong();
                    addSongToList(&songList, song);
                    break;
                case 2: // 删除歌曲
                    printf("请输入要删除的歌曲名称：");
                    fgets(name, MAX_NAME_LEN, stdin);
                    name[strcspn(name, "\n")] = 0;
                    deleteSongFromList(&songList, name);
                    break;
                case 3: // 修改歌曲
                    printf("请输入要修改的歌曲名称：");
                    fgets(name, MAX_NAME_LEN, stdin);
                    name[strcspn(name, "\n")] = 0;
                    modifySongInList(songList, name);
                    break;
                case 4: // 查询歌曲
                    printf("请输入要查询的歌曲名称：");
                    fgets(name, MAX_NAME_LEN, stdin);
                    name[strcspn(name, "\n")] = 0;
                    {
                        ListNode *found = findSongInList(songList, name);
                        if (found != NULL)
                        {
                            printf("\n找到歌曲：\n");
                            printf("  名称：%s\n", found->song.name);
                            printf("  歌手：%s\n", found->song.artist);
                            printf("  专辑：%s\n", found->song.album);
                        }
                        else
                        {
                            printf("未找到歌曲 \"%s\"！\n", name);
                        }
                    }
                    break;
                case 5: // 显示所有歌曲
                    displayAllSongs(songList);
                    break;
                case 6: // 按名称排序
                    sortSongsByName(&songList);
                    break;
                case 0: // 返回主菜单
                    goto main_menu;
                default:
                    printf("无效的选择，请重新输入！\n");
                }
            }
        main_menu:
            break;
        case 2: // 播放队列管理
            while (1)
            {
                showQueueMenu();
                scanf("%d", &subChoice);
                getchar();

                switch (subChoice)
                {
                case 1: // 添加歌曲到播放队列
                    printf("请输入要添加到播放队列的歌曲名称：");
                    fgets(name, MAX_NAME_LEN, stdin);
                    name[strcspn(name, "\n")] = 0;
                    {
                        ListNode *found = findSongInList(songList, name);
                        if (found != NULL)
                        {
                            enqueue(&playQueue, found->song);
                        }
                        else
                        {
                            printf("未找到歌曲 \"%s\"！请先在歌曲管理中添加该歌曲。\n", name);
                        }
                    }
                    break;
                case 2: // 显示播放队列
                    displayQueue(&playQueue);
                    break;
                case 3: // 清空播放队列
                    clearQueue(&playQueue);
                    printf("播放队列已清空！\n");
                    break;
                case 4: // 随机打乱播放队列
                    shuffleQueue(&playQueue);
                    break;
                case 0: // 返回主菜单
                    goto main_menu2;
                default:
                    printf("无效的选择，请重新输入！\n");
                }
            }
        main_menu2:
            break;
        case 3: // 播放历史查看
            displayStack(&playHistory);
            break;

        case 4: // 播放歌曲（从队列中播放）
            if (dequeue(&playQueue, &song))
            {
                printf("\n正在播放：\n");
                printf("  名称：%s\n", song.name);
                printf("  歌手：%s\n", song.artist);
                printf("  专辑：%s\n", song.album);
                printf("播放完成！\n");
                // 将播放的歌曲加入历史记录
                push(&playHistory, song);
            }
            break;

        case 5: // 随机播放（从歌曲列表中随机选择）
            shuffleQueue(&playQueue);
            break;

        case 0: // 退出程序
            printf("感谢使用，再见！\n");
            // 释放链表内存
            while (songList != NULL)
            {
                ListNode *temp = songList;
                songList = songList->next;
                free(temp);
            }
            // 释放队列内存
            clearQueue(&playQueue);
            // 释放栈内存
            clearStack(&playHistory);
            return 0;

        default:
            printf("无效的选择，请重新输入！\n");
        }
    }

    return 0;
}

