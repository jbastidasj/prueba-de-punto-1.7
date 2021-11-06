#include <stdio.h>
#include <math.h>
#include "lcgrand.h"

#define Q_LIMIT1 100
#define Q_LIMIT2 10
#define Q_LIMIT3 20
#define Q_LIMIT4 30

#define BUSY    1
#define IDLE    0


int   next_event_type,num_events,
     num_custs_delayed1, num_in_q1, server_status1,
     num_custs_delayed2, num_in_q2, server_status2,
     num_custs_delayed3, num_in_q3, server_status3,
     num_custs_delayed4, num_in_q4, server_status4,
     num_values_demand;


float mean_interarrival, sim_time, time_end, time_last_event, time_next_event[10],
     time_arrival1[Q_LIMIT1 + 1], mean_service1, total_of_delays1, area_num_in_q1, area_server_status1,
     time_arrival2[Q_LIMIT2 + 1], mean_service2, total_of_delays2, area_num_in_q2, area_server_status2,
     time_arrival3[Q_LIMIT3 + 1], mean_service3, total_of_delays3, area_num_in_q3, area_server_status3,
     time_arrival4[Q_LIMIT4 + 1], mean_service4, total_of_delays4, area_num_in_q4, area_server_status4,
     prob_distrib_demand[26];
FILE  *infile, *outfile;

void  initialize(void);
void  timing(void);
void  arrive1(void);
void  depart1(void);
void  arrive2(void);
void  depart2(void);
void  arrive3(void);
void  depart3(void);
void  arrive4(void);
void  depart4(void);
void  report(void);
void  update_time_avg_stats(void);
float expon(float mean);

main(){
   int i;
   infile  = fopen("mm1alt.in",  "r");
   outfile = fopen("mm1alt.out", "w");
   num_events = 9;
   fscanf(infile, "%f %f %f %f %f %f %d", &mean_interarrival, &mean_service1, &mean_service2, &mean_service3, &mean_service4, &time_end, &num_values_demand);
   for (i = 1; i <= num_values_demand; ++i)
       fscanf(infile, "%f", &prob_distrib_demand[i]);
   fprintf(outfile, "Mean interarrival time %11.3f minutes\n", mean_interarrival);
   fprintf(outfile, "Mean service1 time %16.3f minutes\n", mean_service1);
   fprintf(outfile, "Mean service2 time %16.3f minutes\n", mean_service2);
   fprintf(outfile, "Mean service3 time %16.3f minutes\n", mean_service3);
   fprintf(outfile, "Mean service4 time %16.3f minutes\n", mean_service4);
   fprintf(outfile, "Length of the simulation %9.3f minutes\n\n", time_end);
   initialize();
   do {
       timing();
       update_time_avg_stats();
       switch (next_event_type) {
           case 1:
               arrive1();
               break;
           case 2:
               depart1();
               break;
           case 3:
               arrive2();
               break;
           case 4:
               depart2();
               break;
           case 5:
               arrive3();
               break;
           case 6:
               depart3();
               break;
           case 7:
               arrive4();
               break;
           case 8:
               depart4();
               break;
           case 9:
               report();
               break;
       }
   } while (next_event_type != 9);
   fclose(infile);
   fclose(outfile);
   return 0;
}

void initialize(void){
   sim_time = 0.0;
   time_last_event = 0.0;
   time_next_event[1] = sim_time + expon(1/mean_interarrival);
   time_next_event[2] = 1.0e+30;
   time_next_event[3] = 1.0e+30;
   time_next_event[4] = 1.0e+30;
   time_next_event[5] = 1.0e+30;
   time_next_event[6] = 1.0e+30;
   time_next_event[7] = 1.0e+30;
   time_next_event[8] = 1.0e+30;
   time_next_event[9] = time_end;

   server_status1   = IDLE;
   num_in_q1        = 0;
   num_custs_delayed1  = 0;
   total_of_delays1    = 0.0;
   area_num_in_q1      = 0.0;
   area_server_status1 = 0.0;

   server_status2   = IDLE;
   num_in_q2        = 0;
   num_custs_delayed2  = 0;
   total_of_delays2    = 0.0;
   area_num_in_q2      = 0.0;
   area_server_status2 = 0.0;

   server_status3   = IDLE;
   num_in_q3        = 0;
   num_custs_delayed3  = 0;
   total_of_delays3    = 0.0;
   area_num_in_q3      = 0.0;
   area_server_status3 = 0.0;

   server_status4   = IDLE;
   num_in_q4        = 0;
   num_custs_delayed4  = 0;
   total_of_delays4    = 0.0;
   area_num_in_q4      = 0.0;
   area_server_status4 = 0.0;
}

