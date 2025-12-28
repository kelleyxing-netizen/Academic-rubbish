#include <stdio.h>   // 用于输入输出（printf, scanf, fgets等）
#include <stdlib.h>  // 用于动态内存分配（malloc, free, realloc）
#include <string.h>  // 用于字符串操作（strcpy, strcmp, strlen等）
#include <time.h>    // 用于时间戳生成（time）
#define MAX_CONTENT_LEN 512      // 评论内容最大长度
#define MAX_AUTHOR_LEN 64        // 作者名称最大长度
#define MAX_DEPTH 3              // 最大嵌套深度（抖音限制3层）
#define INIT_CHILDREN_CAPACITY 4 // 初始子节点数组容量
// 评论节点结构体（多叉树节点）
typedef struct CommentNode {
    int id;                      // 评论ID（唯一标识）
    char content[MAX_CONTENT_LEN]; // 评论内容
    char author[MAX_AUTHOR_LEN];   // 作者名称
    time_t timestamp;              // 时间戳
    int likeCount;                 // 点赞数

    // 树结构部分
    struct CommentNode* parent;     // 父节点指针（NULL表示主评论）
    struct CommentNode** children;   // 子节点数组（动态分配）
    int childCount;                // 当前子节点数量
    int childCapacity;              // 子节点数组容量
    int depth;                     // 嵌套深度（0表示主评论）
} CommentNode;
// 在主函数中测试结构体
/*int main() {
    CommentNode* comment = (CommentNode*)malloc(sizeof(CommentNode));
    comment->id = 1;
    strcpy(comment->content, "这是一条测试评论");
    strcpy(comment->author, "测试用户");
    comment->parent = NULL;
    comment->children = NULL;
    comment->childCount = 0;
    comment->depth = 0;

    printf("评论ID: %d\n", comment->id);
    printf("内容: %s\n", comment->content);
    free(comment);
    return 0;
}*/
// 评论系统结构体（管理所有评论）
typedef struct {
    CommentNode** rootComments;  // 主评论数组（森林）
    int rootCount;               // 主评论数量
    int rootCapacity;            // 主评论数组容量
    int nextId;                  // 下一个可用的评论ID
} CommentSystem;
// 创建新的评论节点
CommentNode* createCommentNode(int id, char* content, char* author) {
    CommentNode* node = (CommentNode*)malloc(sizeof(CommentNode));
    if (node == NULL) {
        printf("内存分配失败！\n");
        return NULL;
    }

    // 初始化基本字段
    node->id = id;
    strcpy(node->content, content);
    strcpy(node->author, author);
    node->timestamp = time(NULL);
    node->likeCount = 0;

    // 初始化树结构字段
    node->parent = NULL;
    node->children = NULL;
    node->childCount = 0;
    node->childCapacity = 0;
    node->depth = 0;

    return node;
}
// 添加回复（建立父子关系）
int addReply(CommentNode* parent, CommentNode* reply) {
    // 检查深度限制
    if (parent->depth >= MAX_DEPTH) {
        printf("超过最大嵌套深度（%d层）！无法添加回复。\n", MAX_DEPTH);
        return 0;
    }

    // 检查是否需要扩展数组
    if (parent->childCount >= parent->childCapacity) {
        int newCapacity = (parent->childCapacity == 0) ?
                          INIT_CHILDREN_CAPACITY :
                          parent->childCapacity * 2;

        CommentNode** newChildren = (CommentNode**)realloc(
            parent->children,
            newCapacity * sizeof(CommentNode*)
        );

        if (newChildren == NULL) {
            printf("内存扩展失败！\n");
            return 0;
        }

        parent->children = newChildren;
        parent->childCapacity = newCapacity;
    }

    // 建立父子关系
    parent->children[parent->childCount] = reply;
    reply->parent = parent;
    reply->depth = parent->depth + 1;
    parent->childCount++;

    printf("回复添加成功！\n");
    return 1;
}
// 在主函数中测试
/*int main() {
    CommentNode* mainComment = createCommentNode(1, "主评论", "用户A");
    CommentNode* reply1 = createCommentNode(2, "回复1", "用户B");

    if (addReply(mainComment, reply1)) {
        printf("回复添加成功！\n");
        printf("主评论深度: %d\n", mainComment->depth);
        printf("回复深度: %d\n", reply1->depth);
    }

    return 0;
}*/
// 初始化评论系统
void initCommentSystem(CommentSystem* system) {
    system->rootComments = NULL;
    system->rootCount = 0;
    system->rootCapacity = 0;
    system->nextId = 1;
}

