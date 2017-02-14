
#define MAX_NUM_OF_ATR 10

typedef struct 
{
char att_name[30];
char att_value[30];
}Attribute1;

typedef struct
{
int NumOfElements;
Attribute1 atr[MAX_NUM_OF_ATR];
}c_json;

c_json DM_json;
/////////////prototype///////////////
typedef void (*fun_ptr)(c_json);
void DM_Get(fun_ptr*);

void DM_onGet(c_json rep_json)
{
int i;
for(i=0;i<=rep_json.NumOfElements;i++)
	{
	printf("%s %s\n",rep_json.atr[i].att_name,rep_json.atr[i].att_value);	
	}
}

/////////////// conversion from OCRepresentation to Json format//////
void setJson(c_json& rep_json,OCRepresentation rep )
{
		map<std::string, AttributeValue> div;
		div = rep.getValues();
		rep_json.NumOfElements=rep.numberOfAttributes();
		int j=0;
		for(map<std::string, AttributeValue>::iterator i = div.begin();i != div.end(); i++,j++)
			{
			//rep_json.atr[j].att_name=i->first;
			//rep_json.atr[j].att_value=rep.getValueToString(i->first);
					}
               
}
/////////////////////////////////////////////////////////////////////

void on_Get(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
setJson(DM_json,rep);
}

void DM_Get(fun_ptr *call_back)
{
        QueryParamsMap test;
        DM_resource->get(test, &on_Get);
	(*call_back)(DM_json);
}


