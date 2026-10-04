#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 递归查找：在 path 中查找名字为 name 的文件
void find(char *path, char *name){

    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 打开当前路径
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取文件状态
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {

    // 当前路径是普通文件
    case T_FILE:

        // 找到 path 最后一个 '/' 后面的文件名
        p = path + strlen(path);

        while (p >= path && *p != '/')
            p--;

        p++;

        // 比较文件名
        if (strcmp(p, name) == 0)
            printf("%s\n", path);

        break;

    // 当前路径是目录
    case T_DIR:
        
        p = path + strlen(path);

        while (p >= path && *p != '/')
            p--;
            
        p++;

        // 比较目录名  
        if (strcmp(p, name) == 0)
            printf("%s\n", path);

        // 检查路径是否可能超过 buf 大小
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            fprintf(2, "find: path too long\n");
            break;
        }

        // 先复制当前路径
        strcpy(buf, path);

        // p 指向末尾，准备添加 '/'
        p = buf + strlen(buf);
        *p++ = '/';

        // 读取目录中的每一个目录项
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {

            // 无效目录项
            if (de.inum == 0)
                continue;

            // 跳过当前目录和父目录
            if (strcmp(de.name, ".") == 0 ||
                strcmp(de.name, "..") == 0)
                continue;

            // 构造新的路径：path/name
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = '\0';

            // 递归搜索
            find(buf, name);
        }

        break;
    }

    close(fd);
}


int main(int argc, char *argv[]){
    
    if (argc != 3) {
        fprintf(2, "usage: find <path> <name>\n");
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}