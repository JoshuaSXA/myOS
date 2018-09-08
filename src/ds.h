#ifndef DS_H
#define DS_H

/* 
   Current queues is builded only for the
   shell input 
*/ 
  
struct kernel_queue {
	u8int data[512];

	s16int kq_in;
	s16int kq_out;
    
	u16int kq_max_limit;

	u8int (*kq_pop)(struct kernel_queue *kq);
	u8int (*kq_push)(struct kernel_queue *kq, u8int data);

	u8int (*is_kq_full)(struct kernel_queue *kq);
	u8int (*is_kq_empty)(struct kernel_queue *kq);
};

extern void init_kernel_queue(struct kernel_queue *obj);

#endif
