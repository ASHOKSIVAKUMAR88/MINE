#include<mutex>
#include<condition_variable>
#include<json/json.h>
using namespace std;
using namespace OC;
std::shared_ptr<OCResource> DM_resource;
//std::shared_ptr<int> a;



/****************************INIT api***************************/


int dm_init(int mode_type);	//filename---->file for future database management   mode----->0 for Server,1 for Client, 2 for Both,3 for Gateway(routing capabilities)

int dm_init(int mode_type)
{
	try
	{
		PlatformConfig cfg {OC::ServiceType::InProc,OC::ModeType::Client,"0.0.0.0",0,OC::QualityOfService::HighQos,NULL};
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


#define MAX_RESOURCE 10
typedef struct
{
char res_name[10];
int dev_id;
	}Resources_info;
typedef struct
{
	int numOfResources;
	Resources_info ident[MAX_RESOURCE] ;
	}Resources_struct;
////instance/////////
Resources_struct res_new;

string type_detection = "oic/res";	//fixed URI to found resources based on the resource type.

//int dm_discovery(void);		


static int a =1,b=0;;
  std::mutex dm_blocker;
	std::mutex DM_LOCK_new;
        std::condition_variable dm_cv;
	std::unique_lock<std::mutex>dm_lock_new(DM_LOCK_new);
json_object *jobj_global = json_object_new_array();
void found_resource(std::shared_ptr<OCResource> resource)
{
	std::string resourceURI;
	string resourceID;
	dm_blocker.lock();
	if(resource)
	{ 	
		resourceID = resource->sid().c_str();
		resourceURI = resource->uri().c_str();
		json_object *jobj = json_object_new_object();
		//json_object *obj  =  json_object_new_string(resourceURI.c_str());
		json_object *obj1 =  json_object_new_string(resourceID.c_str());
		b++;
		
		
		json_object_object_add(jobj,resourceURI.c_str(),obj1);
		json_object_array_add(jobj_global,jobj);
		//json_object_object_add(jobj_global,to_string(b).c_str(),jobj);//segmentation fault
		
		//std::cout<<"New string:"<<std::endl<<json_object_to_json_string(jobj_global)<<std::endl;
		//json_object_object_foreach(jobj_global,key,value)
		//{
			//cout<<"key:"<<key<<"value:"<<endl;
	
			//}

		//sleep(1);
//		std::cout << "\tURI of the resource:<b> "<<b<<" nnn" <<resource->sid()<<"new"<< resourceURI << std::endl;
		//hostAddress = resource->host();
		//std::cout << "\tHost address of the resource: " << hostAddress << std::endl;
	dm_blocker.unlock();
	
	}//if loopp
}//found_resource


json_object* dm_discovery(void)//json_object *user_jobj,char * call_back_function)///&obj
{
	std::ostringstream requestURI;
	requestURI << OC_RSRVD_WELL_KNOWN_URI;

	std::cout<<"DISCOVERED Resource:"<<std::endl;
	OCPlatform::findResource("",requestURI.str(),CT_IP_USE_V4 ,&found_resource);
	std::cout <<"Waiting" << std::endl;
	dm_cv.wait_for(dm_lock_new,chrono::seconds(2));
	json_object* jobj_local = json_object_new_object();
	json_object_object_add(jobj_local,"resources",jobj_global);
	return jobj_local;
	//return res_new;
}///dm_discovery


/***************************connect api************************/


string URI_PATH;
void found_discovery(std::shared_ptr<OCResource> resource)
{

	//std::cout << "In foundResource\n";
	std::string resourceURI;
	std::string hostAddress;

	if(resource)
	{ 
		resourceURI = resource->uri();
		hostAddress = resource->host();
		if(resourceURI == URI_PATH)
		{
			if (resource->connectivityType() & TRANSPORT_TYPE_TO_USE)
				DM_resource = resource;
			std::cout<<"connection established with:"<<resourceURI<<std::endl;
		}//if loop for uri path matching	
	}//if loopp

}//found_discovery

void connect(char * uri_path)
{
	std::ostringstream requestURI;
	requestURI << OC_RSRVD_WELL_KNOWN_URI;
	URI_PATH = uri_path;
	OCPlatform::findResource("",requestURI.str(),CT_IP_USE_V4 ,&found_discovery);
dm_cv.wait_for(dm_lock_new,chrono::seconds(2));

}
