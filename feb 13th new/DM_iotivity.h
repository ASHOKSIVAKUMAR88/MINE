#include<mutex>
#include<condition_variable>
#include<json/json.h>
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
#include<memory>

#include<vector>
using namespace std;
using namespace OC;

#define MAX_RESOURCE 40
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

Resource_struct *resource_head = NULL;
//OCResource *DM_Resource_new[50];
//std::map<int, std::shared_ptr<OCResource> > DM_Resource1;
//auto DM_Resource[50];

//Resources_struct res_new;
//Resources_struct local;

int dm_init(int mode_type);
void on_Get(const HeaderOptions& , const OCRepresentation&, const int );
//Resources_struct dm_discovery(void);
json_object* dm_discovery(void);

//void DM_onGet(Resources_info resource_info, c_json rep_json);

