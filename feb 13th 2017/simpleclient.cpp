#include<stdio.h>
//#include"client.h"
#include"init.h"
//#include"api.h"
#include<json/json.h>
int main()
{	
	//Resources_struct local;	//structure defined in low level
	int a =0,res,j=0;
	char uri[100];
	Resources_struct res_old;
	json_object * json_old = json_object_new_object();
	if((dm_init(1))==1)
	{
		printf("failed to initialize\n");
		exit(1);
	}
	json_old = dm_discovery();
	json_object_object_foreach(json_old,key,val)
	{
		strcpy(res_old.ident[a].res_name,key);
		strcpy(res_old.ident[a].res_id,json_object_get_string(val));
		a++;
		printf("new:%s\nvalue:%s\n",key,json_object_get_string(val));
		}
	for(j=0;j<a;j++)
		printf("key:%s value:%s\n",res_old.ident[j].res_name,res_old.ident[j].res_id);
	//printf("on while\n");
	while(1);
	return 0;
}//main()

