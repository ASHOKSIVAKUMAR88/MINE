#include"client.h"
int selection(void)
{
	int choice;
	cout<<"1.GET"<<endl<<"2.PUT"<<endl<<"3.Register"<<endl<<"4.POST"<<endl<<"5.init again"<<endl;
	cout<<"enter Your choice"<<endl;
	cin>>choice;
	switch(choice)
	{
		case 1: getLightRepresentation(curResource);
			break;
		case 2: putLightRepresentation(curResource);
			break;
		case 3: curResource->observe(OBSERVE_TYPE_TO_USE, QueryParamsMap(), &onObserve);
			break;
		case 4: postLightRepresentation(curResource);
			break;
		case 5: init();
			break;
		default:
			cout<<"Invalid selection choose again"<<endl;
			return 1;
			cv.notify_all();
			cv1.notify_all();
		}//switch
		return 0;
				
			}//selection()
int main()
{
 std::cout.setf(std::ios::boolalpha);
if(init()==0);
else
exit(0);
while(1)
{	
	//cv.wait(lock, []{return false;});
	//cout<<"before lock"<<endl;
	unique_lock<std::mutex> Lock(blocker);
	cv.wait(Lock);	
	while(selection());
	//cout<<"good"<<endl;
	
		}//while(1)
return 0;
}//main()

