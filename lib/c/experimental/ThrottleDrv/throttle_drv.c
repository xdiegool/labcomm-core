#include "throttle_drv.h"
#include "ethernet_drv.h"
#include <errno.h>
#include "display.h"
#include "stdlib.h"
#include <string.h>
#include <time.h>
// #include <arpa/inet.h>

#define THROTTLENET_PROTO 0x544e

#define THR_DST_ADR_POS      0
#define THR_SRC_ADR_POS      (THR_DST_ADR_POS  + sizeof(((thr_header_t*)0)->dst_adr))             //6
#define THR_ETH_TYP_POS      (THR_SRC_ADR_POS  + sizeof(((thr_header_t*)0)->src_adr))             //12
#define THR_CHN_ID_POS       (THR_ETH_TYP_POS  + sizeof(((thr_header_t*)0)->eth_type))            //14
#define THR_FRAG_NUM_POS     (THR_CHN_ID_POS   + sizeof(((thr_header_t*)0)->chn_id))              //15
#define THR_FRAG_TOT_NUM_POS (THR_FRAG_NUM_POS + sizeof(((thr_header_t*)0)->frag_num))            //17
#define THR_FRAG_LEN_POS     (THR_FRAG_TOT_NUM_POS + sizeof(((thr_header_t*)0)->frag_num_tot))    //19
#define THR_PAYLOAD_POS      (THR_FRAG_LEN_POS + sizeof(((thr_header_t*)0)->frag_len))            //21
#define THR_MSG_HEADER_LEN   THR_PAYLOAD_POS

#define THR_MSG_DST_ADR(thr_msg)       (struct ether_addr*)(&thr_msg[THR_DST_ADR_POS])
#define THR_MSG_SRC_ADR(thr_msg)       (struct ether_addr*)(&thr_msg[THR_SRC_ADR_POS])
#define THR_MSG_ETH_TYP(thr_msg)       *(unsigned short*)(&thr_msg[THR_ETH_TYP_POS])
#define THR_MSG_CHN_ID(thr_msg)        *(unsigned char*)(&thr_msg[THR_CHN_ID_POS])
#define THR_MSG_FRAG_NUM(thr_msg)      *(unsigned short*)(&thr_msg[THR_FRAG_NUM_POS])
#define THR_MSG_FRAG_TOT_NUM(thr_msg)  *(unsigned short*)(&thr_msg[THR_FRAG_TOT_NUM_POS])
#define THR_MSG_FRAG_LEN(thr_msg)      *(unsigned short*)(&thr_msg[THR_FRAG_LEN_POS])
#define THR_MSG_PAYLOAD(thr_msg)       (unsigned char*)(&thr_msg[THR_PAYLOAD_POS])


/* local type for the Throttle Channel structure */
struct thr_chn_t
{
	struct ether_addr dst_adr;         /* destination MAC address                    */
	unsigned char id;                  /* Channel id                                 */
	unsigned short frag_size;          /* Fragment size                              */
	unsigned short freq;               /* Message transmission frequency             */
	thr_msg_handler_t funct;           /* Callback function invoked at the reception */
	unsigned short msg_length;
	unsigned char* p_msg;
	struct ether_addr last_sender_adr;/* src MAC address of last message */
};

/* Type for the Throttle message */
typedef struct
{
   struct ether_addr dst_adr;  /* destination MAC address  */
   struct ether_addr src_adr;  /* source MAC address       */
   unsigned short eth_type;              /* ethernet packet type     */
   unsigned char  chn_id;                /* channel identification   */
   unsigned short frag_num;              /* fragment number          */
   unsigned short frag_num_tot;          /* total fragment number    */
   unsigned short frag_len;              /* fragment length          */
}thr_header_t;

/* Local functions */
static int thr_msg_check(const unsigned char* thr_msg, unsigned short chn_id, unsigned short length);

static struct eth_int_t* eth_int = NULL;

