#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void odd_even_sort(int a[], int n);
void genlist(int a[], int n, int id);
void printARR(int *a, int n, int id);
int Compute_partner(int phase, int myid);

int main(int argc, char const *argv[])
{
    int myid, processN, i, n, local_n, partner, phase, merge_flag1, merge_flag2, j;
    int *local_list, *partner_list, *temp_list, *temp, *gather_list;
    double startwtime = 0.0;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &processN);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    startwtime = MPI_Wtime();
    MPI_Status status;

    n = atoi(argv[1]);
    local_n = n / processN;
    local_list = malloc(sizeof(n) * local_n);
    partner_list = malloc(sizeof(n) * local_n);
    temp_list = malloc(sizeof(n) * local_n);
    gather_list = malloc(sizeof(n) * n);

    genlist(local_list, local_n, myid);
    odd_even_sort(local_list, local_n);

    MPI_Gather(local_list, local_n, MPI_INTEGER, gather_list, local_n, MPI_INTEGER, 0, MPI_COMM_WORLD);

    if (myid == 0)
        printARR(gather_list, n, -1);

    for (phase = 0; phase < processN; phase++)
    {
        partner = Compute_partner(phase, myid);
        if (partner >= processN)
            partner = -1;

        if (partner >= 0)
        {
            MPI_Send(local_list, local_n, MPI_INT, partner, 0, MPI_COMM_WORLD);
            MPI_Recv(partner_list, local_n, MPI_INT, partner, 0, MPI_COMM_WORLD, &status);

            if (myid < partner)
            {
                merge_flag1 = 0;
                merge_flag2 = 0;
                for (i = 0; i < local_n; i++)
                {
                    if (local_list[merge_flag1] < partner_list[merge_flag2])
                    {
                        temp_list[i] = local_list[merge_flag1];
                        merge_flag1++;
                    }
                    else
                    {
                        temp_list[i] = partner_list[merge_flag2];
                        merge_flag2++;
                    }
                }
            }
            else
            {
                merge_flag1 = local_n - 1;
                merge_flag2 = local_n - 1;
                for (i = local_n - 1; i >= 0; i--)
                {
                    if (local_list[merge_flag1] > partner_list[merge_flag2])
                    {
                        temp_list[i] = local_list[merge_flag1];
                        merge_flag1--;
                    }
                    else
                    {
                        temp_list[i] = partner_list[merge_flag2];
                        merge_flag2--;
                    }
                }
            }

            temp = local_list;
            local_list = temp_list;
            temp_list = temp;
        }
    }

    MPI_Gather(local_list, local_n, MPI_INTEGER, gather_list, local_n, MPI_INTEGER, 0, MPI_COMM_WORLD);

    if (myid == 0)
    {
        printARR(gather_list, n, -1);
        printf("The execution time = %lf\n", MPI_Wtime() - startwtime);
    }
    MPI_Finalize();
    return 0;
}

void odd_even_sort(int a[], int n)
{
    int phase, i, temp;
    int sorted = 0;

    for (phase = 0; phase < n; phase++)
    {
        if (phase % 2 == 0)
        {
            for (i = 1; i < n; i += 2)
                if (a[i - 1] > a[i])
                {
                    temp = a[i];
                    a[i] = a[i - 1];
                    a[i - 1] = temp;
                }
        }
        else
        {
            for (i = 1; i < n - 1; i += 2)
                if (a[i + 1] < a[i])
                {
                    temp = a[i];
                    a[i] = a[i + 1];
                    a[i + 1] = temp;
                }
        }
    }
}

void genlist(int a[], int n, int id)
{
    int i;
    srand(time(NULL) + id * id);

    for (i = 0; i < n; i++)
        a[i] = rand();
}

void printARR(int *a, int n, int id)
{
    if (id >= 0)
        printf("%d : ", id);
    int i;

    for (i = 0; i < n; i++)
        printf("%d ", a[i]);
    printf("\n");
}

int Compute_partner(int phase, int myid)
{
    if (phase % 2 == 0)
    {
        if (myid % 2 == 0)
            return myid + 1;
        else
            return myid - 1;
    }
    else
    {
        if (myid % 2 == 0)
            return myid - 1;
        else
            return myid + 1;
    }
}