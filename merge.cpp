#include <mpi.h>
#include <stdio.h>
#include<bits/stdc++.h>

using namespace std;

void merge(int arr[], int l, int m, int r)
{
	int i,j,k,n1,n2;
	n1=m-l+1;
	n2=r-m;

	int left[n1], right[n2];

	for (i=0;i<n1;i++)
		left[i]=arr[l+i];
	for (j =0;j<n2;j++)
		right[j] =arr[m+1+j];

	i=0;
	j=0;
	k=l;
	int jj;
	for (jj=0;jj<n1+n2;jj++)
	{
		if (i<n1&&j<n2)
		{
			if (left[i] <= right[j])
			{
				arr[k] = left[i];
				i++;
			}
			else
			{
				arr[k] = right[j];
				j++;
			}
			k++;
		}
		else
			break;
	}
	while (i < n1)
	{
		arr[k] = left[i];
		i++;
		k++;
	}
	while (j < n2)
	{
		arr[k] = right[j];
		j++;
		k++;
	}
}


void msort(int a[], int l, int r)
{
	if (l<r)
	{
		int m = l+(r-l)/2;
		msort(a, l, m);
		msort(a, m+1, r);
		merge(a, l, m, r);
	}
}

int main(int argc, char** argv) {
	int i,j;
	int n;
	cin >> n;
	int arr[n];
	for(i=0; i< n ; i++)
		cin >> arr[i];


	MPI_Init(NULL, NULL);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);

	int mn=n/(world_size-1);
	int mod=n%(world_size-1);
	int subarray[mn];//=(int *)malloc(mn * sizeof(int));;
	int subarray2[mn+1];//=(int *)malloc((mn+1)* sizeof(int));;
	if (world_rank==0)
	{

		int val=0;
		for (i=1;i<world_size;i++)
		{
			MPI_Send(&n, 1, MPI_INT,i, 0, MPI_COMM_WORLD);
			if (i>mod)
			{
				for (j=val;j<val+mn;j++)
					subarray[j-val]=arr[j];
				val=val+mn;
				MPI_Send(subarray, mn, MPI_INT,i, 0, MPI_COMM_WORLD);
			}
			else
			{
				for(j=val;j<val+mn+1;j++)
					subarray2[j-val]=arr[j];
				val=val+mn+1;
				MPI_Send(subarray2, mn+1, MPI_INT,i, 0, MPI_COMM_WORLD);
			}
		}
		int arr2[(mn+1)*mod];
		int arr1[mn*(world_size-1-mod)];
		int len1=0;
		int len2=0;
		int sor1[mn*(world_size-1-mod)];
		int sor2[(mn+1)*mod];
		int sor[n];
		int temp[n];
		int p1=0,p2=0;
		for (i=1;i<world_size;i++)
		{
			p1=0;
			p2=0;
			if (i>mod)
			{
				MPI_Recv(subarray, mn, MPI_INT, i, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				for (j=0;j<mn+len1;j++)
				{
					if (p1<len1&& p2<mn)
					{
						if(sor1[p1]<subarray[p2])
						{
							temp[p1+p2]=sor1[p1];
							p1++;
						}
						else
						{
							temp[p1+p2] = subarray[p2];
							p2++;
						}
					}
					else
						break;
				}
				if (p1==len1)
				{
					for(j=p2;j<mn;j++)
					{
						temp[p1+p2]=subarray[p2];
						p2++;
					}
				}
				else if (p2==mn)
				{
					for (j=p1;j<len1;j++)
					{
						temp[p1+p2]=sor1[p1];
						p1++;
					}
				}

				len1+=mn;
				for(j=0;j<len1;j++)
				{
					sor1[j]=temp[j];
				}

			}
			else
			{
				MPI_Recv(subarray2, mn+1, MPI_INT, i, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				for (j=0;j<len2+mn+1;j++)
				{
					if (p1<len2&& p2<mn+1)
					{
						if(sor2[p1]<subarray2[p2])
						{
							temp[p1+p2]=sor2[p1];
							p1++;
						}
						else
						{
							temp[p1+p2] = subarray2[p2];
							p2++;
						}
					}
					else
						break;
				}
				if (p1==len2)
				{
					for(j=p2;j<mn+1;j++)
					{
						temp[p1+p2]=subarray2[p2];
						p2++;
					}
				}
				else if (p2==mn+1)
				{
					for (j=p1;j<len2;j++)
					{
						temp[p1+p2]=sor2[p1];
						p1++;
					}
				}

				len2=len2+mn+1;
				for(j=0;j<len2;j++)
				{
					sor2[j]=temp[j];
				}

			}

		}
		p1=0,p2=0;
		for(j=0;j<len1+len2;j++)
		{
			if (p1<len1&& p2<len2)
			{
				if(sor1[p1]<sor2[p2])
				{
					temp[p1+p2]=sor1[p1];
					p1++;
				}
				else
				{
					temp[p1+p2] = sor2[p2];
					p2++;
				}
			}
			else
				break;
			if (p1==len1)
			{
				for(j=p2;j<len2;j++)
				{
					temp[p1+p2]=sor2[p2];
					p2++;
				}
			}
			else if (p2==len2)
			{
				for (j=p1;j<len1;j++)
				{
					temp[p1+p2]=sor1[p1];
					p1++;
				}
			}

		}

		for(j=0;j<n;j++)
			cout << temp[j] << endl;


	}
	else
	{
		MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		mn=n/(world_size-1);
		mod=n%(world_size-1);

		int kkk;
		if (world_rank>mod)
		{

			MPI_Recv(subarray, mn, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			msort(subarray, 0, mn-1);
			MPI_Send(subarray, mn, MPI_INT,0, 0, MPI_COMM_WORLD);
		}
		else
		{
			MPI_Recv(subarray2, mn+1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			msort(subarray2, 0, mn);
			MPI_Send(subarray2, mn+1, MPI_INT,0, 0, MPI_COMM_WORLD);
		}
	}
	MPI_Finalize();
	return 0;
}
