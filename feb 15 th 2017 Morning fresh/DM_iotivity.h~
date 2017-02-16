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
#define MAX_NUM_OF_ATR 40
using namespace std;
using namespace OC;

string type_detection = "oic/res";	//fixed URI to found resources based on the resource type.

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

c_json DM_json;


typedef void (*onGet_ptr)(json_object*);
typedef void (*onPut_ptr)(json_object*);
typedef void (*onGet_ptr1)(c_json);
 
typedef struct RESOURCE
{
	char uri[30];
	onGet_ptr onget_instance;
	onPut_ptr onput_instance;
	std::shared_ptr<OCResource> DM_Resource;
	struct RESOURCE *next;
}Resource_list;

typedef struct 
{
char name[30];
Resource_list* handler;
}Resource_info;
typedef struct NODE
{
    Resource_info *res;
    NODE *next;
}OnNode;

typedef void (*disc_ptr)(Resource_info*);

int dm_init(int);
void DM_Discovery(disc_ptr);
c_json DM_get(Resource_info*,onGet_ptr);
void DM_onGet(c_json rep_json);
//c_json DM_put(Resource_info,json_object *,onPut_ptr);






