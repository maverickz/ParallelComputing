/**
 * Exercise a limited-depth-tree parallel merge sort under GA.
 * Author:  Karthik Raj
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <ga.h>
#include <mpi.h>
#include "macdecls.h"

#define n (1000000)
int a[n],b[n];

void Merge(a,b,lo,mid,hi)
int a[],b[], lo,mid,hi;
{
  int h,i,j,k;
  h = lo;
  i = lo;
  j = mid+1;
  while ((h<=mid) && (j<=hi))
  { 
  if (a[h]<=a[j]) 
	b[i++] = a[h++]; 
  else 
	b[i++] = a[j++]; 
  }
  if (h>mid)
    {  
		for(k=j;k<=hi;k++) 
			b[i++] = a[k]; 
	}
  else
    {  
		for(k=h;k<=mid;k++) 
			b[i++] = a[k]; 
	 }
  for(k=lo;k<=hi;k++) 
		a[k] = b[k];
}

void Merge_Sort(a,b, lo, hi)
int a[],b[], lo,hi;
{  int temp,mid;
   if (lo < hi)
   { 
     if (hi == lo+1)
     { 
		 if (a[hi]<a[lo]) 
		 {
			temp=a[hi];
			a[hi]=a[lo];
			a[lo]=temp;
		 }
	 }
	 else
	 { 
	   mid = (lo+hi)/2;
	   Merge_Sort(a,b,lo,mid);
	   Merge_Sort(a,b,mid+1,hi);
	   Merge(a,b,lo,mid,hi);
	 }
   }
}

void Test_Sorted(a,lo,hi)
int a[],lo,hi;
{ 
  int i, notsorted;
  notsorted = 0;
  for(i=lo+1;i<=hi;i++)
   if(a[i-1]>a[i]) 
		notsorted++;
  if (notsorted>0) 
		printf("sequence is not sorted in increasing order; # flips =%d\n",notsorted);
  else 
	    printf("sequence is sorted\n");
}

void Dump(a,lo,hi)
int a[],lo,hi;
{ 
  int i;
  for(i=lo;i<=hi;i++) 
		printf("%d ",a[i]);
  printf("\n");
}

int main()
{
	double rtclock(void);
	double clkbegin, clkend;
	double t;
	int i,j,k;
	
	int g_a,g_b;
	int ndim = 1;
	int dims[] = {n};
	int chunk[] = {-1};
	int ld[]={n};
	int lo[1], high[1];
	int low[1],hi[1];
	int numProc, procId;
	
	printf("List Size = %d\n",n);
	for(i=0;i<n;i++)  
		a[i] = n-i;
	printf("Initial "); 
	Test_Sorted(a,0,n-1); 
	clkbegin = rtclock();
	Merge_Sort(a,b, 0, n-1);
	clkend = rtclock();
	printf("After sequential sort, "); 
	Test_Sorted(a,0,n-1); 
	t = clkend-clkbegin;
	printf("Sequential Sort Rate: %.1f Mega-Elements/Second; Time = %.3f sec; a[n/2] = %d; \n\n",	1.0*n/t/1000000,t,a[n/2]);
	/*Initialize the array to be sorted*/
	for(i=0;i<n;i++)  
		a[i] = n-i;
		
	MP_INIT(argc,argv);
	GA_Initialize ();

	numProc = GA_Nnodes();/*Get number of processes*/
	procId = GA_Nodeid();/*Get process id*/
	g_a = NGA_Create(C_INT, ndim, dims, "Array A", chunk);/*Create a global array of size 'n' enforcing an 'equal' partioning of elements among the processes*/
	NGA_Distribution(g_a,procId, lo, hi);/*Find the range of the global array that is held by a process*/ 
	
	low[0] = 0;
	high[0] = n-1;
   /*Initializing the global array*/
    if(procId == 0)
		NGA_Put(g_a, low, high, a, ld);
	GA_Sync();
	GA_Print(g_a);
	
	
}
double rtclock(void)
{
	struct timezone Tzp;
	struct timeval Tp;
	int stat;
	stat = gettimeofday (&Tp, &Tzp);
	if (stat != 0) 
		printf("Error return from gettimeofday: %d",stat);
	return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}
