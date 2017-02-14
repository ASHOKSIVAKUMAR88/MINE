#include"DM_iotivity.h"
#include"DM_iotivity.cpp"
#include"api.h"
#include<stdio.h>
/*typedef struct
{
	char res_name[40];
	char res_id[40];
	}Resources_struct;	*/
void dm_ondiscovery(json_object * jobj_new)
{
	printf("Number of resources: %d",json_object_array_length(jobj_new));
	json_object_object_foreach(jobj_new,key,val)
	{
		printf("new:%s\nvalue:%s\n",key,json_object_get_string(val));
	}


}
int main()
{	
	int mode_type;
	Resources_struct res;
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
	DM_Get(res,DM_onGet);
	else
	{
	printf("no match found\n");
	continue;
		}
			}
	while(1);
	return 0;
	
}


