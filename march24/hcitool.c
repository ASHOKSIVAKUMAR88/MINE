/*
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2014  Google Inc.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>
#include <pthread.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#include "lib/uuid.h"

//#include "monitor/mainloop.h"
#include "src/shared/util.h"
#include "src/shared/att.h"
#include "src/shared/queue.h"
#include "src/shared/gatt-db.h"
#include "src/shared/gatt-client.h"

#define ATT_CID 4

#define PRLOG(...) \
	printf(__VA_ARGS__); print_prompt();

#define COLOR_OFF	"\x1B[0m"
#define COLOR_RED	"\x1B[0;91m"
#define COLOR_GREEN	"\x1B[0;92m"
#define COLOR_YELLOW	"\x1B[0;93m"
#define COLOR_BLUE	"\x1B[0;94m"
#define COLOR_MAGENTA	"\x1B[0;95m"
#define COLOR_BOLDGRAY	"\x1B[1;30m"
#define COLOR_BOLDWHITE	"\x1B[1;37m"

static bool verbose = false;

struct client {
	int fd;
	struct gatt_db *db;
	struct bt_gatt_client *gatt;
};
pthread_t tid;
void mtu_request(int fd);
void read_by_grp_type(int fd);


static int l2cap_le_att_connect(bdaddr_t *src, bdaddr_t *dst, uint8_t dst_type,
									int sec)
{
	int sock;
	struct sockaddr_l2 srcaddr, dstaddr;
	struct bt_security btsec;
    printf("%s \n",__func__);
	if (verbose) {
		char srcaddr_str[18], dstaddr_str[18];

		ba2str(src, srcaddr_str);
		ba2str(dst, dstaddr_str);

		printf("btgatt-client: Opening L2CAP LE connection on ATT "
					"channel:\n\t src: %s\n\tdest: %s\n",
					srcaddr_str, dstaddr_str);
	}

	sock = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (sock < 0) {
		perror("Failed to create L2CAP socket");
		return -1;
	}

	/* Set up source address */
	memset(&srcaddr, 0, sizeof(srcaddr));
	srcaddr.l2_family = AF_BLUETOOTH;
	srcaddr.l2_cid = htobs(ATT_CID);
	srcaddr.l2_bdaddr_type = 0;
	bacpy(&srcaddr.l2_bdaddr, src);

	if (bind(sock, (struct sockaddr *)&srcaddr, sizeof(srcaddr)) < 0) {
		perror("Failed to bind L2CAP socket");
		close(sock);
		return -1;
	}

	/* Set the security level */
	memset(&btsec, 0, sizeof(btsec));
	btsec.level = sec;
	if (setsockopt(sock, SOL_BLUETOOTH, BT_SECURITY, &btsec,
							sizeof(btsec)) != 0) {
		fprintf(stderr, "Failed to set L2CAP security level\n");
		close(sock);
		return -1;
	}

	/* Set up destination address */
	memset(&dstaddr, 0, sizeof(dstaddr));
	dstaddr.l2_family = AF_BLUETOOTH;
	dstaddr.l2_cid = htobs(ATT_CID);
	dstaddr.l2_bdaddr_type = dst_type;
	bacpy(&dstaddr.l2_bdaddr, dst);

	printf("Connecting to device...");
	fflush(stdout);

	if (connect(sock, (struct sockaddr *) &dstaddr, sizeof(dstaddr)) < 0) {
		perror(" Failed to connect");
		close(sock);
		return -1;
	}

	printf(" Done\n");

	return sock;
}

