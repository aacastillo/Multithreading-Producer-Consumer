#include <ProducerConsumer.h>
#include <ctime>

// TODO: add BoundedBuffer, locks and any global variables here
BoundedBuffer * buffer = new BoundedBuffer(5);
int num_items = 0;
int consumed_items = 0;

int pro_usleep = 0;
int con_usleep = 0;
int tot_items = 0;
fstream file;

pthread_mutex_t num_lock;
pthread_mutex_t print_lock;
pthread_mutex_t consumer_lock;


void InitProducerConsumer(int p, int c, int psleep, int csleep, int items) {
  // TODO: constructor to initialize variables declared
  //       also see instructions on the implementation
  srand((unsigned) time(0));
  pro_usleep = psleep * 1000;
  con_usleep = csleep * 1000;
  tot_items = items;

  pthread_mutex_init(&num_lock, NULL);
  pthread_mutex_init(&print_lock, NULL);
  pthread_mutex_init(&consumer_lock, NULL);

  int rc;

  file.open("output.txt", fstream::out | fstream::trunc);
  cout<<"opened file\n";

  //create threads
  pthread_t producer_threads[p];
  for (int i=0; i<p; i++) {
    int * id = &i;
    rc = pthread_create(&(producer_threads[i]), NULL, producer, (void*)id);
    if (rc) {
      cout << "error: unable to create producer thread" << endl;
      exit(-1);
    }
  }

  pthread_t consumer_threads[c];
  for (int j=0; j<c; j++) {
    int * id = &j;
    rc = pthread_create(&consumer_threads[j], NULL, consumer, (void*) id);
    if (rc) {
      cout << "error: unable to create consumer thread" << endl;
      exit(-1);
    }
  }

  //join threads
  for (int i=0; i<p; i++) {
    rc = pthread_join(producer_threads[i], NULL);
    if (rc) {
      cout << "Error:unable to join," << rc << endl;
      exit(-1);
    }
  }

  for (int j=0; j<c; j++) {
    rc = pthread_join(consumer_threads[j], NULL);
    if (rc) {
      cout << "Error:unable to join," << rc << endl;
      exit(-1);
    }
  }
}

/*
Producers
Producers need to access num_items with mutual exclusion and exit when num_items is full
*/

void* producer(void* threadID) {
  // TODO: producer thread, see instruction for implementation
  pthread_mutex_lock(&num_lock);
  while (num_items < tot_items) {
    num_items += 1;
    int cur_thread_items = num_items;
    pthread_mutex_unlock(&num_lock);

    usleep(pro_usleep);
    int num_val = rand() % 10;
    
    buffer->append(num_val);

    pthread_mutex_lock(&print_lock);
    file << "Producer #" << *((int*) threadID) << ", time = " << time(nullptr) << ", producing data item #"
<< cur_thread_items << ", item value=" << num_val << "\n";
    pthread_mutex_unlock(&print_lock);
  }
  pthread_mutex_unlock(&num_lock);
  return NULL;
}

void* consumer(void* threadID) {
  // TODO: consumer thread, see instruction for implementation
    
  pthread_mutex_lock(&consumer_lock);
  while (num_items <= tot_items && consumed_items < tot_items) {
    consumed_items += 1;
    pthread_mutex_unlock(&consumer_lock);

    usleep(con_usleep);
    
    int num_val = buffer->remove();

    pthread_mutex_lock(&print_lock);
    file << "Consumer #" << *((int*) threadID) << ", time = " << time(nullptr) << ", consuming data item with value= " << num_val << "\n";
    pthread_mutex_unlock(&print_lock);
  }
  pthread_mutex_unlock(&consumer_lock);
  
  return NULL;
}
