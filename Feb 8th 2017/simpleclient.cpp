#include"client.h"
#include"init.h"
//#include"api.h"

int main()
{	
	int i=0;	
	char *s;
	json_object *jobj_u = json_object_new_object();
	json_object *jobj_l = json_object_new_object();
	dm_init(0);
	jobj_u =dm_discovery();
	connect("/a/ashok");
	printf("%s\n",json_object_to_json_string(jobj_u));
	//for(i=0;i<json_object_array_length(jobj_u);i++)
	 //{	
		//jobj_l=json_object_get(jobj_u);
		//printf("%s\n",json_object_get_string(jobj_l));
		//getchar();
		//s=json_object_get_string(jobj_l);
		//printf("%s\n",s);
//}
		/*json_object_object_foreach(jobj_l,key,value)
	{
		
		printf("dumm2\n");
		printf("%s %s\n",key,value);
		}
		
		}*/
	
/*	json_object_object_foreach(jobj_u,key,value)
	{
		
		
		}
	printf("%s\n",json_object_to_json_string(jobj_u));*/
	while(1);
	
}//main()

