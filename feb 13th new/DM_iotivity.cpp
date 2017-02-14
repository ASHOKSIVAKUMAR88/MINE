
//#include"DM_iotivity.h"
/*#define MAX_RESOURCE 40
typedef struct
{
	char res_name[40];
	char res_id[40];
}Resources_info;

typedef struct
{
	int numOfResources;
	Resources_info ident[MAX_RESOURCE] ;
}Resources_struct;

typedef struct RESOURCE
{
	std::shared_ptr<OCResource> DM_Resource;
	//int index;
	struct RESOURCE *next;
	}Resource_struct;

Resource_struct *resource_head = NULL;*/
static OCConnectivityType TRANSPORT_TYPE_TO_USE = OCConnectivityType::CT_ADAPTER_IP;


/****************************INIT api***************************/


int dm_init(int mode_type);	//filename---->file for future database management   mode----->0 for Server,1 for Client, 2 for Both,3 for Gateway(routing capabilities)

int dm_init(int mode_type)
{
	try
	{	
		OC::ModeType mode;
		if(mode_type ==0)
			mode = OC::ModeType::Server;
	 	else if(mode_type ==1)
			mode = OC::ModeType::Client;
		else if(mode_type ==2)
			mode = OC::ModeType::Both;
		
		PlatformConfig cfg {OC::ServiceType::InProc,mode,"0.0.0.0",0,OC::QualityOfService::HighQos};
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
////////////////////Mutex LOCK//////////////
mutex std_lock;
condition_variable resource_lock;
std::unique_lock<std::mutex> lock_res(std_lock);



////instance/////////
Resources_struct res_new;

string type_detection = "oic/res";	//fixed URI to found resources based on the resource type.



static int b=0;;
  std::mutex dm_blocker;
	std::mutex DM_LOCK_new;
        std::condition_variable dm_cv;
        std::unique_lock<std::mutex>dm_lock_new(DM_LOCK_new);
json_object *jobj_global = json_object_new_object();
void found_resource(std::shared_ptr<OCResource> resource)
{
	std::string resourceURI;
	std::string resourceID;
	
	dm_blocker.lock();
	std::ostringstream resourceUnique;
	if(resource)
	{ 		
		resourceURI = resource->uri();
		//cout<<"identifier:"<<resource->uniqueIdentifier()<<endl;
		resourceID = string(resource->sid());
		Resource_struct *temp = resource_head;
		Resource_struct *local = (Resource_struct *)malloc(sizeof(Resource_struct)); 
		local->DM_Resource = resource;
		//local->index = b;
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
		json_object *obj =  json_object_new_string(resourceID.c_str());
		json_object_object_add(jobj_global,resourceURI.c_str(),obj);
		//strcpy(res_new.ident[b].res_name,r.c_str());
		//strcpy(res_new.ident[b].res_id,resourceID.c_str());
		
		b++;

		cout<<"good"<<endl;

		dm_blocker.unlock();
	}//if loop
}//found_resource


//Resources_struct dm_discovery(void)
json_object* dm_discovery(void)
{
	std::ostringstream requestURI;
	requestURI << OC_RSRVD_WELL_KNOWN_URI;
	std::cout<<"Discovering Resources:"<<std::endl;
	OCPlatform::findResource("",requestURI.str(),CT_IP_USE_V4 ,&found_resource);
	dm_cv.wait_for(dm_lock_new,chrono::seconds(1));
	//std::cout <<"Waiting" << std::endl;
	//res_new.numOfResources = b;
	return jobj_global;
	//return res_new;
}///dm_discovery

/*
/////////////////////////GET////////////////////////////////////////////
void DM_onGet(Resources_info resource_info, c_json rep_json)
{
	try
	{
	int i;
	for(i=0;i<=MAX_NUM_OF_ATR;i++)
		{
		printf("%s %s\n",rep_json.atr[i].atr_name,rep_json.atr[i].atr_value);	
		}
	}
	catch
	{
	cout<<"DM_onGet"<<endl;
	}
}
*/
/////////////// conversion from OCRepresentation to Json format//////
void on_Get(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
		try
		{
		int j=0;
		map<std::string, AttributeValue> div;
		div = rep.getValues();
		std::string temp,temp1;
		for(map<std::string, AttributeValue>::iterator i = div.begin();i != div.end(); i++,j++)
			{
				temp=string(i->first);
				cout<<"key:"<<temp<<endl;
				//strcpy(DM_json.atr[j].atr_name,(temp).c_str());
				temp1=(rep.getValueToString(temp));
				cout<<"value:"<<temp1<<endl;
				//strcpy(DM_json.atr[j].atr_value,temp1.c_str());
			}
		}
		catch(exception &e)
		{
		cout <<"on_Get failed"<<endl;
		}
}
/*
void DM_Get(Resources_info resource_info, fun_ptr call_back)
{

	try
	{
	
        QueryParamsMap test;
        DM_resource->get(test, &on_Get);
	///call_back(DM_json);
	}
	catch
	{
	cout<<"DM_get failed"<<endl;
	}
}
*/