// 添加主评论
int addRootComment(CommentSystem* system, CommentNode* comment) {
    // 检查是否需要扩展数组
    if (system->rootCount >= system->rootCapacity) {
        int newCapacity = (system->rootCapacity == 0) ?
                          INIT_CHILDREN_CAPACITY :
                          system->rootCapacity * 2;

        CommentNode** newRoots = (CommentNode**)realloc(
            system->rootComments,
            newCapacity * sizeof(CommentNode*)
        );

        if (newRoots == NULL) {
            printf("内存扩展失败！\n");
            return 0;
        }

        system->rootComments = newRoots;
        system->rootCapacity = newCapacity;
    }

    // 确保是主评论
    comment->parent = NULL;
    comment->depth = 0;

    // 添加到主评论数组
    system->rootComments[system->rootCount] = comment;
    system->rootCount++;

    printf("主评论添加成功！\n");
    return 1;
}
// 前序遍历：先显示当前评论，再显示子评论
void displayComment(CommentNode* node, int indent) {
    if (node == NULL) return;

    // 打印缩进（表示层级）
    for (int i = 0; i < indent; i++) {
        printf("  ");  // 每层缩进2个空格
    }

    // 显示评论内容
    printf("[%s] %s (点赞: %d, ID: %d)\n",
           node->author,
           node->content,
           node->likeCount,
           node->id);

    // 递归显示所有子评论
    for (int i = 0; i < node->childCount; i++) {
        displayComment(node->children[i], indent + 1);
    }
}
// 测试遍历
/*int main() {
    CommentNode* root = createCommentNode(1, "主评论", "用户A");
    CommentNode* reply1 = createCommentNode(2, "回复1", "用户B");
    CommentNode* reply2 = createCommentNode(3, "回复2", "用户C");
    CommentNode* reply1_1 = createCommentNode(4, "回复1的回复", "用户D");

    addReply(root, reply1);
    addReply(root, reply2);
    addReply(reply1, reply1_1);

    displayComment(root, 0);

    return 0;
}*/
// 后序遍历：先处理子节点，再处理当前节点
void postOrderTraverse(CommentNode* node, void (*visit)(CommentNode*)) {
    if (node == NULL) return;

    // 先遍历所有子节点
    for (int i = 0; i < node->childCount; i++) {
        postOrderTraverse(node->children[i], visit);
    }

    // 再处理当前节点
    visit(node);
}
// 显示所有主评论及其回复
void displayAllComments(CommentSystem* system) {
    if (system->rootCount == 0) {
        printf("暂无评论！\n");
        return;
    }

    printf("\n========== 评论列表 ==========\n");
    for (int i = 0; i < system->rootCount; i++) {
        printf("\n--- 主评论 %d ---\n", i + 1);
        displayComment(system->rootComments[i], 0);
    }
    printf("\n==============================\n\n");
}
// 深度优先搜索：根据ID查找评论
CommentNode* findCommentById(CommentNode* root, int id) {
    if (root == NULL) return NULL;

    // 如果当前节点匹配，直接返回
    if (root->id == id) {
        return root;
    }

    // 在子树中递归查找
    for (int i = 0; i < root->childCount; i++) {
        CommentNode* found = findCommentById(root->children[i], id);
        if (found != NULL) {
            return found;
        }
    }

    return NULL;
}

