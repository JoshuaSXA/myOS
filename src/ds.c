#include "common.h"
#include "ds.h"

u8int kernel_queue_is_empty(struct kernel_queue *kq) {
	if (kq->kq_in == -1 || kq->kq_out == -1) {
		return 1;
	}

    return 0;
}

u8int kernel_queue_is_full(struct kernel_queue *kq) {
    s16int diff = kq->kq_in - kq->kq_out;

    diff = (diff < 0)? -diff : diff;

    diff += 1;

	if (diff == 2 || diff == kq->kq_max_limit) {
		return 1;
	}

    return 0;
}

u8int kernel_queue_pop(struct kernel_queue *kq) {
	u8int return_data;

	if (kq->is_kq_empty(kq)) {
		return 1;
	}

	if(kq->kq_in == kq->kq_out) {
		return_data = kq->data[kq->kq_out];
		kq->kq_in = kq->kq_out = -1;
	}
	else {
		return_data = kq->data[kq->kq_out];
		kq->kq_out = (kq->kq_out + 1) & 0x1FFF;
	}

    return return_data;
}

u8int kernel_queue_push(struct kernel_queue *kq, u8int data) {
	if (kq->is_kq_full(kq)) {
		return 1;
	}
    
	if(kq->is_kq_empty(kq)) {
		kq->kq_in = kq->kq_out = 0;
	}
	else {
		kq->kq_in = (kq->kq_in + 1) & 0x1FFF;
	}

	kq->data[kq->kq_in] = data;

	return 0;
}

void init_kernel_queue(struct kernel_queue *kq) {    
    kq->kq_max_limit = 500; // TODO : Make it argument, if needed
    kq->kq_in = kq->kq_out = -1;
    kq->is_kq_full = &kernel_queue_is_full;
    kq->is_kq_empty = &kernel_queue_is_empty;
    kq->kq_pop = kernel_queue_pop;
    kq->kq_push = kernel_queue_push;
}


