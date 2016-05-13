#define Per_Page_Line 20
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "conio.h"
//文学助手
#define Max_Str_Len 255 //最大串长
typedef char  SString[Max_Str_Len + 1]; //串的定长顺序存储表示
int next[Max_Str_Len];  //KMP算法中用到的next

typedef struct text
{
    char string[80];//存储每一行的元素
    struct text *next;//指向后一个节点的指针
    struct text *pre;//指向前一个节点的指针
    int num;//每一行元素的长度
    int flat;//确定此行是否被删除的标志,删除为0，存在为1
}text;

FILE *fp,*out;//fp是执行被打开的文件，out指向被写入的文件
text *first;//链表的头指针
int sum, delnum, xiugai = 0, page = 0;
//修改用来指示文件是否被修改过，如果修改过才回存盘。page用来计算显示的
//页数.delnum用来存储被删除的字节数，sum存储一页的总字节数


void Createlist()
{//建立一个二十个节点的链表，是文本中的一页
    text *p1,*p2;
    p1 = p2 = (text *)malloc(sizeof(text));//分配空间
    first = NULL;
    int n = 0;
    while (n < Per_Page_Line) {
        n = n+1;
        if (n == 1) first = p1;
        else {
            p2->next = p1;
            p1->pre = p2;
            p1->flat = 0;
        }
        p2 = p1;
        p1 = (text *)malloc(sizeof(text));
    }
    p2->next = NULL;
}

void freemem()
{//释放链表所占的内存
    text *p;
    for (p = first; first != NULL;) {
        first = p->next;
        free(p);
        p = first;
    }
}

int Openfiles()
{//打开文件的函数
    char name[30], outname[30];
    printf("请输入要打开的文件名：");
    scanf("%s", name);
    if ((fp = fopen(name, "r+")) == NULL) {
        printf("打不开原文件！ \n");
        exit(0);
    }
    printf("请输入要写入的文件名：");
    scanf("%s", outname);
    if ((out = fopen(outname, "w+")) == NULL) {
        printf("打不开目标文件！\n");
        exit(0);
    }
    return 0;
}

int display()
{//从文件中读入到链表中，并显示出来
    getchar();
    int i;
    char conti = 'y';
    text *p;
    rewind(fp);
    int line = 1;
    printf("------------------------------\n文件内容\n\n");
    while ((!feof(fp)) && (conti == 'y' || conti == 'Y')) {
        for (i = 0, p = first, sum = 0; (i < Per_Page_Line) && (!feof(fp)); i++, p = p->next) {
            fgets(p->string, sizeof(p->string), fp);
            printf("%d %s", line, p->string);
            p->flat = 1;
            line++;
            p->num = strlen(p->string);
            sum = sum + p->num;
        }
        printf("\n继续显示?(yes/no): ");
        conti = getchar();
        printf("\n");
        getchar();//吃掉回车
        
        if (feof(fp)) {
            line = 1;
            puts("文件已经结束!");
            rewind(fp);
            return 0;
        }
    }
    rewind(fp);
    return 0;
}


int saveanddisplay(int hang)
{//命令n执行的函数，用来将活区的内容显示并读入下一页内容
    getchar();
    int i, endflat = 0;
    char conti='y';
    text *p = NULL;
    page++;
    printf("------------------------------\n");
    for (i = 0, p = first; i<hang; i++, p = p->next)  //将活区写入文件
        if (p->flat == 1) {
            fputs(p->string, out);
            p->flat = 0;
        }
    if (!feof(fp))
        printf("第%d页\n", page);
    int line = 1;
    for (i = 0, p = first, sum = 0; (i < hang) && (!feof(fp)); i++, p = p->next)//从文件读入活区
        if (fgets(p->string, sizeof(p->string), fp)) {
            printf("%d %s", line, p->string);
            p->flat = 1;
            line++;
            p->num = strlen(p->string);
            sum = sum + p->num;
        }
    if (feof(fp)) {
        printf("\n文件已经结束！\n");
        page = 0;
        line = 1;
        rewind(fp);
        return 0;
    }
    return 0;
}

int saveall()
{//退出编辑函数后执行的函数，将所有的内容存盘
    int i, endflat = 0;
    char conti = 'y';
    text *p;
    for (i = 0,p = first; i < 10; i++, p = p->next) //将活区写入文件
        if (p->flat == 1) {
            fputs(p->string, out);
            p->flat = 0;
        }
    while (!feof(fp))//将其余的内容写入文件
        fputc(fgetc(fp), out);
    return 0;
}

int delete()
{//删除d命令对应的函数，用来删min－max中的行，用结构体中的flat表示是否被删除
    text *p1, *p2, *p3;
    int min, max, i;
    xiugai = 1;
    printf("请输入要删除的起始行min和终止行max (若min = max = i, 则删除第i行)\n");
    printf("min行: ");
    scanf("%d", &min);
    printf("max行: ");
    scanf("%d", &max);
    if (first == NULL) {
        printf("\nlist null!\n");
        return 0;
    }
    p1 = p2 = p3 = first;
    
    if (min == max) {
        int k = min;
        for (i = 0; i < k - 1; i++) {
            p1 = p1->next;
        }
        p1->flat = 0;
        
    } else if (min > max) {
        printf("起始行不能大于终止行\n");
        return 0;
    } else {
        for (i = 0; i < min - 1; i++) {/*找到要删除的第一行*/
            p1 = p1->next;
        }
        for (i = 0; i < max - 1; i++) {/*找到要删除的最后一行*/
            p2 = p2->next;
        }
        for (delnum = 0; p1 != p2; p1 = p1->next) {/*删除中间的节点，将flat赋值0*/
            p1->flat = 0;
            delnum = delnum + p1->num;
        }
    }
    
    puts("删除后的活区内容：");
    for (i = 0; i < Per_Page_Line; i++) {//显示删除后的结果
        if (p3->flat == 1)
            printf("%s", p3->string);
        p3 = p3->next;

    }
        
    text *p;
    for (i = 0, p = first; i < Per_Page_Line; i++, p = p->next)  //将活区写入文件
        if (p->flat == 1) {
            fputs(p->string, out);
            p->flat = 0;
        }
    
    return 0;
}

