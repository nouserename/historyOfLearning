/*
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
����1����ʼ���߳����͵ı���
����2��ͨ����NULL����ʾʹ���߳�Ĭ�����ԡ�
����3������ָ��,�ú������н��������߳̽�����
����4�������Ĳ���
*/


/*
pthread_join,�����ȴ��߳��˳�����ȡ�߳��˳�״̬,��Ӧ������ waitpid() ����
int pthread_join(pthread_t thread, void **retval);
����
�ɹ���0��ʧ�ܣ������
������
thread���߳�ID ������ָ�룩��
retval�������ú����ķ���ֵ


��ôpthread_join������ʲô���أ�����
    pthread_joinʹһ���̵߳ȴ���һ���߳̽�����
    ���������û��pthread_join���̻߳�ܿ�����Ӷ�ʹ�������̽������Ӷ�ʹ
�������߳�û�л��Ὺʼִ�оͽ����ˡ�����pthread_join�����̻߳�һֱ
�ȴ�ֱ���ȴ����߳̽����Լ��Ž�����ʹ�������߳��л���ִ��
*/

/*
fgets() ��ԭ��Ϊ��
# include <stdio.h>
char *fgets(char *s, int size, FILE *stream);

    fgets() ��Ȼ�� gets() ��ȫ������ȫ��Ҫ�������۵ģ����۾�����
��ʹ�ñ� gets() Ҫ�鷳һ�㣬���������������Ĺ����Ǵ� stream 
���ж�ȡ size ���ַ��洢���ַ�ָ����� s ��ָ����ڴ�ռ䡣
���ķ���ֵ��һ��ָ�룬ָ���ַ����е�һ���ַ��ĵ�ַ��
*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>

int array_length, file_length;
int *array_master;
FILE *freader;

//��������
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

    /* �����߳�ִ�й鲢����,�����µ��̵߳��ù鲢�����㷨 merge_sort��*/
    pthread_t tid;
    pthread_create(&tid, NULL, merge_sort, arg);

    /*����ͬ��������������Ϊ�˴�ӡ������õ����ݣ�����ȴ������߳�ִ����ϣ�
    ���ܴ�ӡ��������õ����飬������Ҫ�߳�ͬ����*/
    pthread_join(tid, NULL);


    //���ȱ����һ�ι鲢����
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

//���ļ�������
int read_length(char *fname)
{
    freader = fopen(fname, "rt");
    char line[80];
    int file_length = 0;

    /* fgets���ļ��ж����ݣ�ÿ��һ�е��ַ������Ϊ80���ַ�����
    �����ļ�ĩβ EOF������NULL */
    while(fgets(line, 80, freader) != NULL)
        file_length += 1;
    
    return file_length;
}

// ���ڴ��ļ���ȡ����
int *read_file(char *fname)
{
    freader = fopen(fname, "rt");   //ֻ����ʽ���ļ�
    int bufsize = file_length;
    char line[80];
    int integer;
    int index = 0;
    int *input = (int *)malloc(bufsize*sizeof(int));

    while(fgets(line, 80, freader) != NULL)
    {
        //���ַ��� line �л������������ַ�����������ת����
        sscanf(line, "%d", &integer);
        input[index] = integer;
        ++index;
        ++array_length;
    }

    fclose(freader); //�ر��ļ�

    return input;
}

void merge(int arr[], int left, int middle, int right)
{
    int i, j, k;
    int half1 = middle - left + 1;
    int half2 = right - middle;

    int first[half1], second[half2];    /* ����������ʱ���飬����ǰ�벿�����ݺͺ�벿������ 
    ���鲢�����ʱ��Ӧ���������������飬������ԭַ�Ͽ��������ݵķ�ʽ���£�
     �� arr ���鸴�� left �� right ֮��ǰ�벿�ֵ����� */

    for(i = 0; i < half1; i++)
        first[i] = arr[left + i];

    for(j = 0; j < half2; j++)
        second[j] = arr[middle + 1 + j];

    i = 0, j = 0, k = left;

    //�Ƚ�������ʱ�����е������ҳ���ǰ��Сֵ��Ȼ�������arr
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

        k++;    //arr���������
    }

 /* ����ʱ������ʣ��������� arr ���� 
    ��ʱ�п�������ĳ�����������������û����ȫ����ԭַ�У����Բ����±߷�ʽ��ʣ������������飺
    ���������while ѭ�����ֻ��һ����*/
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
    //��������
    int *arr = array_master;    //ָ��ȫ�ֱ��� array_master ����
    int *argu = (int *)arg;
    int l = argu[0];    //���̴߳���Ĳ��������Ҫ�������ݵ���С����ֵ
    int r = argu[1];    //���̴߳���Ĳ��������Ҫ�������ݵ��������ֵ

    //�� l==r �򲻱�����
    if(l < r)
    {
        //���������߳�������
        pthread_t tid1;
        pthread_t tid2;

        //���������̴߳������Ĳ���
        int arg1[2];
        int arg2[2];

        int middle;
        middle = (l + (r - 1)) / 2;
        arg1[0] = l;
        arg1[1] = middle;
        arg2[0] = middle + 1;
        arg2[1] = r;

        /* �����ö��ַ�������ֳ������ֱַ��������Դ��ڲ��еĿ���
         ������ö��߳�*/
        pthread_create(&tid1, NULL, merge_sort, arg1);
        pthread_create(&tid2, NULL, merge_sort, arg2);

        /* �������ȴ����������鶼��������ϣ����ܽ��й鲢��
        ����������� pthread_join ʹ���߳�ͬ�� */
        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);

        //��ʱ�鲢����������������
        merge(arr, l, middle, r);
        pthread_exit(0);    //�˳��߳�
    }

    return NULL;
}


    
    