// 在整个系统中查找评论
CommentNode* findCommentInSystem(CommentSystem* system, int id) {
    for (int i = 0; i < system->rootCount; i++) {
        CommentNode* found = findCommentById(system->rootComments[i], id);
        if (found != NULL) {
            return found;
        }
    }
    return NULL;
}
// 查找某个作者的所有评论
void findCommentsByAuthor(CommentNode* root, char* author,
                          CommentNode** results, int* count, int maxResults) {
    if (root == NULL || *count >= maxResults) return;

    // 如果当前节点匹配，添加到结果数组
    if (strcmp(root->author, author) == 0) {
        results[*count] = root;
        (*count)++;
    }

    // 递归查找子树
    for (int i = 0; i < root->childCount; i++) {
        findCommentsByAuthor(root->children[i], author, results, count, maxResults);
    }
}
// 从父节点的子节点数组中移除指定节点
void removeFromParent(CommentNode* parent, CommentNode* child) {
    if (parent == NULL || child == NULL) return;

    // 查找子节点在数组中的位置
    int index = -1;
    for (int i = 0; i < parent->childCount; i++) {
        if (parent->children[i] == child) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("未找到子节点！\n");
        return;
    }

    // 将后面的元素前移
    for (int i = index; i < parent->childCount - 1; i++) {
        parent->children[i] = parent->children[i + 1];
    }

    parent->childCount--;
}
// 删除评论及其所有子评论（级联删除）
void deleteComment(CommentNode* node) {
    if (node == NULL) return;

    // 先删除所有子评论（后序遍历）
    for (int i = node->childCount - 1; i >= 0; i--) {
        deleteComment(node->children[i]);
    }

    // 从父节点中移除
    if (node->parent != NULL) {
        removeFromParent(node->parent, node);
    }

    // 释放子节点数组内存
    if (node->children != NULL) {
        free(node->children);
    }

    // 释放节点本身
    free(node);
}
// 从系统中删除指定ID的评论
int deleteCommentFromSystem(CommentSystem* system, int id) {
    // 查找评论
    CommentNode* target = findCommentInSystem(system, id);
    if (target == NULL) {
        printf("未找到ID为 %d 的评论！\n", id);
        return 0;
    }

    // 如果是主评论，从系统中移除
    if (target->parent == NULL) {
        // 在主评论数组中查找并移除
        for (int i = 0; i < system->rootCount; i++) {
            if (system->rootComments[i] == target) {
                // 将后面的元素前移
                for (int j = i; j < system->rootCount - 1; j++) {
                    system->rootComments[j] = system->rootComments[j + 1];
                }
                system->rootCount--;
                break;
            }
        }
    }

    // 删除评论及其所有子评论
    deleteComment(target);

    printf("评论 %d 及其所有回复已删除！\n", id);
    return 1;
}
// 统计某个评论及其所有子评论的总数
int countAllComments(CommentNode* root) {
    if (root == NULL) return 0;

    int count = 1;  // 当前节点

    // 递归统计所有子节点
    for (int i = 0; i < root->childCount; i++) {
        count += countAllComments(root->children[i]);
    }

    return count;
}

