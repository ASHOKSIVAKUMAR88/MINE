#include<stdio.h>
//#include"client.h"
#include"init.h"
//#include"api.h"
#include<json/json.h>
int main()
{	
	//Resources_struct local;	//structure defined in low level
	int a =0,res;
	char uri[100];
	json_object * json_old = json_object_new_object();
	if((dm_init(1))==1)
	{
		printf("failed to initialize\n");
		exit(1);
	}
	json_old = dm_discovery();
	json_object_object_foreach(json_old,key,val)
	{
		printf("new:%s\nvalue:%s\n",key,json_object_get_string(val));
		}
	while(1);
	return 0;
}//main()

