using namespace std;
using namespace OC;
std::shared_ptr<OCResource> DM_resource;

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



string type_detection = "oic/res";	//fixed URI to found resources based on the resource type.

int dm_discovery(void);		


void found_resource(std::shared_ptr<OCResource> resource)
{

	std::string resourceURI;
	std::string hostAddress;

	if(resource)
	{ 
		resourceURI = resource->uri();
		std::cout << "\tURI of the resource: " << resourceURI << std::endl;
		hostAddress = resource->host();
		std::cout << "\tHost address of the resource: " << hostAddress << std::endl;

	}//if loopp
}//found_resource
int dm_discovery(void)//json_object *user_jobj,char * call_back_function)///&obj
{
	std::ostringstream requestURI;
	requestURI << OC_RSRVD_WELL_KNOWN_URI;

	std::cout<<"DISCOVERED Resource:"<<std::endl;
	OCPlatform::findResource("",requestURI.str(),CT_IP_USE_V4 ,&found_resource);
	return 0;
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

}