// 统计系统中的总评论数
int countTotalComments(CommentSystem* system) {
    int total = 0;
    for (int i = 0; i < system->rootCount; i++) {
        total += countAllComments(system->rootComments[i]);
    }
    return total;
}
// 计算树的最大深度
int getTreeDepth(CommentNode* root) {
    if (root == NULL) return 0;
    if (root->childCount == 0) return 1;

    int maxChildDepth = 0;
    for (int i = 0; i < root->childCount; i++) {
        int childDepth = getTreeDepth(root->children[i]);
        if (childDepth > maxChildDepth) {
            maxChildDepth = childDepth;
        }
    }

    return 1 + maxChildDepth;
}
// 给评论点赞
void likeComment(CommentNode* comment) {
    if (comment == NULL) {
        printf("评论不存在！\n");
        return;
    }

    comment->likeCount++;
    printf("评论 %d 点赞成功！当前点赞数：%d\n", comment->id, comment->likeCount);
}
// 输入评论信息
void inputCommentInfo(char* content, char* author) {
    printf("请输入评论内容：");
    fgets(content, MAX_CONTENT_LEN, stdin);
    content[strcspn(content, "\n")] = 0;  // 移除换行符

    printf("请输入作者名称：");
    fgets(author, MAX_AUTHOR_LEN, stdin);
    author[strcspn(author, "\n")] = 0;  // 移除换行符
}
// 主菜单
void showMainMenu() {
    printf("\n========== 评论系统 ==========\n");
    printf("1. 添加主评论\n");
    printf("2. 添加回复\n");
    printf("3. 显示所有评论\n");
    printf("4. 查找评论\n");
    printf("5. 删除评论\n");
    printf("6. 点赞评论\n");
    printf("7. 统计信息\n");
    printf("0. 退出程序\n");
    printf("================================\n");
    printf("请选择操作：");
}
// 查找子菜单
void showSearchMenu() {
    printf("\n========== 查找评论 ==========\n");
    printf("1. 根据ID查找\n");
    printf("2. 根据作者查找\n");
    printf("0. 返回主菜单\n");
    printf("================================\n");
    printf("请选择操作：");
}
int main() {
    // 1. 声明变量
    CommentSystem system;
    initCommentSystem(&system);

    int choice, subChoice;
    int commentId, parentId;
    char content[MAX_CONTENT_LEN];
    char author[MAX_AUTHOR_LEN];

    printf("欢迎使用评论系统！\n");

    // 2. 主循环
    while (1) {
        showMainMenu();
        scanf("%d", &choice);
        getchar(); // 清除输入缓冲区

        switch (choice) {
            case 1: // 添加主评论
                inputCommentInfo(content, author);
                {
                    CommentNode* newComment = createCommentNode(
                        system.nextId++, content, author
                    );
                    if (newComment != NULL) {
                        addRootComment(&system, newComment);
                    }
                }
                break;

            case 2: // 添加回复
                printf("请输入父评论ID：");
                scanf("%d", &parentId);
                getchar();

                CommentNode* parent = findCommentInSystem(&system, parentId);
                if (parent == NULL) {
                    printf("未找到ID为 %d 的评论！\n", parentId);
                    break;
                }

                inputCommentInfo(content, author);
                {
                    CommentNode* newReply = createCommentNode(
                        system.nextId++, content, author
                    );
                    if (newReply != NULL) {
                        addReply(parent, newReply);
                    }
                }
                break;

            case 3: // 显示所有评论
                displayAllComments(&system);
                break;

            case 4: // 查找评论
                while (1) {
                    showSearchMenu();
                    scanf("%d", &subChoice);
                    getchar();

                    switch (subChoice) {
                        case 1: // 根据ID查找
                            printf("请输入评论ID：");
                            scanf("%d", &commentId);
                            getchar();

                            CommentNode* found = findCommentInSystem(&system, commentId);
                            if (found != NULL) {
                                printf("\n找到评论：\n");
                                displayComment(found, 0);
                            } else {
                                printf("未找到ID为 %d 的评论！\n", commentId);
                            }
                            break;

                        case 2: // 根据作者查找
                            printf("请输入作者名称：");
                            fgets(author, MAX_AUTHOR_LEN, stdin);
                            author[strcspn(author, "\n")] = 0;

                            printf("\n%s 的所有评论：\n", author);
                            for (int i = 0; i < system.rootCount; i++) {
                                CommentNode* results[100];
                                int count = 0;
                                findCommentsByAuthor(
                                    system.rootComments[i],
                                    author,
                                    results,
                                    &count,
                                    100
                                );

                                for (int j = 0; j < count; j++) {
                                    displayComment(results[j], 0);
                                    printf("\n");
                                }
                            }
                            break;

                        case 0:
                            goto main_menu;
                    }
                }
                main_menu:
                break;

            case 5: // 删除评论
                printf("请输入要删除的评论ID：");
                scanf("%d", &commentId);
                getchar();
                deleteCommentFromSystem(&system, commentId);
                break;

            case 6: // 点赞评论
                printf("请输入要点赞的评论ID：");
                scanf("%d", &commentId);
                getchar();

                CommentNode* target = findCommentInSystem(&system, commentId);
                if (target != NULL) {
                    likeComment(target);
                } else {
                    printf("未找到ID为 %d 的评论！\n", commentId);
                }
                break;

            case 7: // 统计信息
                printf("\n========== 统计信息 ==========\n");
                printf("主评论数量：%d\n", system.rootCount);
                printf("总评论数量：%d\n", countTotalComments(&system));
                if (system.rootCount > 0) {
                    printf("平均每主评论回复数：%.2f\n",
                           (float)(countTotalComments(&system) - system.rootCount) /
                           system.rootCount);
                }
                printf("==============================\n\n");
                break;

            case 0: // 退出程序
                printf("感谢使用，再见！\n");
                // 释放所有内存
                for (int i = 0; i < system.rootCount; i++) {
                    deleteComment(system.rootComments[i]);
                }
                if (system.rootComments != NULL) {
                    free(system.rootComments);
                }
                return 0;

            default:
                printf("无效的选择，请重新输入！\n");
        }
    }

    return 0;
}