void timing(void){
   int   i;
   float min_time_next_event = 1.0e+29;
   next_event_type = 0;
   for (i = 1; i <= num_events; ++i)
       if (time_next_event[i] <= min_time_next_event) {
           min_time_next_event = time_next_event[i];
           next_event_type     = i;
       }
   if (next_event_type == 0) {
       fprintf(outfile, "\nEvent list empty at time %f", sim_time);
       exit(1);
   }
   sim_time = min_time_next_event;
}

void arrive1(void){
   float delay;
   time_next_event[1] = sim_time + expon(1/mean_interarrival);
   if (server_status1 == BUSY) {
       ++num_in_q1;
       if (num_in_q1 > Q_LIMIT1) {
           fprintf(outfile, "\nOverflow of the array time_arrival1 at");
           fprintf(outfile, " time %f", sim_time);
           exit(2);
       }
       time_arrival1[num_in_q1] = sim_time;
   }
   else {
       delay            = 0.0;
       total_of_delays1 += delay;
       ++num_custs_delayed1;
       server_status1 = BUSY;
       time_next_event[2] = sim_time + expon(mean_service1);
   }
}

void depart1(void){
   int   i;
   float delay;
   if (num_in_q1 == 0) {
       server_status1      = IDLE;
       time_next_event[2] = 1.0e+30;
   }
   else {
       --num_in_q1;
       delay            = sim_time - time_arrival1[1];
       total_of_delays1 += delay;
       ++num_custs_delayed1;
       time_next_event[2] = sim_time + expon(mean_service1);
       for (i = 1; i <= num_in_q1; ++i)
           time_arrival1[i] = time_arrival1[i + 1];
   }
   switch(random_integer(prob_distrib_demand)){
       case 1:
           time_next_event[3] = sim_time;
           break;
       case 2:
           time_next_event[5] = sim_time;
           break;
       case 3:
           time_next_event[7] = sim_time;
           break;
   }
}

void arrive2(void){
   time_next_event[3] = 1.0e+30;
   float delay;
   if (server_status2 == BUSY) {
       ++num_in_q2;
       if (num_in_q2 > Q_LIMIT2) {
           fprintf(outfile, "\nOverflow of the array time_arrival2 at");
           fprintf(outfile, " time %f", sim_time);
           exit(2);
       }
       time_arrival2[num_in_q2] = sim_time;
   }
   else {
       delay            = 0.0;
       total_of_delays2 += delay;
       ++num_custs_delayed2;
       server_status2 = BUSY;
       time_next_event[4] = sim_time + expon(mean_service2);
   }
}

void depart2(void){
   int   i;
   float delay;
   if (num_in_q2 == 0) {
       server_status2      = IDLE;
       time_next_event[4] = 1.0e+30;
   }
   else {
       --num_in_q2;
       delay            = sim_time - time_arrival2[1];
       total_of_delays2 += delay;
       ++num_custs_delayed2;
       time_next_event[4] = sim_time + expon(mean_service2);
       for (i = 1; i <= num_in_q2; ++i)
           time_arrival2[i] = time_arrival2[i + 1];
   }
   time_next_event[5] = sim_time;
}

void arrive3(void){
   time_next_event[5] = 1.0e+30;
   float delay;
   if (server_status3 == BUSY) {
       ++num_in_q3;
       if (num_in_q3 > Q_LIMIT3) {
           fprintf(outfile, "\nOverflow of the array time_arrival3 at");
           fprintf(outfile, " time %f", sim_time);
           exit(2);
       }
       time_arrival3[num_in_q3] = sim_time;
   }
   else {
       delay            = 0.0;
       total_of_delays3 += delay;
       ++num_custs_delayed3;
       server_status3 = BUSY;
       time_next_event[6] = sim_time + expon(mean_service3);
   }
}

