#include "iotivity_config.h"

//#define DEBUG
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
#include <condition_variable>
#include "OCPlatform.h"
#include "OCApi.h"
#include<iostream>
using namespace std;
mutex blocker;
 std::mutex mylock;
condition_variable cv,cv1;
//unique_lock<std::mutex> Lock(blocker);
//unique_lock<std::mutex> ml(mylock);
class Light
{
public:

    bool m_state;
    int m_power;
    std::string m_name;
    std::string m_string;

    Light() : m_state(false), m_power(0), m_name(""),m_string("NUll data")
    {
	   }
};

Light mylight;//global instance

using namespace OC;std::shared_ptr<OCResource> curResource;
typedef std::map<  OCResourceIdentifier, std::shared_ptr<OCResource> > DiscoveredResourceMap;
DiscoveredResourceMap discoveredResources;
static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe; 
std::mutex curResourceLock;
static OCConnectivityType TRANSPORT_TYPE_TO_USE = OCConnectivityType::CT_ADAPTER_IP;
void postLightRepresentation(std::shared_ptr<OCResource>);
void getLightRepresentation(std::shared_ptr<OCResource>);

/////////////////////////REGISTRATION////////////////////////

int observe_count()
{
    static int oc = 0;
    return ++oc;
}


void onObserve(const HeaderOptions /*headerOptions*/, const OCRepresentation& rep,
                    const int& eCode, const int& sequenceNumber)
{
    try
    {
        if(eCode == OC_STACK_OK && sequenceNumber <= MAX_SEQUENCE_NUMBER)
        {
            if(sequenceNumber == OC_OBSERVE_REGISTER)//
            {
                std::cout << "Observe registration action is successful" << std::endl;
            }

            std::cout << "OBSERVE RESULT:"<<std::endl;
            std::cout << "\tSequenceNumber: "<< sequenceNumber << std::endl;
            rep.getValue("state", mylight.m_state);
            rep.getValue("power", mylight.m_power);
            rep.getValue("name", mylight.m_name);
	    rep.getValue("input_string", mylight.m_string);
	
            std::cout << "\tstate: " << mylight.m_state << std::endl;
            std::cout << "\tpower: " << mylight.m_power << std::endl;
            std::cout << "\tname: " << mylight.m_name << std::endl;
	    std::cout << "\tinput_string: " << mylight.m_string << std::endl;
	 if(observe_count() == 11)
            {
                std::cout<<"Cancelling Observe..."<<std::endl;
                OCStackResult result = curResource->cancelObserve();

                std::cout << "Cancel result: "<< result <<std::endl;
                sleep(5);
                std::cout << "DONE"<<std::endl;
                //std::exit(0);
            }
  
        }
        else
        {
            if(eCode == OC_STACK_OK)
            {
		
                std::cout << "No observe option header is returned in the response." << std::endl;
                std::cout << "For a registration request, it means the registration failed"
                        << std::endl;
            }
            else
            {
                std::cout << "onObserve Response error: " << eCode << std::endl;
                std::exit(-1);
            }
        }
    }
    catch(std::exception& e)
    {
        std::cout << "Exception: " << e.what() << " in onObserve" << std::endl;
    }
	cv.notify_all();

}
 static int choice=1,sel=1;
