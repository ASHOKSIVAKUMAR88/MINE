#include<mutex>
#include<condition_variable>
#include "iotivity_config.h"
#include<stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <Windows.h>
#endif
#include <string>
#include <map>
#include <cstdlib>
#include <mutex>
#include "OCPlatform.h"
#include "OCApi.h"
#include<iostream>
#include<OCRepresentation.h>
#include<json/json.h>
using namespace std;
using namespace OC;
std::shared_ptr<OCResource> DM_resource;
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


#define MAX_RESOURCE 10
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
////instance/////////
Resources_struct res_new;

string type_detection = "oic/res";	//fixed URI to found resources based on the resource type.



static int b=0;;
  std::mutex dm_blocker;
	std::mutex DM_LOCK_new;
        std::condition_variable dm_cv;
        std::unique_lock<std::mutex>dm_lock_new(DM_LOCK_new);
void found_resource(std::shared_ptr<OCResource> resource)
{
	std::string resourceURI;
	std::string resourceID;
	dm_blocker.lock();

	if(resource)
	{ 		
		resourceURI = resource->uri();
		resourceID=resource->sid();
		strcpy(res_new.ident[b].res_name,resourceURI.c_str());
		strcpy(res_new.ident[b].res_id,resourceID.c_str());
		//cout<<"resource:"<<b<<" "<<res_new.ident[b].res_name<<endl;
		b++;
		dm_blocker.unlock();
	}//if loopp
}//found_resource


struct json_object* dm_discovery(void)
{
	//json_object* json_local = json_object_new_object();
	std::ostringstream requestURI;
	requestURI << OC_RSRVD_WELL_KNOWN_URI;

	std::cout<<"DISCOVERED Resource:"<<std::endl;
	OCPlatform::findResource("",requestURI.str(),CT_IP_USE_V4 ,&found_resource);
	dm_cv.wait_for(dm_lock_new,chrono::seconds(1));
	std::cout <<"Waiting" << std::endl;
	res_new.numOfResources = b;
	json_object* json_local = json_object_new_object();
	json_object* json_local_array = json_object_new_array();
	for(int i=res_new.numOfResources-1;i>=0;i++)
	{
		for(int j = 0;j<=i;j++)
			json_object
		}
	return json_local;
}///dm_discovery


/***************************connect api************************/


string URI_PATH;
void found_discovery(std::shared_ptr<OCResource> resource)
{
	std::string resourceURI;
	std::string hostAddress;
	dm_blocker.lock();
	if(resource)
	{ 
		resourceURI = resource->uri();
		if(resourceURI == URI_PATH)
		{
			if (resource->connectivityType() & TRANSPORT_TYPE_TO_USE)
				DM_resource = resource;
			std::cout<<"connection established with:"<<resourceURI<<std::endl;
		}//if loop for uri path matching	
	}//if loop
		dm_blocker.unlock();
	return ;

}//found_discovery
int connect(char * uri_path)
{
	string s(uri_path);
	std::ostringstream requestURI;

	requestURI << OC_RSRVD_WELL_KNOWN_URI;
	URI_PATH = s;
OCPlatform::findResource("",requestURI.str(),CT_IP_USE_V4 ,&found_discovery);
dm_cv.wait_for(dm_lock_new,chrono::seconds(2));
return 0;
}
