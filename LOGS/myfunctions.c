//************To print Proxy details*********************










#define CA_GATT_SERVICE_UUID_LOWER "ade3d529-c784-4f63-a987-eb69f70ee816"
struct BLE_ADDRESS_FILTER
{
	char address[100][18];
}ble_address_filter;
int ble_address_count=0;//for IOT and NON-IOT devices count

int FILTER_ADDRESS(char *ADDRESS)
{
	int i=0;
	for(i=0;i<ble_address_count;i++)
		if(strcasecmp(ble_address_filter.address[i],ADDRESS)==0)
			return 0;
	return 1;		

}
void ADD_ADDRESS(char *ADDRESS)
{
	int i=0;
	for(i=0;i<ble_address_count;i++);
	memcpy(ble_address_filter.address[i],ADDRESS,18);
	//memcpy(ble_address_filter.address[ble_address_count],ADDRESS,18);
	ble_address_count++;
}

//*********************Differentiating IOT and NON-IOT devices code****************************
//Mostly used at caleoninterfacesadded() in caleinterface.c file
//Mostly used at CACentralConnect() in central.c file
extern int FILTER_ADDRESS(char *);
extern void ADD_ADDRESS(char *);

FUNCTION_TO_DIFFERENTIATE()
{
//********************ADDED CODE***********************s
    char * address = g_variant_print(g_dbus_proxy_get_cached_property(device,"Address"),TRUE);
    address[18]='\0';
    if(FILTER_ADDRESS(address+1)==0)
        return;
    ADD_ADDRESS(address+1);
    char uuids = g_variant_print(g_dbus_proxy_get_cached_property(device,"UUIDs"),TRUE);
    /*if (strstr(uuids, CA_GATT_SERVICE_UUID) != NULL
            ||
        strstr(uuids,"ade3d529-c784-4f63-a987-eb69f70ee816") != NULL)
            {
        printf("\nIOT Address:%s/n",address+1);
            }
           else if(strcasecmp(uuids,"@as []")==0)
             {
        printf("\nNOn-IOT Address:%s/n",address+1);
      	      }*/
if(strcasecmp(uuids,"@as []")==0)
            {
        printf("\nNon - IOT Address:%s/n",address+1);
            }
           else if (strstr(uuids, CA_GATT_SERVICE_UUID) != NULL
            ||
        strstr(uuids,"ade3d529-c784-4f63-a987-eb69f70ee816") != NULL)
             {
        printf("\nIOT Address:%s/n",address+1);
      	      }
//********************ADDED CODE***********************s

}