void depart3(void){
   int   i;
   float delay;
   if (num_in_q3 == 0) {
       server_status3      = IDLE;
       time_next_event[6] = 1.0e+30;
   }
   else {
       --num_in_q3;
       delay            = sim_time - time_arrival3[1];
       total_of_delays3 += delay;
       ++num_custs_delayed3;
       time_next_event[6] = sim_time + expon(mean_service3);
       for (i = 1; i <= num_in_q3; ++i)
           time_arrival3[i] = time_arrival3[i + 1];
   }
   time_next_event[7] = sim_time;
}

void arrive4(void){
   time_next_event[7] = 1.0e+30;
   float delay;
   if (server_status4 == BUSY) {
       ++num_in_q4;
       if (num_in_q4 > Q_LIMIT4) {
           fprintf(outfile, "\nOverflow of the array time_arrival4 at");
           fprintf(outfile, " time %f", sim_time);
           exit(2);
       }
       time_arrival4[num_in_q4] = sim_time;
   }
   else {
       delay            = 0.0;
       total_of_delays4 += delay;
       ++num_custs_delayed4;
       server_status4 = BUSY;
       time_next_event[8] = sim_time + expon(mean_service4);
   }
}

void depart4(void){
   int   i;
   float delay;
   if (num_in_q4 == 0) {
       server_status4      = IDLE;
       time_next_event[8] = 1.0e+30;
   }
   else {
       --num_in_q4;
       delay            = sim_time - time_arrival4[1];
       total_of_delays4 += delay;
       ++num_custs_delayed4;
       time_next_event[8] = sim_time + expon(mean_service4);
       for (i = 1; i <= num_in_q4; ++i)
           time_arrival4[i] = time_arrival4[i + 1];
   }
}

void report(void){
   fprintf(outfile, "Average delay in queue1%11.3f minutes\n",
           total_of_delays1/num_custs_delayed1);
   fprintf(outfile, "Average number in queue1%10.3f\n",
           area_num_in_q1);
   fprintf(outfile, "Server1 utilization%15.3f\n",
           area_server_status1 / sim_time);
   fprintf(outfile, "Number of delays1 completed%7d",
           num_custs_delayed1);

   fprintf(outfile, "\n\nAverage delay in queue2%11.3f minutes\n",
           total_of_delays2 / num_custs_delayed2);
   fprintf(outfile, "Average number in queue2%10.3f\n",
           area_num_in_q2);
   fprintf(outfile, "Server2 utilization%15.3f\n",
           area_server_status2 / sim_time);
   fprintf(outfile, "Number of delays2 completed%7d",
           num_custs_delayed2);

   fprintf(outfile, "\n\nAverage delay in queue3%11.3f minutes\n",
           total_of_delays3 / num_custs_delayed3);
   fprintf(outfile, "Average number in queue3%10.3f\n",
           area_num_in_q3);
   fprintf(outfile, "Server3 utilization%15.3f\n",
           area_server_status3 / sim_time);
   fprintf(outfile, "Number of delays3 completed%7d",
           num_custs_delayed3);

   fprintf(outfile, "\n\nAverage delay in queue4%11.3f minutes\n",
           total_of_delays4 / num_custs_delayed4);
   fprintf(outfile, "Average number in queue4%10.3f\n",
           area_num_in_q4 / sim_time);
   fprintf(outfile, "Server4 utilization%15.3f\n",
           area_server_status4 / sim_time);
   fprintf(outfile, "Number of delays4 completed%7d",
           num_custs_delayed4);
}

void update_time_avg_stats(void){
   float time_since_last_event;
   time_since_last_event = sim_time - time_last_event;
   time_last_event       = sim_time;

   area_num_in_q1      += num_in_q1 * time_since_last_event;
   area_server_status1 += server_status1 * time_since_last_event;

   area_num_in_q2      += num_in_q2 * time_since_last_event;
   area_server_status2 += server_status2 * time_since_last_event;

   area_num_in_q3      += num_in_q3 * time_since_last_event;
   area_server_status3 += server_status3 * time_since_last_event;

   area_num_in_q4      += num_in_q4 * time_since_last_event;
   area_server_status4 += server_status4 * time_since_last_event;
}

float expon(float mean){
   return -mean * log(lcgrand(1));
}

int random_integer(float prob_distrib[]){
   int   i;
   float u;
   u = lcgrand(1);
   for (i = 1; u >= prob_distrib[i]; ++i)
       ;
   return i;
}
