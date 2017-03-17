//#include"OCRepresentation.h"

#define MAX_NUM_OF_ATR 10
//void DM_Get(fun_ptr);
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
typedef struct
{
	char res_name[40];
	char res_id[40];
	}Resources_struct;
//Resources_info resource_info; //{name,id}

typedef void (*fun_ptr)(c_json);

c_json DM_json;

/*
with linked list:
-----------------
typedef struct RESOURCE
{
    std::shared_ptr<OCResource> DM_Resource;
    struct RESOURCE *next;
 }Resource_list;
 Resorce_list *resource_head=NULL;
*/

/////////////////////////GET////////////////////////////////////////////
void DM_onGet(c_json rep_json)
{
	try
	{
	int i;
	cout<<"\t\t\tresource attributes "<<endl;
	for(i=0;i<rep_json.NumOfElements;i++)
		{
			printf("%s %s\n",rep_json.atr[i].atr_name,rep_json.atr[i].atr_value);	
		}
	}
	catch(std::exception& e)
	{
		cout << "Exception: " << e.what() <<"DM_onGet"<<endl;
	}
}

/////////////// conversion from OCRepresentation to Json format//////
std::mutex DM_LOCK_GET;
std::condition_variable DM_GET;
std::unique_lock<std::mutex>DM_WAIT(DM_LOCK_GET);
void on_Get(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
		try
		{
		DM_json.NumOfElements=rep.numberOfAttributes();
		int j=0;
		map<std::string, AttributeValue> div;
		div = rep.getValues();
		std::string temp,temp1;
		for(map<std::string, AttributeValue>::iterator i = div.begin();i != div.end(); i++,j++)
		     {
			temp=string(i->first);
			strcpy(DM_json.atr[j].atr_name,(temp).c_str());
			temp1=(rep.getValueToString(temp));
			strcpy(DM_json.atr[j].atr_value,temp1.c_str());
		      }
		}
		catch(std::exception& e)
		{
		cout << "Exception: " << e.what() << " in on_Get failed"<<endl;
		}
		DM_GET.notify_all();
}
void DM_Get(Resources_struct resource1, fun_ptr call_back)
{
	
	try
	{
	int j;
	QueryParamsMap test;
        Resource_list *cur_node=resource_head;
        while(cur_node != NULL)
            {  	if((resource1.res_name==cur_node->DM_Resource->uri()) &&
                   (resource1.res_id==cur_node->DM_Resource->sid()))
	        	{	
						
                                cur_node->DM_Resource->get(test, &on_Get);
				DM_GET.wait(DM_WAIT);
				call_back(DM_json);
				break;
			}		
            cur_node=cur_node->next;
            }
       // free(cur_node);
        }
	catch(std::exception& e)
	{
	cout << "Exception: " << e.what() << " in DM_get failed" <<endl;
	}
}

//////////////////////put/////////////////////////////////////////////
/*
void DM_onPut(c_json dmjson)
{
		try
		{
                        int i;
			for(i=0;i<=MAX_NUM_OF_ATR;i++)
				printf("%s %s\n",dmjson.atr[i].atr_name,dmjson.atr[i].atr_value);	
		}
		catch(std::exception& e)
		{
			cout << "Exception: " << e.what() << " in DM_onput failed" <<endl;
		}
		
}

void onPut(const HeaderOptions& /*headerOptions, const OCRepresentation& rep, const int eCode)
{
return;		
}

void DM_Put(Resources_struct resource1, fun_ptr call_backargument as DM_onPut) ///int resID just added ..not using now
{
      try
       {
        int j,i;
        QueryParamsMap test;
        Resource_list *cur_node=resource_head;
        while(cur_node != NULL)
            {  	if((resource1.res_name==cur_node->DM_Resource->uri()) &&
                   (resource1.res_id==cur_node->DM_Resource->sid()))
                        {
			for(i=0;i<=MAX_NUM_OF_ATR;i++)
			    rep.setValue(DM_json.atr[i].atr_name, DM_json.atr[i].atr_value);
			}		
            cur_node->DM_resource->put(rep, QueryParamsMap(), &on_Put);
            call_back(DM_json);
            }
        free(cur_node);
       }
      catch(std::exception& e)
       {
	cout << "Exception: " << e.what() << " in Put failed" <<endl;
       }
}



*/


