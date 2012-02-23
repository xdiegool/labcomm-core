#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
//#include <linux/if_ether.h>
//#include <linux/if_packet.h>
//#include <linux/if_arp.h>
#include <net/if.h>
#include <netpacket/packet.h>
//#include <netinet/in.h>
#include "ethernet_drv.h"
#include "display.h"


/** LOCAL FUNCTIONS **/

/* local type for the ethernet interface */
struct eth_int_t
{
	struct ether_addr mac_adr;        /* MAC address                */
	int socket_id;                    /* socket file descriptor     */
	int index;                        /* index of the eth interface */
	unsigned char validity;
};

/***
Open an Ethernet Raw packet connection
***/
struct eth_int_t* eth_open(const char* eth_int_name)
{
   struct eth_int_t* tmp_eth = NULL; /* pointer to ethernet interface struct */
   struct ifreq ifr;                 /* struct used by ioctl function        */

   /* TO-DO: check if a ethernet socket has already been created for eth_int_name interface */

   /* Ethernet interface name pointer valid ? */
   if (NULL == eth_int_name)
   {
      errno = EINVAL;
      DISPLAY_ERR("Input parameter NULL");
   }
   else
   {
      /* allocate memory for the Ethernet Interface */
      tmp_eth = (struct eth_int_t*)malloc(sizeof(struct eth_int_t));
      if (NULL == tmp_eth)
      {
   	    DISPLAY_ERR("Error allocating memory!");
      }
      else
      {
   	   /* creates a Packet Socket, raw delivery, accept all protocol */
         tmp_eth->socket_id = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
         if (-1 == tmp_eth->socket_id) /* error occurred during socket creation ? */
         {
            free(tmp_eth); /* deallocate the memory */
            tmp_eth = NULL;
            DISPLAY_ERR("Error in creating socket");
         }
         else
         {
            /* retrieve the Ethernet card MAC address */
            strcpy(ifr.ifr_name, eth_int_name);
            if (ioctl(tmp_eth->socket_id, SIOCGIFHWADDR, &ifr) == -1) /* error during the retrieve of  MAC address ? */
            {
               close(tmp_eth->socket_id); /* close the socket      */
               free(tmp_eth);             /* deallocate the memory */
               tmp_eth = NULL;
               DISPLAY_ERR("ioctl error SIOCGIFHWADDR");
            }
            else
            {
               /* copy the MAC address into the eth interface struct */
               memcpy(tmp_eth->mac_adr.ether_addr_octet, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

               /*retrieve the Ethernet interface index*/
               if (ioctl(tmp_eth->socket_id, SIOCGIFINDEX, &ifr) == -1)/* error during the retrieve of index */
               {
                  close(tmp_eth->socket_id); /* close the socket      */
                  free(tmp_eth);             /* deallocate the memory */
                  tmp_eth = NULL;
                  DISPLAY_ERR("ioctl error SIOCGIFINDEX");
               }
               else
               {
                  /* copy the interface index into the eth interface struct */
                  tmp_eth->index = ifr.ifr_ifindex;
                  printf("Successfully got our MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                          tmp_eth->mac_adr.ether_addr_octet[0],tmp_eth->mac_adr.ether_addr_octet[1],tmp_eth->mac_adr.ether_addr_octet[2],tmp_eth->mac_adr.ether_addr_octet[3],tmp_eth->mac_adr.ether_addr_octet[4],tmp_eth->mac_adr.ether_addr_octet[5]);
                  printf("Successfully got interface index for %s: %i\n",eth_int_name, tmp_eth->index);
               }
            }
         }
      }
   }
   return(tmp_eth);
}

/***
Close the Ethernet Raw packet connection
***/
int eth_close(struct eth_int_t* eth_int)
{
   int ret = 0;

   /* Ethernet interface pointer valid ? */
   if (NULL == eth_int)
   {
      ret = -1;
      errno = EINVAL;
      DISPLAY_ERR("Input parameter NULL");
   }
   else
   {
      if (-1 == close(eth_int->socket_id)) /* error during the closure of the socket ? */
      {
         ret = -1;
         DISPLAY_ERR("Socket closure error");
      }
      else
      {
         free(eth_int);  /* deallocate the memory */
         eth_int = NULL;
      }
   }
   return(ret);
}

/***
Returns the MAC address of the Ethernet Interface
***/
int eth_getMACadr(const struct eth_int_t* eth_int, struct ether_addr * mac_adr)
{
   int ret = 0;

   if ((NULL == eth_int) || (NULL == mac_adr))  /* Input parameters valid ? */
   {
      ret = -1;
      errno = EINVAL;
      DISPLAY_ERR("Input parameter(s) NULL");
   }
   else
   {
      /* TO-DO: introduce a validity flag to be check before accessing to eth_int (better a crc) */
      memcpy(mac_adr, &eth_int->mac_adr, sizeof(mac_adr));
   }
   return(ret);
}


/***
send a complete Ethernet Raw Packet
***/
int eth_send(const struct eth_int_t* eth_int, const unsigned char* eth_frame, unsigned short length)
{
   struct ethhdr *eth_header = NULL;  /* Pointer to Ethernet frame header (Dest,Source,Type) */
   struct sockaddr_ll socket_address;
   int ret;

   if ((NULL == eth_int) || (NULL == eth_frame))
   {
      ret = -1;
      errno = EINVAL;
      DISPLAY_ERR("Input parameter(s) Null!");
   }
   else
   {
      eth_header = (struct ethhdr*)eth_frame;

      /*prepare sockaddr_ll (address structure for PACKET_SOCKET) */
      socket_address.sll_family   = AF_PACKET;
      socket_address.sll_addr[0]  = eth_header->h_dest[0];
      socket_address.sll_addr[1]  = eth_header->h_dest[1];
      socket_address.sll_addr[2]  = eth_header->h_dest[2];
      socket_address.sll_addr[3]  = eth_header->h_dest[3];
      socket_address.sll_addr[4]  = eth_header->h_dest[4];
      socket_address.sll_addr[5]  = eth_header->h_dest[5];
      socket_address.sll_addr[6]  = 0x00; /* not used */
      socket_address.sll_addr[7]  = 0x00; /* not used */
      socket_address.sll_halen    = ETH_ALEN;         /* Length of the MAC address         */
      socket_address.sll_ifindex  = eth_int->index;   /* Ethernet Interface index          */
      // The rest should be zero for sending, and are set by the system for receiving.
      socket_address.sll_hatype   = 0;            
      socket_address.sll_protocol = 0;
      socket_address.sll_pkttype  = 0;

      //socket_address.sll_protocol = htons(ETH_P_IP);  /* Physical layer protocol           */
      //socket_address.sll_hatype   = ARPHRD_ETHER;     /* ARP hardware identifier: Ethernet */
      //socket_address.sll_pkttype  = PACKET_OTHERHOST; /* Packet type: Another host         */

      /*send the Ethernet frame */
      ret = sendto(eth_int->socket_id, eth_frame, length, 0, (struct sockaddr*)&socket_address, sizeof(socket_address));
      if (ret == -1) /* error during the trasmission of the Ethernet frame ? */
      {
         DISPLAY_ERR("sendto error!");
      }
   }
   return(ret);
}

/***
receive a complete Ethernet Raw Packet
***/
int eth_receive (const struct eth_int_t* eth_int, unsigned char* eth_frame, unsigned short length)
{
   int ret;

   if ((NULL == eth_int) || (NULL == eth_frame))
   {
      ret = -1;
      errno = EINVAL;
      DISPLAY_ERR("Input parameter(s) Null!");
   }
   else
   {
      /*Wait for incoming packet...*/
      ret = recvfrom(eth_int->socket_id, (void*)eth_frame, length, 0, NULL, NULL);
      if (ret == -1) /* error during the reception of the Ethernet frame ? */
      {
         DISPLAY_ERR("recvfrom error!");
      }
   }
   return(ret);
}