///////////////////////////found resource///////////////
void foundResource(std::shared_ptr<OCResource> resource)
{
   
    //std::cout << "In foundResource\n";
    std::string resourceURI;
    std::string hostAddress;
   
	
    /*try
    {
        {
	//Lock.lock();		
	std::unique_lock<std::mutex> lock(curResourceLock);
            //std::lock_guard<std::mutex> lock(curResourceLock);
            if(discoveredResources.find(resource->uniqueIdentifier()) == discoveredResources.end())
            {
                std::cout << "Found resource " << resource->uniqueIdentifier() <<
                    " for the first time on server with ID: "<< resource->sid()<<std::endl;
                discoveredResources[resource->uniqueIdentifier()] = resource;
            }
            else
            {
                std::cout<<"Found resource "<< resource->uniqueIdentifier() << " again!"<<std::endl;
            }
////////////////////////////////modifying////////////////////////////
           if(curResource)
            {
                std::cout << "Found another resource, ignoring"<<std::endl;
                return;
            }
        }*/
        if(resource)
        {
            //std::cout<<"DISCOVERED Resource:"<<std::endl;
            resourceURI = resource->uri();
            //std::cout << "\tURI of the resource: " << resourceURI << std::endl;

            hostAddress = resource->host();
            //std::cout << "\tHost address of the resource: " << hostAddress << std::endl;

           // std::cout << "\tList of resource types: " << std::endl;
            /*for(auto &resourceTypes : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceTypes << std::endl;
            }


            //std::cout << "\tList of resource interfaces: " << std::endl;
            for(auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                std::cout << "\t\t" << resourceInterfaces << std::endl;
            }*/

	//std::cout<<"\t\t\t In listing resources "<<hostAddress<<std::endl;
         //cout<<"Found Resource: "<<resourceURI<<endl;
	if(resourceURI == "/a/ashok")
		cout<<"1."<<resourceURI<<endl;
	 if(resourceURI == "/a/ashok434")
		cout<<"2."<<resourceURI<<endl;
	  if(resourceURI == "/a/divya")
		cout<<"3."<<resourceURI<<endl;
	   if(resourceURI == "/a/ramya")
		cout<<"4."<<resourceURI<<endl;
		if(resourceURI == "/a/votarytech")
			cout<<"5."<<resourceURI<<endl;
	
	if(sel)
	{
		sel = 0;
		cout<<"Enter your resource selection"<<endl;
		cin>>choice;
		cv1.notify_all();
		///cv.notify_all();
		}
	unique_lock<std::mutex> ml(mylock);
	cv1.wait(ml);
	 if(resourceURI == "/a/ashok" && choice ==1 )
            {
			cout<<"1."<<resourceURI<<endl;
                if (resource->connectivityType() & TRANSPORT_TYPE_TO_USE)
                {
                    curResource = resource;
  		    cv.notify_all();
                }
            }
	 if(resourceURI == "/a/ashok434" && choice ==2)
            {
			cout<<"2."<<resourceURI<<endl;
                if (resource->connectivityType() & TRANSPORT_TYPE_TO_USE)
                {
                    curResource = resource;
			cv.notify_all();

                   
                }
            }
	 if(resourceURI == "/a/divya" && choice == 3)
            {
			cout<<"3."<<resourceURI<<endl;
                if (resource->connectivityType() & TRANSPORT_TYPE_TO_USE)
                {
                    curResource = resource;
			cv.notify_all();

                   
                }
            }
	 if(resourceURI == "/a/ramya" && choice == 4)
            {
			cout<<"4."<<resourceURI<<endl;
                if (resource->connectivityType() & TRANSPORT_TYPE_TO_USE)
                {
                    curResource = resource;
			cv.notify_all();

                   
                }
            }
	if(resourceURI == "/a/votarytech" && choice ==5 )
            {
			cout<<"1."<<resourceURI<<endl;
                if (resource->connectivityType() & TRANSPORT_TYPE_TO_USE)
                {
                    curResource = resource;
  		    cv.notify_all();
                }
            }
        }
        else
        {
            // Resource is invalid
            std::cout << "Resource is invalid" << std::endl;
        }

    /*}
    catch(std::exception& e)
    {
        std::cerr << "Exception in foundResource: "<< e.what() << std::endl;
    }*/
}

/////////////////////////init definition/////////////////////////////////////////////////////
int init(void)
{
    sel =1;// A global variable used at found resources 
    std::ostringstream requestURI;
    try
    {
        requestURI << OC_RSRVD_WELL_KNOWN_URI;
	OCPlatform::findResource("",requestURI.str(),CT_ADAPTER_IP , &foundResource);
        std::cout<< "Finding Resource... " <<std::endl;
    }
catch(OCException& e)
    {
        oclog() << "Exception in main: "<<e.what();
    }

    return 0;
}/////////////////init()//////////