int insert()
{//插入i命令对应的函数，在i行后插入文本
    int hang, i, increhang = 1, number = 1;
    text *p, *p1, *p2;
    xiugai = 1;
    printf("输入要插入的行号:");
    scanf("%d", &hang);
    p = (text *)malloc(sizeof(text));//为插入行分配空间
    p->flat = 1;
    if (hang == 1) {
        p->next = first;
        first = p;
    } else {
        p1 = first;
        i = 1;
        while (p1 && i < hang - 1) {
            p1 = p1->next;
            i++;
        }
        p->next = p1->next;
        p1->next = p;
    }
    printf("输入要插入的行内容: ");
    p->string[0] = getchar();
    for (i = 1; (i < 80) && (p->string[i - 1] != '.'); i++) {
        p->string[i] = getchar();
        if((i + 1 == 80) && (p->string[i] != '.')) {//如果插入的内容超过一行的容量，则分配下一行空间并将其连入链表
            p1 = p;
            p = (text *)malloc(sizeof(text));
            p->flat = 1;
            p->next = p1->next;
            p->pre = p1;
            p1->next->pre = p;
            p1->next = p;
            i = 0;
            increhang++;
        }
        p->num = i;
    }
    p->string[i - 1] = '\n';
    p->string[i] = '\0';
    puts("修改后的活区内容：");
    for (p2 = first; p2 != NULL; p2 = p2->next)//显示出修改后的链表
        printf("%s", p2->string);
    for (i = 0, p = first; i < hang; i++, p = p->next)  //将活区写入文件
        if (p->flat == 1) {
            fputs(p->string, out);
            p->flat = 0;
        }
    xiugai = 1;
    return 0;
}

int index_KMP(SString S, SString T, int pos)    //KMP算法
{//下面四个函数是文学研究助手的
    int i = pos, j = 1;
    while (i <= S[0] && j <= T[0]) {
        if (j == 0 || S[i] == T[j]) {
            ++i;
            ++j;
        } else {
            j = next[j];
        }
    }
    if (j > T[0]) {
        return (i - T[0]);
    } else
        return 0;
}

int length(SString str)     //求字符串长度
{
    int i = 1;
    while (str[i]) {
        i++;
    }
    return (i-1);
}

void find(SString keys)
{   //keys[0]是这个串的长度，keys[1]是这个串的内容
    SString text;   //存放从文件里读取来的一行字符串
    int i = 1, j = 0, q = 0, k; //i存放行号，j存放列号，k控制输出格式，q统计次数
    keys[0] = length(keys); //关键字的长度
    printf("\n%s出现在: \n", &keys[1]);     //打印关键字
    while (!feof(fp)) {
        k = 0;
        fgets(&text[1], Max_Str_Len, fp);    //从文件里读取一行，存入text串里
        text[0] = length(text); //读入的串的长度
        j = index_KMP(text, keys, j+1); //统计关键字在该行出现的位置
        if (j != 0) {
            printf("行=%d", i);
            k++;    //次数+1
        }
        while (j != 0) {    //若改行找到了关键字，再往后看还有没有
            j = index_KMP(text, keys, j+1);
            if (j != 0) {
                k++;   //次数+1
            }
        }
        i++;    //行号+1，寻找下一行
        q += k; //累加k
        
        if (k) printf("\n");    //格式控制
    }
    printf("出现了%d次\n", q);
    rewind(fp);
}

int assistant()
{//文学研究助手
    SString words[10];  //用来存储输入的关键字
    int m, n, i;
    printf("请输入要查询的字符串的个数: ");
    scanf("%d", &n);
    printf("请输入你要查询的字符串: ");
    for (i = 0; i < n; i++) {
        scanf("%s", &words[i][1]);  //[i][0]用来存字符串的长度
    }
    for (i = 0; i < n; i++) {
        find(words[i]); //对每一个关键字，都调用find函数来查找统计
    }
    getchar();
    return 0;
}

int main()
{//主函数，用来接受命令
    Openfiles();
    Createlist();
    char cmd;
    do
    {
        getchar();
        printf("------------------------------\n");
        printf("活区切换                  格式: n<回车>\n");
        printf("活区显示                  格式: p<回车>\n");
        printf("行插入                    格式: i<回车><行号><回车><文本>.<回车>\n");
        printf("行删除                    格式: d<回车><行号><回车>\n");
        printf("文学研究助手               格式: c<回车><个数><回车><内容>\n");
        printf("保存并退出                 格式: e<回车>\n");
        printf("请输入命令：");
        cmd = getchar();
        switch(cmd) {
            case 'e':
                if (xiugai == 1)
                    saveall();
                freemem();
                if(fp)
                    fclose(fp);
                if(out)
                    fclose(out);
                return 0;
            case 'p':            //活区显示
                display();
                break;
            case 'n':            //活区转换
                saveanddisplay(Per_Page_Line);
                break;
            case 'd':           //删除行
                delete();
                break;
            case 'c':
                assistant();    //文学研究助手
                break;
            case 'i':              //插入行
                insert();
                break;
        }
    } while (cmd != 'e' && cmd != 'E');
    return 0;
}
