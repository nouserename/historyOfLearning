/*
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
参数1：初始化线程类型的变量
参数2：通常传NULL，表示使用线程默认属性。
参数3：函数指针,该函数运行结束，则线程结束。
参数4：函数的参数
*/


/*
pthread_join,阻塞等待线程退出，获取线程退出状态,对应进程中 waitpid() 函数
int pthread_join(pthread_t thread, void **retval);
返回
成功：0；失败：错误号
参数：
thread：线程ID （不是指针）；
retval：所调用函数的返回值


那么pthread_join函数有什么用呢？？？
    pthread_join使一个线程等待另一个线程结束。
    代码中如果没有pthread_join主线程会很快结束从而使整个进程结束，从而使
创建的线程没有机会开始执行就结束了。加入pthread_join后，主线程会一直
等待直到等待的线程结束自己才结束，使创建的线程有机会执行
*/

/*
fgets() 的原型为：
# include <stdio.h>
char *fgets(char *s, int size, FILE *stream);

    fgets() 虽然比 gets() 安全，但安全是要付出代价的，代价就是它
的使用比 gets() 要麻烦一点，有三个参数。它的功能是从 stream 
流中读取 size 个字符存储到字符指针变量 s 所指向的内存空间。
它的返回值是一个指针，指向字符串中第一个字符的地址。
*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>

int array_length, file_length;
int *array_master;
FILE *freader;

//声明函数
int read_length(char *fname);
int *read_file(char *fname);
void merge(int arr[], int left, int middle, int right);
void *merge_sort(void *arg);



int main(int argc, char *argv[])
{
    char *fname = argv[1];

    file_length = read_length(fname);

    array_master = read_file(fname);

    int arg[2];
    arg[0] = 0;
    arg[1] = file_length - 1;

    /* 创建线程执行归并排序,创建新的线程调用归并排序算法 merge_sort。*/
    pthread_t tid;
    pthread_create(&tid, NULL, merge_sort, arg);

    /*进程同步，在主程序中为了打印已排序好的数据，必须等待排序线程执行完毕，
    才能打印到已排序好的数组，所以需要线程同步。*/
    pthread_join(tid, NULL);


    //最后缺少了一次归并排序
    int middle = file_length / 2;
    merge(array_master, 0, middle, file_length - 1);


    int j;
    for(j = 0; j < array_length; j++)
    {
        if(j == array_length - 1)
            printf("%d\n", array_master[j]);
        else
            printf("%d ", array_master[j]);
    }

    return 0;
}

//求文件的行数
int read_length(char *fname)
{
    freader = fopen(fname, "rt");
    char line[80];
    int file_length = 0;

    /* fgets从文件中读数据，每读一行的字符串（最长为80个字符），
    读到文件末尾 EOF，返回NULL */
    while(fgets(line, 80, freader) != NULL)
        file_length += 1;
    
    return file_length;
}

// 用于从文件读取数据
int *read_file(char *fname)
{
    freader = fopen(fname, "rt");   //只读方式打开文件
    int bufsize = file_length;
    char line[80];
    int integer;
    int index = 0;
    int *input = (int *)malloc(bufsize*sizeof(int));

    while(fgets(line, 80, freader) != NULL)
    {
        //从字符串 line 中获得整数（完成字符串到整数的转换）
        sscanf(line, "%d", &integer);
        input[index] = integer;
        ++index;
        ++array_length;
    }

    fclose(freader); //关闭文件

    return input;
}

void merge(int arr[], int left, int middle, int right)
{
    int i, j, k;
    int half1 = middle - left + 1;
    int half2 = right - middle;

    int first[half1], second[half2];    /* 声明两个临时数组，保存前半部分数据和后半部分数据 
    当归并排序的时候应该有两个排序数组，用来从原址上拷贝的数据的方式如下：
     从 arr 数组复制 left 到 right 之间前半部分的数据 */

    for(i = 0; i < half1; i++)
        first[i] = arr[left + i];

    for(j = 0; j < half2; j++)
        second[j] = arr[middle + 1 + j];

    i = 0, j = 0, k = left;

    //比较两个临时数组中的数，找出当前最小值，然后按序存入arr
    while(i < half1 && j < half2)
    {
        if(first[i] <= second[j])
        {
            arr[k] = first[i];
            ++i;
        }
        else
        {
            arr[k] = second[j];
            j++;
        }

        k++;    //arr数组的索引
    }

 /* 将临时数组中剩余的数存入 arr 数组 
    此时有可能其中某个有序子数组的数还没有完全插入原址中，所以采用下边方式将剩余的数插入数组：
    下面的两个while 循环最多只会一个。*/
    while (i < half1)
    {
        arr[k] = first[i];
        i++;
        k++;
    }

    while (j < half2)
    {
        arr[k] = second[j];
        j++;
        k++;
    }
}


void *merge_sort(void *arg)
{
    //变量声明
    int *arr = array_master;    //指向全局变量 array_master 数组
    int *argu = (int *)arg;
    int l = argu[0];    //由线程传入的参数，获得要排序数据的最小索引值
    int r = argu[1];    //有线程传入的参数，获得要排序数据的最大索引值

    //若 l==r 则不必排序
    if(l < r)
    {
        //声明两个线程描述符
        pthread_t tid1;
        pthread_t tid2;

        //声明调用线程处理函数的参数
        int arg1[2];
        int arg2[2];

        int middle;
        middle = (l + (r - 1)) / 2;
        arg1[0] = l;
        arg1[1] = middle;
        arg2[0] = middle + 1;
        arg2[1] = r;

        /* 由于用二分法对数组分成两部分分别排序，所以存在并行的可能
         这里采用多线程*/
        pthread_create(&tid1, NULL, merge_sort, arg1);
        pthread_create(&tid2, NULL, merge_sort, arg2);

        /* 这里必须等待两部分数组都已排序完毕，才能进行归并，
        所以这里调用 pthread_join 使得线程同步 */
        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);

        //此时归并两个已排序子序列
        merge(arr, l, middle, r);
        pthread_exit(0);    //退出线程
    }

    return NULL;
}


    
    