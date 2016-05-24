#include <stdlib.h> 
#include <time.h> 
#include <string.h> 

struct tm *localtime_r (const time_t *, struct tm *); 
struct tm *gmtime_r (const time_t *, struct tm *); 

struct tm * 
localtime_r (const time_t *timer, struct tm *result) 
{ 
   struct tm *local_result; 
   local_result = localtime (timer); 

   if (local_result == NULL || result == NULL) 
     return NULL; 

   memcpy (result, local_result, sizeof (result)); 
   return result; 
} 

struct tm * 
gmtime_r (const time_t *timer, struct tm *result) 
{ 
   struct tm *local_result; 
   local_result = gmtime (timer); 

   if (local_result == NULL || result == NULL) 
     return NULL; 

   memcpy (result, local_result, sizeof (result)); 
   return result; 
} 
