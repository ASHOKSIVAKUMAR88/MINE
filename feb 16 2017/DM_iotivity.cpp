#include"DM_iotivity.h"
#include"observefunction.cpp"
int dm_init(int mode_type)
{
	try
	{	
		ModeType mode_new;
		if(mode_type ==0)
			mode_new = OC::ModeType::Server;
	 	else if(mode_type ==1)
			mode_new = OC::ModeType::Client;
		else if(mode_type ==2)
			mode_new = OC::ModeType::Both;
		
		PlatformConfig cfg {OC::ServiceType::InProc,mode_new,"0.0.0.0",0,OC::QualityOfService::HighQos};
		OCPlatform::Configure(cfg);
		std::cout<<"client is intialized with mode type as client"<<std::endl;
		return 0;
	}
	catch(OCException& e)
	{
		oclog() << "Exception in main: "<<e.what();
		return 1;
	}
}//init function


/******************************discover api**********************/
static int b=0;;
std::mutex dm_blocker;
Resource_list *resource_head = NULL;
disc_ptr global_call_back;
void found_resource(std::shared_ptr<OCResource> resource)
{
	std::string resourceURI;
	std::string resourceID;
	
	dm_blocker.lock();
	std::ostringstream resourceUnique;
	if(resource)
	{ 		
		resourceURI = resource->uri();
		resourceID = string(resource->sid());
		Resource_list *temp = resource_head;
		Resource_list *local = (Resource_list *)malloc(sizeof(Resource_list)); 
		local->DM_Resource = resource;
		//local->uri=resource->uri();
		strcpy(local->uri,(resource->uri()).c_str());
		local->onget_instance=NULL;
		local->onput_instance=NULL;
		if (resource_head == NULL)
		{
			local->next = resource_head;
			resource_head = local;
			}	
		else
		{
			local->next = NULL;
			while(temp->next != NULL)
			temp = temp->next;
			temp->next = local;
			}
		Resource_info *local_info=(Resource_info*)malloc(sizeof(Resource_info));
		local_info->handler=(Resource_list*)local;
		strcpy(local_info->name,(resource->uri()).c_str());
		global_call_back(local_info);
	}//if loop
dm_blocker.unlock();
}//found_resource

std::mutex DM_LOCK_new;
std::condition_variable dm_cv;
std::unique_lock<std::mutex>dm_lock_new(DM_LOCK_new);

void DM_Discovery(disc_ptr call_back)
{
global_call_back=call_back;
	std::ostringstream requestURI;
	requestURI << OC_RSRVD_WELL_KNOWN_URI;
	std::cout<<"Discovering Resources:"<<std::endl;
	OCPlatform::findResource("",requestURI.str(),CT_IP_USE_V4 ,&found_resource);
	std::cout <<"Waiting" << std::endl;
	dm_cv.wait_for(dm_lock_new,chrono::seconds(1));
	
	}///dm_discovery

void DM_onGet(c_json rep_json)
{

	try
	{
	int i;
	cout<<"\t\t\tresource attributes "<<endl;
	/*for(i=0;i<rep_json.NumOfElements;i++)
		{
			printf("%s %s\n",rep_json.atr[i].atr_name,rep_json.atr[i].atr_value);	
		}*/
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
json_object *jobj_global = json_object_new_object();
//template<class T>
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
			//auto good = i->second;
			//if(std::is_same<int,rep.getValue(i->first)>)
			//	cout<<"int"<<endl;
			//if(is_same<string,good>)
			//	cout<<"string"<<endl;
			//if(is_same<bool,good>)
			//	cout<<"bool"<<endl;
			//cout<<"type is :"<<typeid(i->second).name()<<endl;
			//json_object_add_object(jobj_global,string(i->first),)
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
c_json DM_Get(Resource_info* resource1, onGet_ptr1 call_back)
{

	try
	{
	int j;
	QueryParamsMap test;
	cout<<resource1->name<<endl;
    	resource1->handler->DM_Resource->get(test, &on_Get);

	DM_GET.wait(DM_WAIT);
	call_back(DM_json);
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
			cout<<"updated information :"<<endl;
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

void DM_Put(Resource_info* resource1,c_json in_data, onPut_ptr1 call_back) 

{
      try
       {
	
        QueryParamsMap test;
	OCRepresentation rep;
	int i,j;
			for(i=0;i<=in_data.NumOfElements;i++)
			{
				string key(in_data.atr[i].atr_name);
				string value(in_data.atr[i].atr_value);
				rep.setValue(key,value);
			}

				resource1->handler->DM_Resource->put(rep, QueryParamsMap(), &onPut);
				DM_GET.wait(DM_WAIT);
				call_back(in_data);
			 
       }
      catch(std::exception& e)
       {
	cout << "Exception: " << e.what() << " in Put failed" <<endl;
       }
}


