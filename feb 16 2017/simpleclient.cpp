//#include"DM_iotivity.h"
//#include"DM_iotivity.cpp"
//#include"api.h"
#include"../../Source/IoTvity_Wrapper/DM_iotivity.h"
#include"new_app.cpp"
#include<stdio.h>
int main()
{	
	int mode_type;
	Resource_info* res_local;
	c_json get_local;
	char flg;
	char st[40],ch[10];
	printf("Application starting\n");
	printf("please enter mode_type as -> 0 for Server,1 for Client, 2 for Both,3 for Gateway(routing capabilities)\n");
	printf("mode type is:");
	scanf("%d", &mode_type);

	if(dm_init(mode_type)!=0)
	{
		printf("failed to initialize\n");
		exit(1);
	}


	DM_Discovery(dm_ondiscovery);
	while(1)
	{
		printf("please enter your choice 'get': to get 'put' to put  'obs' for observe\n");
		scanf("%s",ch);
		if(strcmp(ch,"get")==0)
		{
			printf("enter name to get:");
			scanf("%s",st);
			res_local = selection(st);
			if(res_local !=NULL)
			{
			get_local=DM_Get(res_local,DM_onGet);
			for(int i=0;i<get_local.NumOfElements;i++)
			{
				printf("%s : %s\n",get_local.atr[i].atr_name,get_local.atr[i].atr_value);
			}
				}
			else
				printf("No Match Found\n");
		}
		else if(strcmp(ch,"put")==0)
		{
			printf("select uri to put:");
			scanf("%s",st);
			res_local = selection(st);
			if(res_local != NULL)
			{
			get_local=DM_Get(res_local,DM_onGet);
			for(int i=0;i<get_local.NumOfElements;i++)
			{
				printf("%s :",get_local.atr[i].atr_name);
				scanf("%s",get_local.atr[i].atr_value);
			}
			
 			DM_Put(res_local,get_local,DM_onPut);
				}//if
			else
				printf("No Match Found\n");

		}
		else if(strcmp(ch,"obs")==0)
		{
			printf("select uri to observe:");
			scanf("%s",st);
			res_local = selection(st);
			if(res_local !=NULL)
				DM_observe(res_local,DM_onobserve);
			else
			printf("No Match Found \n");

		}
		else
		{
			printf("invalid choice\n");
			exit(1);
		}
	}
	while(1);
	return 0;

}


