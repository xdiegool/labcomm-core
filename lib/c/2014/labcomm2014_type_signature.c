#include "labcomm2014.h"
#include <string.h>   // for memcmp
#include <stdio.h>   // for debug printf

/* Dump signature bytes on stdout 
 */

void labcomm2014_signature_print(struct labcomm2014_signature_data *signature)
{
  struct labcomm2014_signature_data *p = signature ;
  while (p->length != -1) {
    if (p->length) {
      int i;
      for ( i = 0 ; i < p->length ; i++) {
        printf("%02x ", p->u.bytes[i]);
      }
    } else if(p->u.signature){
      labcomm2014_signature_print(p->u.signature->treedata);
    } else {
      printf("neither data nor ref, bailing out.\n");
      return;
    }
    p+=1;
  }
  printf("\n");
}
static labcomm2014_bool sig_dump_checked(struct labcomm2014_signature_data *signature, 
                            char *buf, int *len, int buflen);

/* buf (out)   : byte array to write signature into
   len (in/out): input: buf size, out: signature length

   return TRUE if aborted due to overrun
 */
labcomm2014_bool labcomm2014_signature_dump(struct labcomm2014_signature_data *signature, 
                           char *buf, int *len)
{
  int buflen = *len;
  *len = 0;
  return sig_dump_checked(signature, buf, len, buflen);
}

/* internal function with bounds checking for buf.
 * buflen: capacity of buf
 */
static labcomm2014_bool sig_dump_checked(struct labcomm2014_signature_data *signature, 
                            char *buf, int *len, int buflen)
{
  struct labcomm2014_signature_data *p = signature;
  while ( (p->length != -1) && (*len < buflen)) {
    if (p->length) {
      int i;
      for ( i = 0 ; (i < p->length) && (*len < buflen); i++) {
        *buf++ = p->u.bytes[i];
        ++*len;
      }
    } else if(p->u.signature){
      int tmplen=*len;
      //recursing. c.f. dump()
      sig_dump_checked(p->u.signature->treedata, buf, len, buflen);
      int inner_len = *len-tmplen;
      buf += inner_len;
    } else {
      //printf("neither data nor ref, bailing out.\n");
      return TRUE;
    }
    p+=1;
  }
  return (*len >= buflen);
}

/* compare signature (flattened, if needed) to other
   return TRUE if equal
*/
labcomm2014_bool labcomm2014_signature_cmp( struct labcomm2014_signature_data *s1,
                           struct labcomm2014_signature_data *s2)
{
  int buflen=512;
  char buf1[buflen];
  int len1=buflen;
  char buf2[buflen];
  int len2=buflen;
  labcomm2014_bool res1 = labcomm2014_signature_dump(s1, buf1, &len1);
  labcomm2014_bool res2 = labcomm2014_signature_dump(s2, buf2, &len2);
  if(res1 || res2) {
    printf("WARNING: OVERRUN\n");
    return FALSE;
  } else {
    return(len1 == len2 && memcmp(buf1, buf2, len1)==0);
  }
}
/* maps a function f(char b, struct labcomm2014_signature *s, void *context) 
 * on each byte (or type ref) in the signature. 
 *
 * If flatten, the signature is flattened to a byte array, and the 
 * second argument to f is always zero.
 *
 * Otherwise, when a type ref is encountered, f is called with the first
 * argument zero and the referenced type as the second argument.
 *
 * The context parameter is passed on, unaltered, to f
 */ 
void map_signature( void(*f)(char, const struct labcomm2014_signature *, void *), 
                    void *context,
                    const struct labcomm2014_signature *signature, labcomm2014_bool flatten)
{
  struct labcomm2014_signature_data* p = signature->treedata;
  while (p->length != -1) {
    //fprintf(stderr, "%p %x\n", p, p->length);
    if (p->length) {
      int i;
      for ( i = 0 ; i < p->length ; i++) {
        (*f)(p->u.bytes[i], 0, context);
      }
    } else if (p->u.signature) {
      if(p->u.signature == 0) printf("p->u.signature == null\n");
      if(flatten) {
        map_signature(f, context, p->u.signature, flatten);
      } else {
        (*f)(0, p->u.signature, context);
      }
    } else {
      fprintf(stderr, "neither data nor ref, bailing out.\n");
      return;
    }
    p+=1;
  }
}
