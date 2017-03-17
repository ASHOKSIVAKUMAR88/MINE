#include"DM_iotivity.h"
#include"DM_iotivity.cpp"
#include"api.h"
#include<stdio.h>
/*typedef struct
{
	char res_name[40];
	char res_id[40];
	}Resources_info;


typedef struct
{
char atr_name[30];
char atr_value[30];
}Attribute;

typedef struct
{
int NumOfElements;
Attribute atr[MAX_NUM_OF_ATR];
}c_json;
	*/

int main()
{	
	int mode_type;
	Resources_info res;
	c_json get_local;
	//int ResID, choice;
	int dup=0;
	char st[40];
	printf("Application starting\n");
	printf("please enter mode_type as -> 0 for Server,1 for Client, 2 for Both,3 for Gateway(routing capabilities)\n");
	printf("mode type is:");
	scanf("%d", &mode_type);

	if(dm_init(mode_type)!=0)
	{
		printf("failed to initialize\n");
		exit(1);
	}
	json_object *jobj_local = json_object_new_object();
	
	jobj_local = dm_discovery(dm_ondiscovery);
	
	while(1)
	{
	printf("Enter URI name: ");
	scanf("%s",st);
	json_object_object_foreach(jobj_local,key1,val1)
	{
		if(strcmp(st,key1)==0)
		{
			dup =1;
			strcpy(res.res_name,key1);
			strcpy(res.res_id,json_object_get_string(val1));
			break;
		}
		else
			dup =0;
		//printf("new:%s\nvalue:%s\n",key,json_object_get_string(val));
		}
	if(dup == 1)
	get_local=DM_Get(res,DM_onGet);
	
	else
	{
	printf("no match found\n");
	continue;
		}
/*for(int j=0;j<get_local.NumOfElements;j++)
	{
		printf("%s : %s\n",get_local.atr[j].atr_name,get_local.atr[j].atr_value);
	}*/
	printf("Enter URI name to put: ");
	scanf("%s",st);
	json_object_object_foreach(jobj_local,key2,val2)
	{
		if(strcmp(st,key2)==0)
		{
			dup =1;
			strcpy(res.res_name,key2);
			strcpy(res.res_id,json_object_get_string(val2));
			for(int j=0;j<get_local.NumOfElements;j++)
			{
				printf("%s:",get_local.atr[j].atr_name);
				scanf("%s",get_local.atr[j].atr_value);
			}
			break;
		}
		else
			dup =0;
		//printf("new:%s\nvalue:%s\n",key,json_object_get_string(val));
		}

	if(dup == 1)
	{
	for(int j=0;j<get_local.NumOfElements;j++)
	{
		printf("%s : %s\n",get_local.atr[j].atr_name,get_local.atr[j].atr_value);
	}
	DM_Put(res,get_local,DM_onPut);
	}
	else
	{
	printf("no match found\n");
	continue;
		}
			}//while
	
	
	while(1);
	return 0;
	
}