/**
* @fn         int thr_init(const char* eth_int_name)
*
* @brief      Initializes the Throttle Net Driver, enabling the Ethernet Raw communication for the eth_int_name interface.
*
* @param [in] eth_int_name: ethernet interface name (e.g. "eth0","eth1")
*
* @retval     int: 0 on success; -1 on error and errno is set appropriately
*
*******************************************************************************/
int thr_init(const char* eth_int_name)
{
   int ret = 0;

   if (NULL == eth_int_name)
   {
      ret = -1;
      errno = EINVAL;
      DISPLAY_ERR("Input parameter NULL");
   }
   else
   {
      eth_int = eth_open(eth_int_name); /* open the Ethernet socket */
      if (NULL == eth_int)
      {
         ret = -1;
      }
   }
   return(ret);
}

/**
* @fn         struct thr_chn_t* thr_open_chn(const char* dst_adr, unsigned char chn_id, unsigned char frag_size, unsigned short freq, const thr_msg_handler_t funct);
*
* @brief      Open a logic channel
*
* @param [in] dst_adr: destination MAC address;
* @param [in] chn_id: identification of the channel;
* @param [in] frag_size: fragment size;
* @param [in] freq: frequency of trasmission for each fragment;
* @param [in] msg_handler: callback invoked when a message is completely received;
*
* @retval     struct thr_chn_t*: pointer to the Channel structure. NULL on error and errno is set appropriately.
*
*******************************************************************************/
struct thr_chn_t* thr_open_chn(const struct ether_addr* dst_adr, unsigned char chn_id, unsigned char frag_size, unsigned short freq, const thr_msg_handler_t funct)
{
   struct thr_chn_t* tmp_chn = NULL; /* pointer to Channel structure */

   /* TO-DO: check if a similar channel has already been created */

   /* Input parameters not valid ? */
   if ((NULL == dst_adr) || (0 == frag_size))
   {
      errno = EINVAL;
      DISPLAY_ERR("Input parameter(s) NULL");
   }
   else
   {
      /* allocate memory for the Channel structure */
      tmp_chn = (struct thr_chn_t*)malloc(sizeof(struct thr_chn_t));
      if (NULL == tmp_chn)
      {
         DISPLAY_ERR("Error allocating memory!");
      }
      else
      {
         /* Update the Channel structure */
         memcpy(&tmp_chn->dst_adr, dst_adr, sizeof(tmp_chn->dst_adr));
         tmp_chn->id = chn_id;
         tmp_chn->frag_size = frag_size;
         tmp_chn->freq = freq;
         tmp_chn->funct = funct;
      }
#ifdef DEBUG
      printf("thr_open_chn: callback = %x\n", tmp_chn->funct);
#endif

   }

   return(tmp_chn);
}

void thr_close_chn(struct thr_chn_t* c){
   free(c);
}

