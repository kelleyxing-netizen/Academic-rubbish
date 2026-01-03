#include <stdio.h>
#include <stdlib.h>
#include "byow.h"

int main() {
    long seed;
    printf("Welcome to BYOW (Build Your Own World)\n");
    printf("Please enter a seed (integer): ");
    scanf("%ld", &seed);

    // 1. 创建并生成世界
    World* myWorld = createWorld(seed);

    char input;
    // 2. 游戏主循环
    while (1) {
        printWorld(myWorld);

        printf("Action: ");

        scanf(" %c", &input);

        if (input == 'q') {
            break;
        }

        // 3. 处理移动
        movePlayer(myWorld, input);
    }

    // 清理内存
    free(myWorld);
    printf("Game Over.\n");
    return 0;
}
