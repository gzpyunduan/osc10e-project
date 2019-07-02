#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct memory
{
	int start;
	int end;
	int state;
	char name[5];
	struct memory *next;

};


struct memory *head;

int totalsize;

void allocate(int path,int size,char *name);
void divide(struct memory *pointer,int size,char *name);
void release(char *name);
void show();
void merge();
void quietus();

int main(int argc,char **argv)
{
	int flag=1;
	char command[5];
	char name[3];
	int size;
	char path[2];

	head=(struct memory*)malloc(sizeof(struct memory));

	totalsize=atoi(argv[1]);
	head->start=0;
	head->end=totalsize-1;
	head->state=0;
	head->next=NULL;
	while(flag==1)
	{
		printf("allocator>");
		scanf("%s",command);
		if(strcmp(command,"RQ")==0)
		{
			printf("RQ\n");
			scanf("%s",name);
			scanf("%d",&size);
			scanf("%s",path);
			printf("%s %d %s\n",name,size,path);
			if(strcmp(path,"F")==0)	allocate(1,size,name);
			if(strcmp(path,"B")==0)	allocate(2,size,name);
			if(strcmp(path,"W")==0)	allocate(3,size,name);
		}
		if(strcmp(command,"RL")==0)
		{
			scanf("%s",name);
			release(name);
			//printf("rl : %d",request[0]);
		}
		if(strcmp(command,"C")==0)
		{
			merge();
		}
		if(strcmp(command,"STAT")==0)	show();
		if(strcmp(command,"Q")==0)	flag=0;
	}


	quietus();

}

void allocate(int path,int size,char *name)	//path 1:first fit
{						//path 2:best fit
						//path 3:worst fit
	struct memory *pointer;
	struct memory *bestpointer;
	struct memory *worstpointer;
	int bestsize;
	int worstsize;
	worstpointer=NULL;
	bestpointer=NULL;
	pointer=head;

	switch(path)
	{
		case 1: while(pointer!=NULL)
			{
				if(pointer->state==0&&(pointer->end-pointer->start+1>=size))
					break;
				else
					pointer=pointer->next;
			}
			if(pointer!=NULL)
				divide(pointer,size,name);
			else
				printf("No place for you!%s!\n",name);
			break;

		case 2:
			bestsize=totalsize;
			while(pointer!=NULL)
			{
				if(pointer->state==0&&(pointer->end-pointer->start+1>=size)&&(pointer->end-pointer->start+1<=bestsize))
				{
					bestpointer=pointer;
					bestsize=pointer->end-pointer->start+1;
					pointer=pointer->next;
				}
				else
					pointer=pointer->next;
			}
			if(bestpointer!=NULL)
			{
				divide(bestpointer,size,name);
				bestpointer=NULL;
			}

			else
				printf("No place for you!%s!\n",name);
			break;

		case 3:
			worstsize=0;
			while(pointer!=NULL)
			{
				if(pointer->state==0&&(pointer->end-pointer->start+1>=size)&&(pointer->end-pointer->start+1>=worstsize))
				{
					worstpointer=pointer;
					worstsize=pointer->end-pointer->start+1;
					pointer=pointer->next;
				}
				else
					pointer=pointer->next;
			}
			if(worstpointer!=NULL)
			{
				divide(worstpointer,size,name);
				worstpointer=NULL;
			}
			else
				printf("No place for you!%s!\n",name);
			break;

	}

}

void divide(struct memory *pointer,int size,char *name)
{
	struct memory *new;
	new=(struct memory*)malloc(sizeof(struct memory));

	new->end=pointer->end;
	pointer->end=pointer->start+size-1;
	new->start=pointer->end+1;
	new->next=pointer->next;
	new->state=0;
	if(new->start>=new->end)
	{
		free(new);
		new=NULL;
	}
	pointer->next=new;
	pointer->state=1;
	strcpy(pointer->name,name);
}
void release(char *name)
{
	int found=0;
	struct memory *pointer;
	struct memory *pointern;//need to judge forward and backword
	struct memory *tmp;
	pointer=head;
	if(strcmp(pointer->name,name)==0)
	{
		if(pointer->next!=NULL)
		{
			if(pointer->next->state==0)
			{
				pointern=pointer->next;
				pointer->end=pointern->end;
				pointer->state=0;
				pointer->next=pointern->next;
				free(pointern);
			}
			else
			{
				pointer->state=0;
			}
		}
		else
		{
			pointer->state=0;
		}
		return;
	}
	for(pointer=head;pointer->next!=NULL;pointer=pointer->next)
	{
		if(strcmp(pointer->next->name,name)==0)
		{
			pointern=pointer->next;
			found=1;
			break;
		}
		pointer=pointer->next;
	}
	if(found!=1)
	{
		printf("Process not found!\n");
		return;
	}
	if(pointern->next!=NULL)
	{
		if(pointer->state==0&&pointern->next->state==0)
		{
			tmp=pointern->next;
			pointer->end=pointern->next->end;
			pointer->next=pointern->next->next;
			free(tmp);
			free(pointern);
		}
		else if(pointer->state!=0&&pointern->next->state==0)
		{
			pointern->next->start=pointern->start;
			pointer->next=pointern->next;
			free(pointern);
		}
		else if(pointer->state==0&&pointern->next->state!=0)
		{
			pointer->end=pointern->end;
			pointer->next=pointern->next;
			free(pointern);
		}
		else	pointern->state=0;
	}
	else
	{
		if(pointer->state==0)
		{
			pointer->end=pointern->end;
			free(pointern);
			pointer->next=NULL;
		}
		else
			pointern->state=0;
	}


}
void merge()
{

	struct memory *pointer;
	struct memory *past;
	struct memory *tmp;
	int sizepointer;
	pointer=head;
	past=pointer;
	pointer=pointer->next;

	while(pointer!=NULL)
	{

		if(past->state==1)//full
		{
			past=past->next;
			pointer=pointer->next;
		}
		else
		{
			if(pointer->state==0)
			{
				past->end=pointer->end;
				past->next=pointer->next;
				tmp=pointer;
				pointer=pointer->next;
				free(tmp);
			}
			else
			{
				//turn
				sizepointer=pointer->end-pointer->start;
				past->end=past->start+sizepointer;
				pointer->start=past->end+1;
				pointer->state=0;
				past->state=1;
				strcpy(past->name,pointer->name);

				past=past->next;
				pointer=pointer->next;
			}
		}


	}

}
void show()
{
	struct memory *pointer;
	pointer=head;
	while(pointer!=NULL)
	{
		if(pointer->state!=0)
			printf("Address [%d,%d] Process %s\n",pointer->start,pointer->end,pointer->name);
		else
			printf("Address [%d,%d] Unused\n",pointer->start,pointer->end);
		pointer=pointer->next;
	}
}
void quietus()
{
	struct memory *pointer1=head;
	struct memory *pointer2;
	while(pointer1)
	{
		pointer2=pointer1->next;
		free(pointer1);
		pointer1=pointer2;
	}

}
