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
	}Resources_info;
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
		std::vector<std::string> vector_new= rep.getDataModelVersions();
		cout<<"ohMY god"<<endl;
		for(std::vector<std::string>::iterator it = vector_new.begin();it != vector_new.end();it++)
			cout<<"Vector "<<*it<<endl;
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
c_json DM_Get(Resources_info resource1, fun_ptr call_back)
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
	return DM_json;
}

//////////////////////put/////////////////////////////////////////////

void DM_onPut(c_json dmjson)
{
		try
		{
                        int i;
			cout<<"ashok pichi:"<<endl;
			for(i=0;i<=dmjson.NumOfElements;i++)
				printf("%s %s\n",dmjson.atr[i].atr_name,dmjson.atr[i].atr_value);	
		}
		catch(std::exception& e)
		{
			cout << "Exception: " << e.what() << " in DM_onput failed" <<endl;
		}
		
}

void onPut(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
		DM_GET.notify_all();
return;	
	
}

void DM_Put(Resources_info resource1,c_json in_data, fun_ptr call_back/*argument as DM_onPut*/) ///int resID just added ..not using now
{
      try
       {
	cout<<"put checking:"<<endl;
        int j,i;
        QueryParamsMap test;
	OCRepresentation rep;
	
        Resource_list *cur_node=resource_head;
        while(cur_node != NULL)
            {  	if((resource1.res_name==cur_node->DM_Resource->uri()) &&
                   (resource1.res_id==cur_node->DM_Resource->sid()))
                        {
			for(i=0;i<=in_data.NumOfElements;i++)
			{
				string key(in_data.atr[i].atr_name);
				string value(in_data.atr[i].atr_value);
				int x=1;
 				rep.setValue(key,x);
			}
			    //rep.setValue(string(in_data.atr[i].atr_name), string(in_data.atr[i].atr_value));
				cur_node->DM_Resource->put(rep, QueryParamsMap(), &onPut);
				DM_GET.wait(DM_WAIT);
				call_back(in_data);
				break;
			}		
            		cur_node = cur_node->next;
			
           		 
            }
       }
      catch(std::exception& e)
       {
	cout << "Exception: " << e.what() << " in Put failed" <<endl;
       }
}


