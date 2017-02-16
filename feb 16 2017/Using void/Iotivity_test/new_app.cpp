//#include<DM_iotivity.h>
//new code

/*
//APPLICATION using structure
typedef struct
{
        char name[30];
        void* handler;
}Resource_info;
*/

OnNode *header=NULL;

void dm_ondiscovery(Resource_info *node)
{
    OnNode *tmp2=header;
    OnNode *tmp=(OnNode *)malloc(sizeof(node));
    tmp->res=node;
    tmp->next=NULL;
    if(header==NULL)
    {
        header=tmp;
    }
    else
    {
        while(tmp2->next!=NULL)
        {
            tmp2=tmp2->next;
        }
        tmp2->next=tmp;
    }
	printf("resource name:%s\n",tmp->res->name);

}
/*get or put input*/
Resource_info* selection(char *usr_str)
{
Resource_info *gp_node;
OnNode *cur_node=header;
    while(cur_node != NULL)
        {  	if(strcmp(usr_str,cur_node->res->name)==0)
                    {
                         gp_node=cur_node->res;
				return gp_node;
                        //break;
                    }
        cur_node=cur_node->next;
        }
return NULL;
}
