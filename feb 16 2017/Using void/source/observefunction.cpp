

static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe;
Resource_info* global_res= NULL;

onobserve_ptr1 global_on_call_back =NULL;
/*----------------------------*/
void DM_onobserve(c_json rep_json)
{
    try
    {
        int i;
        for(i=0;i<=rep_json.NumOfElements;i++)
                printf("%s %s\n",rep_json.atr[i].atr_name,rep_json.atr[i].atr_value);
    }
    catch(std::exception& e)
    {
            cout << "Exception: " << e.what() <<"DM_onobserve"<<endl;
    }

}


void onObserve(const HeaderOptions /*headerOptions*/, const OCRepresentation& rep,
                    const int& eCode, const int& sequenceNumber)
{
    try
    {
        c_json local_json;
        static int k=0;
        local_json.NumOfElements=rep.numberOfAttributes();
        int j=0;
        map<std::string, AttributeValue> div;
        div = rep.getValues();
        std::string temp,temp1;
        for(map<std::string, AttributeValue>::iterator i = div.begin();i != div.end(); i++,j++)
             {
                temp=string(i->first);
                strcpy(local_json.atr[j].atr_name,(temp).c_str());
                temp1=(rep.getValueToString(temp));
                strcpy(local_json.atr[j].atr_value,temp1.c_str());
              }
          global_on_call_back(local_json);

         if(k++ > 5) ///NuberOfTimesObserve=5
            {
                std::cout<<"Cancelling Observe..."<<std::endl;
                //OCStackResult result = curResource->cancelObserve();// ((resource1->handler)->DM_Resource)->cancelobserve()
                ((Resource_list*)global_res->handler)->DM_Resource->cancelObserve();
                //std::cout << "Cancel result: "<< result <<std::endl;
                sleep(10);
                std::cout << "DONE"<<std::endl;
		k=0;
                
            }
        }
            catch(std::exception& e)
            {
                     cout << "Exception: " << e.what() << " in on_observe failed"<<endl;
            }

}



void DM_observe(Resource_info* resource1, onobserve_ptr1 call_back)  ///call_back = DM_onobserve
{
        global_on_call_back= call_back;
	global_res = resource1;
        try
        {
        cout<<resource1->name<<endl;
        ((Resource_list*)resource1->handler)->DM_Resource->observe(OBSERVE_TYPE_TO_USE, QueryParamsMap(), &onObserve);
        //call_back(DM_json);
        }
        catch(std::exception& e)
        {
        cout << "Exception: " << e.what() << " in DM_get failed" <<endl;
        }
        return;
}
