#include"stdio.h"
#include <string.h>
#include <stdlib.h>

#define BITSPERWORD 32
#define SHIFT 5
#define MASK 0x1F
#define N 900000000
#define MAX 0x7FFFFFFF
#define PAGE_SIZE  (1<<12)
#define BLOCK_SIZE  (1<<9)

void set(int a[], unsigned long  i); 
//void clr(int a[], int i);
int test(int a[], unsigned long  i);

int write[1 + N/BITSPERWORD ];
int read[1 + N/BITSPERWORD ];

int main(int argc, char *argv[]){

	if (argc != 2) {
		fprintf(stderr, "usage: %s <trace file>\n",
				argv[0]);
		exit(1);
	}
	FILE *tracefile = fopen(argv[1],"r");
	if(tracefile==NULL){
		fprintf(stderr, "can't find the file:%s\n",argv[1]);
		exit(1);
	}
	int write_num = 0;     // the number of write-request
	int read_num = 0;     // the number of read-request
	unsigned long sum_size  = 0;    // the total size of all request
	unsigned long min_address = MAX;    //  the min lbn of all request
	unsigned long max_address = 0;    // the max lbn of all request
	unsigned long sum_write_address = 0;  // the size of write-request address:
	unsigned long sum_read_address = 0;  // the size of read-request address:	
	float  working_set ;
	char line[201];
	double time = 0.0;
	int devno = 0;
	unsigned long logical_block_number;
	int size;
	int isread;
	unsigned long i;
	memset(write,0,sizeof(write));
	memset(read,0,sizeof(read));
	fgets(line,200,tracefile);
	while(!feof(tracefile)){
		sscanf(line, "%lf %d %ld %d %d", &time, &devno, &logical_block_number,&size, &isread);
		if(isread ==1){   // the request is read
			read_num++;
			for(i= logical_block_number;i<logical_block_number+size;i=i+8){
				set(read,i);
			}
		}
		else{            // the request is write
			write_num++;
			for(i= logical_block_number;i<logical_block_number+size;i=i+8){
				set(write,i);
			}
		}
		if((logical_block_number+size)>max_address)
			max_address = logical_block_number + size;
			
		if(logical_block_number<min_address)
			min_address = logical_block_number;
		sum_size += size;
		fgets(line,200,tracefile);
	}
	for(i=0;i<N;i++){
		if(test(write,i))
			sum_write_address++;
		if(test(read,i))
			sum_read_address++;
	}
	working_set = max_address*1.0/(1<<20)*BLOCK_SIZE/(1<<10);
		
	// write down the results
	FILE *fout = fopen("result.out","a");
	//fout = stdout;
	//fprintf(fout, "=============  %s  ===========\n", argv[1]);
	fprintf(fout,"%s\t",argv[1]);
	//fprintf(fout, "total of request: %d\n", write_num+read_num);
	//fprintf(fout, "%d\t", write_num+read_num);
	//fprintf(fout, "write ratio : %.2f%%\n",write_num/((write_num+read_num)*1.0)*100);
	
	fprintf(fout, "%d\t", write_num);     // write requests
	fprintf(fout, " %d\t", read_num);     // read requests
	fprintf(fout, "%.2f%%\t",write_num/((write_num+read_num)*1.0)*100);    // write ratio
	fprintf(fout, " %0.2f\t", sum_size/((write_num+read_num)*1.0));  //average size of requests
	fprintf(fout, "%ld\t", max_address-min_address);   // max address - min address
	fprintf(fout, "%ld\t", sum_write_address);   // unique write
	fprintf(fout, "%ld\t", sum_read_address);   // unique read
	
	//fprintf(fout, "%d\t",write_num);
	//fprintf(fout, "total of page read : %d\n",read_num);
	//fprintf(fout, "the average size of all request : %0.2f\n", sum_size/((write_num+read_num)*1.0));
	//fprintf(fout, "the address of all request is from %ld to %ld, and the size is %ld\n",min_address,max_address,max_address-min_address);
	//fprintf(fout, "total of unique page writes: %ld\n", sum_write_address);
	//fprintf(fout, "%ld\t", sum_write_address);
	//fprintf(fout, "the size of read-request address:  %ld\n", sum_read_address);
	//fprintf(fout, "%.2fGB\t", working_set);
	//fprintf(fout, "working set: %.2fGB\n", working_set);
	//fprintf(fout, "proportion of accessed pages: %.2f%%", sum_write_address/(max_address*1.0)*PAGE_SIZE/BLOCK_SIZE*100);
	//fprintf(fout, "%.2f%%\n", sum_write_address/(max_address*1.0)*PAGE_SIZE/BLOCK_SIZE*100);
	//fprintf(fout, "\n=============  end  =============\n\n");	
	fprintf(fout, "%0.2f\t",working_set);
	fprintf(fout, "%.2f%%\t", sum_write_address/(max_address*1.0)*PAGE_SIZE/BLOCK_SIZE*100);
	fprintf(fout, "%.2f%%\n", sum_read_address/(max_address*1.0)*PAGE_SIZE/BLOCK_SIZE*100);
	fclose(fout);
	
	return 0;
	
}

void set(int a[], unsigned long i){ 
	a[i>>SHIFT] |=  (1<<(i & MASK));	
}

//void clr(int a[], int i){
//	a[i>>SHIFT]  &=  ~(1<<(i & MASK));
//}

int test(int a[], unsigned long  i){
	return a[i>>SHIFT] & (1<<(i & MASK));
}
	
