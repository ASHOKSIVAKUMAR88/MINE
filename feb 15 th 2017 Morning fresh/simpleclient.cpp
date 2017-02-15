#include"DM_iotivity.h"
#include"DM_iotivity.cpp"
//#include"api.h"
//#include"../../Source/IoTvity_Wrapper/DM_iotivity.h"
#include"new_app.cpp"
#include<stdio.h>
int main()
{	
	int mode_type;
	//Resource_info res;
	c_json get_local;
	int flg=0;
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
	
		
	dm_discovery(dm_ondiscovery);
	printf("enter name to select:");
	scanf("%s",st);
	get_local=DM_Get(selection(st),DM_onGet);
	for(int i=0;i<get_local.NumOfElements;i++)
	{
		printf("%s : %s\n",get_local.atr[i].atr_name,get_local.atr[i].atr_value);
	}

	while(1);
	return 0;
	
}


