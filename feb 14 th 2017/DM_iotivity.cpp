
//#include"DM_iotivity.h"


/*
typedef struct RESOURCE
{
	std::shared_ptr<OCResource> DM_Resource;
	//int index;
	struct RESOURCE *next;
	}Resource_list;

Resource_list *resource_head = NULL;
static OCConnectivityType TRANSPORT_TYPE_TO_USE = OCConnectivityType::CT_ADAPTER_IP;
*/

/****************************INIT api***************************/


int dm_init(int mode_type);	//filename---->file for future database management   mode----->0 for Server,1 for Client, 2 for Both,3 for Gateway(routing capabilities)

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

string type_detection = "oic/res";	//fixed URI to found resources based on the resource type.



static int b=0;;
std::mutex dm_blocker;
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
		resourceID = string(resource->sid());
		Resource_list *temp = resource_head;
		Resource_list *local = (Resource_list *)malloc(sizeof(Resource_list)); 
		local->DM_Resource = resource;
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
		b++;
		
	}//if loop
dm_blocker.unlock();
}//found_resource

std::mutex DM_LOCK_new;
std::condition_variable dm_cv;
std::unique_lock<std::mutex>dm_lock_new(DM_LOCK_new);

void dm_ondiscovery(json_object * jobj_new)
{
	printf("Number of resources: %d",json_object_array_length(jobj_new));
	json_object_object_foreach(jobj_new,key,val)
	{
		printf("new:%s\nvalue:%s\n",key,json_object_get_string(val));
	}
}

json_object* dm_discovery(disc_ptr call_back)
{
	std::ostringstream requestURI;
	requestURI << OC_RSRVD_WELL_KNOWN_URI;
	std::cout<<"Discovering Resources:"<<std::endl;
	OCPlatform::findResource("",requestURI.str(),CT_IP_USE_V4 ,&found_resource);
	std::cout <<"Waiting" << std::endl;
	dm_cv.wait_for(dm_lock_new,chrono::seconds(1));
	call_back(jobj_global);
	return jobj_global;
}///dm_discovery