/////////////////////////////////GET///////////////////
void onGet(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
    try
    {
        if(eCode == OC_STACK_OK)
        {
            std::cout << "GET request was successful" << std::endl;
            std::cout << "Resource URI: " << rep.getUri() << std::endl;

            rep.getValue("state", mylight.m_state);
            rep.getValue("power", mylight.m_power);
            rep.getValue("name", mylight.m_name);
	    rep.getValue("input_string", mylight.m_string);

            std::cout << "\tstate: " << mylight.m_state << std::endl;
            std::cout << "\tpower: " << mylight.m_power << std::endl;
            std::cout << "\tname: " << mylight.m_name << std::endl;
	    std::cout << "\tinput_string: " << mylight.m_string << std::endl;
        }
        else
        {
            std::cout << "onGET Response error: " << eCode << std::endl;
            std::exit(-1);
        }
    }
    catch(std::exception& e)
    {
        std::cout << "Exception: " << e.what() << " in onGet" << std::endl;
    }
	
  		    cv.notify_all();
}

void getLightRepresentation(std::shared_ptr<OCResource> resource)
{
    if(resource)
    {
        std::cout << "Getting Light Representation..."<<std::endl;
        // Invoke resource's get API with the callback parameter

        QueryParamsMap test;
        resource->get(test, &onGet);
	
    }
}


///////////////////////////PUT//////////////////////
void onPut(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
    try
    {
        if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CHANGED)
        {
            std::cout << "PUT request was successful" << std::endl;

            rep.getValue("state", mylight.m_state);
            rep.getValue("power", mylight.m_power);
            rep.getValue("name", mylight.m_name);
	    rep.getValue("input_string", mylight.m_string);
	    

            std::cout << "\tstate: " << mylight.m_state << std::endl;
            std::cout << "\tpower: " << mylight.m_power << std::endl;
            std::cout << "\tname: " << mylight.m_name << std::endl;
				std::cout << "\tinput_string: " << mylight.m_string << std::endl;
        }
        else
        {
            std::cout << "onPut Response error: " << eCode << std::endl;
            std::exit(-1);
        }
    }
    catch(std::exception& e)
    {
        std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
    }
	
  		    cv.notify_all();
}
void putLightRepresentation(std::shared_ptr<OCResource> resource)
{
	static int temp = 0;
    if(resource)
    {
        OCRepresentation rep;

        std::cout << "Putting light representation..."<<std::endl;

        mylight.m_state = true;
        mylight.m_power = 15;
	if(temp)
	{
	mylight.m_string = "World Domination";
	temp=0; 
	}
	else
	{
	std::cout<<"Enter string:"<<std::endl;
	std::cin>>mylight.m_string;
		}
        rep.setValue("state", mylight.m_state);
        rep.setValue("power", mylight.m_power);
	rep.setValue("input_string", mylight.m_string);
	std::cout<<"so far so good"<<std::endl;
        // Invoke resource's put API with rep, query map and the callback parameter
        resource->put(rep, QueryParamsMap(), &onPut);
    }
}

//////////////////////////////POST///////////////////


void onPost(const HeaderOptions& /*headerOptions*/,const OCRepresentation& rep, const int eCode)
{
    try
    {
        if(eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CREATED
                || eCode == OC_STACK_RESOURCE_CHANGED)
        {
            std::cout << "POST request was successful" << std::endl;

            if(rep.hasAttribute("createduri"))
            {
                std::cout << "\tUri of the created resource: "
                    << rep.getValue<std::string>("createduri") << std::endl;
            }
            else
            {
                rep.getValue("state", mylight.m_state);
                rep.getValue("power", mylight.m_power);
                rep.getValue("name", mylight.m_name);
		rep.getValue("input_string", mylight.m_string);
		

                std::cout << "\tstate: " << mylight.m_state << std::endl;
                std::cout << "\tpower: " << mylight.m_power << std::endl;
                std::cout << "\tname: " << mylight.m_name << std::endl;
		std::cout << "\tinput_string: " << mylight.m_string << std::endl;
            }
        }
        else
        {
            std::cout << "onPost Response error: " << eCode << std::endl;
            std::exit(-1);
        }
    }
    catch(std::exception& e)
    {
        std::cout << "Exception: " << e.what() << " in onPost" << std::endl;
    }
	
  		    cv.notify_all();
}

void postLightRepresentation(std::shared_ptr<OCResource> resource)
{
    if(resource)
    {
        OCRepresentation rep;

        std::cout << "Posting light representation..."<<std::endl;
	
        rep.setValue("state", mylight.m_state);
        rep.setValue("power", mylight.m_power);
	rep.setValue("input_string", mylight.m_string);
        resource->post(rep, QueryParamsMap(), &onPost);
    }
}



