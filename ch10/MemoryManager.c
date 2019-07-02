#include <stdio.h>
#include <stdlib.h>
#include <math.h>


struct theTLBunit
{
	int pagenum;
	int framenum;
	int age;//used for lru

};

void easyget(int va,int *page,int *offest)
{
	*page=va/256;
	*offest=va%256;
}

char physicalmemory[256][256];//内存
int virtualtoPhysical[256];//页表

int available[256];

double pfr;
double thr;

int main(int argc,char **argv)
{
	int miss=0;
	int tlbhits=0;
	int counter;
	int acounter=0;
	int i;
	int flag=0;
	int minindex;
	int minage;
	int virtualAddress[1000];
	int pageNum;
	int offset;
	FILE *open;
	FILE *back_store;
	char *file;
	struct theTLBunit TLBunit[16];
	char result;

	for(acounter=0;acounter<256;acounter++)	available[acounter]=0;
	acounter=0;
	for(counter=0;counter<16;counter++)
	{
		TLBunit[counter].age=0;
		TLBunit[counter].pagenum=-1;
	}




	file = argv[1];
	open  = fopen(file, "r");
	back_store = fopen("BACKING_STORE.bin","r");

	for(counter=0;counter<1000;counter++)
	{
		fscanf(open,"%d",&virtualAddress[counter]);

		easyget(virtualAddress[counter],&pageNum,&offset);



		if(available[pageNum])
		{
			for(i=0;i<16;i++)
			{
				if(TLBunit[i].pagenum==pageNum)
				{
					flag=1;
					break;
				}
			}

			if(flag==1)
			{
				result=physicalmemory [ TLBunit[i].framenum ][ offset ];
				printf("logical address:%d ,physical address: %d ,result is %d (TLB hit)\n",virtualAddress[counter], 							256*TLBunit[i].framenum+offset ,result);
				TLBunit[i].age=counter;
				tlbhits++;
				flag=0;
				continue;
			}

			result=physicalmemory [ virtualtoPhysical [pageNum] ][ offset ];
			printf("logical address:%d ,physical address: %d ,result is %d (found)\n",virtualAddress[counter],256*virtualtoPhysical [pageNum]+offset,result);

			//放进TLB,采取方案LRU
			minindex=0;
			minage=10000;
			for(i=0;i<16;i++)
				if(TLBunit[i].age<minage)	{minage=TLBunit[i].age; minindex=i;}
			TLBunit[minindex].pagenum=pageNum;
			TLBunit[minindex].framenum=virtualtoPhysical [pageNum];
			TLBunit[minindex].age=counter;


		}


		else
		{
			miss++;
			fseek(back_store,pageNum*256*sizeof(char), 0);
			fread ( physicalmemory[acounter], sizeof(char),256,back_store );
			virtualtoPhysical[pageNum]=acounter;
			available[pageNum]=1;

			result=physicalmemory [ virtualtoPhysical [pageNum] ][ offset ];
			printf("logical address:%d ,physical address: %d ,result is %d\n",virtualAddress[counter],256*acounter+offset,result);
			acounter++;

			//放进TLB,采取方案LRU
			minindex=0;
			minage=10000;
			for(i=0;i<16;i++)
				if(TLBunit[i].age<minage)	{minage=TLBunit[i].age; minindex=i;}
			TLBunit[minindex].pagenum=pageNum;
			TLBunit[minindex].framenum=virtualtoPhysical [pageNum];
			TLBunit[minindex].age=counter;
			//result=physicalmemory [ virtualtoPhysical[acounter-1] ][ offset ];
			//printf("%d %d \n",virtualAddress[counter],result);
		}


	}

		pfr=miss/1000.0;
		thr=tlbhits/1000.0;
		printf("page miss rate is %f while TLB hit rate is %f\n",pfr,thr);
}
