#include"DM_iotivity.h"
#include"DM_iotivity.cpp"
#include<stdio.h>
int main()
{	
	int mode_type;
	//int ResID, choice;
	//int a;

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
	jobj_local = dm_discovery();
	printf("Number of resources: %d",json_object_array_length(jobj_local));
	json_object_object_foreach(jobj_local,key,val)
	{
		printf("new:%s\nvalue:%s\n",key,json_object_get_string(val));
		}
	
	while(1);
	return 0;
	
}