/**
* @fn         int thr_send(const struct thr_chn_t* thr_chn, const char* data, unsigned int length)
*
* @brief      Sends a Throttle message. A channel must be opened first.
*
* @param [in] thr_chn: pointer to the Throttle channel descriptor;
* @param [in] data: data buffer to be sent;
* @param [in] length: length of the data buffer;
*
* @retval     int: number of byte sent, on success. -1 on error and errno is set appropriately.
*
*******************************************************************************/
int thr_send(const struct thr_chn_t* thr_chn, const char* data, unsigned int length)
{
   unsigned char* thr_msg;
   unsigned char i;
   int ret = 0;

   /* Check the input parameters */
   if ((NULL == thr_chn) || (NULL == data) || (0 == length))
   {
      ret = -1;
      errno = EINVAL;
      DISPLAY_ERR("Input parameter(s) NULL");
   }
   else
   {
      if (0 == thr_chn->frag_size)
      {
         ret = -1;
         errno = ERANGE;
         DISPLAY_ERR("Division by zero");
      }
      else
      {
         /* allocate memory for the Throttle Message */
         thr_msg = (unsigned char*)malloc(THR_MSG_HEADER_LEN + thr_chn->frag_size);
         if (NULL == thr_msg)
         {
            ret = -1;
            DISPLAY_ERR("Error allocating memory!");
         }
         else
         {
            /* Compose the Ethernet Frame to be sent */
            memcpy(THR_MSG_DST_ADR(thr_msg), &thr_chn->dst_adr, sizeof(thr_chn->dst_adr)); /* Destiantion MAC Address  */
            eth_getMACadr(eth_int, THR_MSG_SRC_ADR(thr_msg));                        /* Source MAC Address       */
            THR_MSG_ETH_TYP(thr_msg) = htons(THROTTLENET_PROTO);                     /* Ethernet Packet Type     */
            THR_MSG_CHN_ID(thr_msg) = thr_chn->id;                                   /* Channel identification   */
            THR_MSG_FRAG_TOT_NUM(thr_msg) = ((length - 1) / thr_chn->frag_size) + 1; /* Total number of fragment */

            struct timespec thr_time;
            thr_time.tv_sec = thr_chn->freq / 1000;
            thr_time.tv_nsec = (thr_chn->freq % 1000) * 1000000;

            /* Message is splitted into fragments and they are sent */
            for (i = 1; i <= THR_MSG_FRAG_TOT_NUM(thr_msg); i++)
            {
               THR_MSG_FRAG_NUM(thr_msg) = i; /* fragment number */
               /* update the fragment length */
               if (length >= thr_chn->frag_size)
               {
                  THR_MSG_FRAG_LEN(thr_msg) = thr_chn->frag_size;
                  length -= thr_chn->frag_size;
               }
               else
               {
                  THR_MSG_FRAG_LEN(thr_msg) = length;
               }
               memcpy ((void*)THR_MSG_PAYLOAD(thr_msg), (void*)data, THR_MSG_FRAG_LEN(thr_msg));                /* update the payload */
               ret = eth_send(eth_int, &thr_msg[0], (THR_MSG_HEADER_LEN + THR_MSG_FRAG_LEN(thr_msg))); /* send the message   */
               if (-1 == ret)  /*Error during the Ethernet trasmission ? */
               {
                  DISPLAY_ERR("Error during Throttle msg trasmission!");
                  break;
               }
               else
               {
                  data += THR_MSG_FRAG_LEN(thr_msg);
                  nanosleep(&thr_time, NULL);
               }
            }
            /*deallocate the memory */
            free(thr_msg);
            thr_msg = NULL;
         }
      }
   }
   return(ret);
}

