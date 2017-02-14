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
/*
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
*/
typedef struct RESOURCE
{
	std::shared_ptr<OCResource> DM_Resource;
	//int index;
	struct RESOURCE *next;
	}Resource_list;

Resource_list *resource_head = NULL;
int dm_init(int mode_type);
void on_Get(const HeaderOptions& , const OCRepresentation&, const int );

typedef void (*disc_ptr)(json_object*);
json_object* dm_discovery(json_object*);

//void DM_onGet(Resources_info resource_info, c_json rep_json);