/*void mtu_request(int fd)
{
	unsigned char wpdu[512];
	unsigned char rpdu[512];
	unsigned int len=0;
	unsigned int i;
	unsigned int size=0;
    printf("%s \n",__func__);
	
	wpdu[0] = BT_ATT_OP_MTU_REQ;
    wpdu[1]=0x12;
    wpdu[2]=0x20;
	
	size=write(fd,wpdu,3);
	printf("write=%d\n",size);
    while(len >= 0)
	{
	len=read(fd,rpdu,100);
	printf("len=%d\n",len);
	for(i=0;i<len;i++)
	printf(" rpdu[%d]=%.2X",i,rpdu[i]);
    }
printf("\n");

}*/
void read_by_grp_type(int fd)
{
	unsigned char wpdu[512];
	unsigned char rpdu[512];
	unsigned int len=0;
	unsigned int i;
	unsigned int size=0;
    printf("%s \n",__func__);
	printf("\n");
	wpdu[0] = BT_ATT_OP_READ_BY_GRP_TYPE_REQ;
	wpdu[1]=0x01;
	wpdu[2]=0x00;
	wpdu[3]=0xff;
	wpdu[4]=0xff;
	wpdu[5]=0x00;
	wpdu[6]=0x28;
	size=write(fd,wpdu,7);
	printf("size=%d\n",size);
	
	len=0;
	while(len == 0)
	{
	len=read(fd,rpdu,100);
	printf("len=%d\n",len);
	for(i=0;i<len;i++)
	printf(" rpdu[%d]=%.2X\n",i,rpdu[i]);
    }
}
void *read_data(void *pfd)
{
    unsigned char rpdu[512];
    int len;
    int i;
    long int fd=(long int )pfd;
    printf("%s \n",__func__);
    while(1)
    {
    len=read(fd,rpdu,512);
    printf("len=%d\t",len);
    for(i=0;i<len;i++)
    printf("rpdu[%d]=%.2X\n",i,rpdu[i]);
    }
    return 0;
  }
void write_data1(void * pfd)
{
int len=0;
int i;
long int fd=(long int )pfd;
printf("%s \n",__func__);
unsigned char wpdu[512];
unsigned char rpdu[512];
rpdu[0]=BT_ATT_OP_WRITE_REQ;
rpdu[1]=0x00;
rpdu[2]=0x10;
rpdu[3]=0x01;
rpdu[4]=0x00;
len=write(fd,rpdu,5);
printf("notification bit enabling is succes ===>%d\n",len);
}
void write_data2(void* pfd)
{

    int len=0;
    int i;
    long int fd=(long int )pfd;
    printf("%s \n",__func__);
    unsigned char wpdu[512];
    unsigned char rpdu[512];

    wpdu[0]=BT_ATT_OP_WRITE_CMD;
    wpdu[1]=0x81;
    wpdu[2]=0x00;
    wpdu[3]=0x00;
    wpdu[4]=0x00;
    wpdu[5]=0x00;
    wpdu[6]=0x19;
    wpdu[7]=0xD8;
    wpdu[8]=0x01;
    wpdu[9]=0xFC;
    wpdu[10]=0x90;
    wpdu[11]=0xAE;
    wpdu[12]=0xB3;
    wpdu[13]=0xFF;
    wpdu[14]=0xA0;
    wpdu[15]=0xCE;
    wpdu[16]=0xAB;
    wpdu[17]=0xB3;
    wpdu[18]=0x6F;
    wpdu[19]=0x69;
    wpdu[20]=0x01;
    wpdu[21]=0x00;
    wpdu[22]=0x63;
    wpdu[23]=0x03;
    wpdu[24]=0x72;
    wpdu[25]=0x65;
    wpdu[26]=0x73;
    wpdu[27]=0x61;
    wpdu[28]=0x3C;
    wpdu[29]=0xE1;
    wpdu[30]=0xFE;
    wpdu[31]=0xD6;
    wpdu[32]=0xC0;



        len=write(fd,wpdu,33);

         //  len=write(fd,wpdu,13);
           while(1);

}
int main(int argc, char *argv[])
{
	int sec = BT_SECURITY_LOW;
	uint8_t dst_type = BDADDR_LE_PUBLIC;
	bdaddr_t src_addr, dst_addr;
	int dev_id = -1;
	int fd;

			if (str2ba(argv[2], &dst_addr) < 0) {
				perror("Invalid remote address:\n");
				return EXIT_FAILURE;
			}
			
	if (dev_id == -1)
		bacpy(&src_addr, BDADDR_ANY);
	else if (hci_devba(dev_id, &src_addr) < 0) {
		perror("Adapter not available");
		return EXIT_FAILURE;
	}

	fd = l2cap_le_att_connect(&src_addr, &dst_addr, dst_type, sec);
	if (fd < 0)
		return EXIT_FAILURE;
    //init(fd);

        write_data1(fd);
        write_data2(fd);

    //read_by_grp_type(fd);
        //pthread_join(tid,NULL);
        //while(1);
return 0;
}