int thr_receive(struct thr_chn_t* thr_chn, unsigned char* data, void* param)
{
   unsigned char* thr_msg = NULL;
   unsigned char* p_data = NULL;
   unsigned short frag_index = 1;
   short byte_rec = 0;
   unsigned char msg_received = 0;
   int ret;

   if ((NULL == thr_chn) || (NULL == data) || (NULL == thr_chn->funct) || (NULL == param))
   {
      ret = -1;
      errno = EINVAL;
      DISPLAY_ERR("Input parameter(s) NULL");
   }
   else
   {
      /* allocate memory for the Throttle Message */
      thr_msg = (unsigned char*)malloc(THR_MSG_HEADER_LEN + thr_chn->frag_size);
      if (NULL == thr_msg)
      {
         ret = -1;
         DISPLAY_ERR("Error allocating memory!");
      }
      else
      {
         p_data = data;
         do
         {
            byte_rec = eth_receive(eth_int, thr_msg, (THR_MSG_HEADER_LEN + thr_chn->frag_size));  /* receive the Ethernet Raw packet */
            if (-1 == byte_rec)
            {
               /* discard the message */
               DISPLAY_ERR("Error during Throttle msg reception: Fragment discarded.");
            }
            else
            {
               /* check if the Ethernet Raw message is correct (Ethernet protocol, Channel ID, length) */
               ret = thr_msg_check(thr_msg, thr_chn->id, byte_rec);
               if (ret < 0)
               {
                  //printf("Throttle msg mismatch: Fragment discarded.\n");
               }
               else
               {
#ifdef DEBUG
                  printf("thr_receive: Message Index %d on %d. Actual Index %d\n", THR_MSG_FRAG_NUM(thr_msg), THR_MSG_FRAG_TOT_NUM(thr_msg), frag_index);
#endif
                  if (frag_index == THR_MSG_FRAG_NUM(thr_msg)) /* The fragment is the one expected ? */
                  {
                     /* Rebuild the original data linking the payloads of each fragment */
                     memcpy((void*)p_data, (void*)THR_MSG_PAYLOAD(thr_msg), THR_MSG_FRAG_LEN(thr_msg));
                     p_data += THR_MSG_FRAG_LEN(thr_msg);  /* update the pointer to the buffer   */
                     frag_index++;                /* update the fragment index          */
                     ret = p_data - data;         /* update the number of received byte */
                     if (frag_index > THR_MSG_FRAG_TOT_NUM(thr_msg))
                     {
                        msg_received = 1;
                     }
                  }
                  else
                  {
                     printf("thr_receive: Fragment mismatch: Fragment discarded.\n");
                     frag_index = 1;
                     p_data = data;
                  }
               }
            }
         }while (msg_received != 1);

         thr_chn->p_msg = (unsigned char*)malloc(ret);
         if (NULL == thr_chn->p_msg)
         {
            ret = -1;
            DISPLAY_ERR("Error allocating memory!");
         }
         else
         {
#ifdef DEBUG
            printf("thr_receive: Number of byte receive %d\n",ret);
#endif
            thr_chn->msg_length = ret;
            memcpy(thr_chn->p_msg, data, ret); /* copy the msg into the thr structure */
            memcpy(&thr_chn->last_sender_adr, THR_MSG_SRC_ADR(thr_msg), 6); /* ... and the address of the sender */
#ifdef DEBUG
            printf("thr_receive: calling %x\n", thr_chn->funct);
#endif
            (thr_chn->funct)(param);
            free(thr_chn->p_msg);
            thr_chn->p_msg = NULL;
         }

         /* deallocate memory for the Throttle Message */
         free(thr_msg);
         thr_msg = NULL;
         p_data = NULL;
      }
   }
   return(ret);
}


static int thr_msg_check(const unsigned char* thr_msg, unsigned short chn_id, unsigned short length)
{
   int ret = 0;

   if (length > THR_MSG_HEADER_LEN)                    /* Ethernet Raw Packet contains a valid Payload ? */
   {
      if (THROTTLENET_PROTO == ntohs(THR_MSG_ETH_TYP(thr_msg))) /* Is Ethernet Type THROTTLENET Protocol ?        */
      {
         if (THR_MSG_CHN_ID(thr_msg) == chn_id)                 /* Is Channel identification correct ?            */
         {
            /* throttle message is correct */
         }
         else
         {
            ret = -1;
         }
      }
      else
      {
         /* discard the message */
         ret = -2;
      }
   }
   else
   {
      /* discard the message */
      ret = -3;
   }
   return(ret);
}


int thr_read(struct thr_chn_t* thr_chn, unsigned char* data, int length)
{
   int ret = 0;

   if ((NULL == thr_chn) || (NULL == data))
   {
      ret = -1;
      errno = EINVAL;
      DISPLAY_ERR("Input parameter(s) NULL");
   }
   else
   {
      if (length > thr_chn->msg_length)
      {
         length = thr_chn->msg_length;
#ifdef DEBUG
         printf("thr_read: truncating length to %d\n", length);
#endif
      }
#ifdef DEBUG
      printf("thr_read: calling memcpy(%x, %x, %d\n", data, thr_chn->p_msg, length);
#endif
      memcpy(data, thr_chn->p_msg, length); /* copy the msg into the thr structure */
      ret = length;
      thr_chn->msg_length = 0;
   }
   return(ret);
}

struct ether_addr* get_sender_addr(struct thr_chn_t* ch) {
	return &ch->last_sender_adr;
}

unsigned char get_channel(struct thr_chn_t* ch) {
	return ch->id;
}
