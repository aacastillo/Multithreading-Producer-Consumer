#include <BoundedBuffer.h>

BoundedBuffer::BoundedBuffer(int N) {
  // TODO: constructor to initiliaze all the varibales declared in
  buffer = new int[N];
  buffer_size = N;
  buffer_cnt = 0;
  buffer_first = 0;
  buffer_last = 0;

  pthread_mutex_init(&buffer_lock, NULL);
  pthread_cond_init(&buffer_not_full, NULL);
  pthread_cond_init(&buffer_not_empty, NULL);
}

BoundedBuffer::~BoundedBuffer() {
  free(buffer);
  buffer_size = NULL;
  buffer_cnt = NULL;
  buffer_first = NULL;
  buffer_last = NULL;

  pthread_mutex_destroy(&buffer_lock);
  pthread_cond_destroy(&buffer_not_full);
  pthread_cond_destroy(&buffer_not_empty);
}

void BoundedBuffer::append(int data) {
  // TODO: append a data item to the circular buffer
  
  pthread_mutex_lock(&buffer_lock);

  while (buffer_cnt == buffer_size) { //while buffer is full
    pthread_cond_wait(&buffer_not_full, &buffer_lock); //wait for not full signal
  }

  buffer[buffer_last] = data;
  buffer_last = (buffer_last + 1) % buffer_size;
  buffer_cnt += 1;

  pthread_cond_signal(&buffer_not_empty);
  pthread_mutex_unlock(&buffer_lock);
} //this runs completely but stays in while loop in producerconsumer

int BoundedBuffer::remove() {
  // TODO: remove and return a data item from the circular buffer
  pthread_mutex_lock(&buffer_lock);
  while (buffer_cnt == 0) { //while buffer is empty
    pthread_cond_wait(&buffer_not_empty, &buffer_lock); //wait for not empty signal
  }
  
  int return_data = buffer[buffer_first];
  buffer_first = (buffer_first + 1) % buffer_size;
  buffer_cnt -= 1;

  pthread_cond_signal(&buffer_not_full);
  pthread_mutex_unlock(&buffer_lock);

  return return_data;
}

bool BoundedBuffer::isEmpty() {
  // TODO: check is the buffer is empty
  
  pthread_mutex_lock(&buffer_lock);
  bool isEmpty = true;
  if (buffer_cnt != 0) {
    isEmpty = false;
  }
  pthread_mutex_unlock(&buffer_lock);
  
  return isEmpty;
}
