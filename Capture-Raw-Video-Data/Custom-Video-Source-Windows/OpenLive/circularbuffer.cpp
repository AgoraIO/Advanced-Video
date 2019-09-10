#include "stdafx.h"
#include "CircularBuffer.hpp"
#include <algorithm>

Locker::Locker()
    : _userDesc(NULL)
{
    InitializeCriticalSection(&_lock);
}

Locker::~Locker()
{
    unlock();

    DeleteCriticalSection(&_lock);
}

void Locker::lock(const char *userDesc)
{
    EnterCriticalSection(&_lock);
    _userDesc = userDesc;
}

void Locker::unlock()
{
    _userDesc = NULL;
    LeaveCriticalSection(&_lock);
}

bool Locker::tryLock()
{
    return TryEnterCriticalSection(&_lock) != 0;
}


AutoLocker::AutoLocker(Locker *locker, const char *userDesc)
    : _locker(locker)
{
    if (_locker != NULL)
    {
#if 1
        for (int i = 0; ; i++)
        {
            if (i >= 1000 * 30)
            {
                if (nullptr != userDesc)
                {
                    printf("\nWarning AutoLocker %s i=%d", userDesc, i);
                }
                else
                {
                    printf("\nWarning AutoLocker i=%d", i);
                }
                //i = i/0;
            }

            //ASSERT(i<1000 * 30);

            if (_locker->tryLock())
            {
                break;
            }

            Sleep(1);
        }
#else
        _locker->lock(userDesc);
#endif
    }
}

AutoLocker::~AutoLocker()
{
    if (_locker != NULL)
    {
        _locker->unlock();
    }
}

CircularBuffer::CircularBuffer()
: beg_index_(0)
, end_index_(0)
, size_(0)
, capacity_(0)
,data_(NULL)
,isInit_(FALSE)
,ncount_(0)
,nlen_(0)
,pushLen_(new size_t)
{
}

CircularBuffer::~CircularBuffer()
{
	uninit();
}

BOOL CircularBuffer::init(size_t ncount)
{
	lock_.lock();
	if(isInit_)
	{
		lock_.unlock();
		return FALSE;
	}
	ncount_ = ncount;
 nlen_ = 960 * 540 * 3 / 2;
	capacity_ = nlen_ * ncount;
	data_ = new char [capacity_];
	if(NULL== data_){
		printf("malloc memcpy failed\n");
	}
	memset(data_,0,nlen_);

	isInit_ = TRUE;

	lock_.unlock();
	return TRUE;
}

BOOL CircularBuffer::init(size_t capacity,size_t ncount)
{
	lock_.lock();
	if(isInit_)
	{
		lock_.unlock();
		return FALSE;
	}
	
	nlen_ = capacity/ncount;
	ncount_ = ncount;
	capacity_ = capacity;

	data_ = new char[capacity];
	ZeroMemory(data_,capacity_);
	if(NULL == data_)
	{
		lock_.unlock();
		printf("new memory failed..\n");
		return FALSE;
	}
	isInit_ = TRUE;

	lock_.unlock();

	return TRUE;
}

BOOL CircularBuffer::uninit()
{
	lock_.lock();
	if(data_)
	{
		delete [] data_;
		data_ = NULL;
	}
	if(pushLen_)
	{
		delete pushLen_;
		pushLen_ = NULL;
	}
	isInit_ = FALSE;
	size_ = 0;
	ncount_ = 0;
	nlen_ = 0;

	lock_.unlock();

	return TRUE;
}

BOOL CircularBuffer::push(unsigned char *data, size_t bytes)
{
	lock_.lock();
	reInit(bytes * ncount_);

	if(NULL == data_){
		lock_.unlock();
		return FALSE;
	}
	if(NULL== data){
		lock_.unlock();
		return FALSE;
	}
	if (bytes == 0){
		lock_.unlock();
		return 0;
	}
 
// ASSERT(nlen_ == bytes);

	size_t bytes_to_write = min(bytes, capacity_ - size_);
	if(!bytes_to_write) bytes_to_write = bytes;

	// Write in a single step
	//desc beg_index,end_index,size before push

	if (bytes_to_write <= capacity_ - end_index_ && NULL != data)
	{
		memcpy(data_ + end_index_, data, bytes_to_write);
		end_index_ += bytes_to_write;
		if (end_index_ == capacity_) end_index_ = 0;
	}

	size_ += bytes_to_write;
	if(isFull()){
		//LOG_MSG("circular buffer is full");
	}

	if(capacity_ < size_){//assert(size_ == capacity + nlen_)
		beg_index_ += (size_ - capacity_);
		if(capacity_ == beg_index_){
			beg_index_ = 0;
		}
		size_ = capacity_;
	}
	//desc beg_index,end_index,size pushed
	lock_.unlock();
	return TRUE;
}

BOOL CircularBuffer::pop(char *data, int &bytes)
{
	lock_.lock();
	if(data_ == NULL)
	{
		lock_.unlock();
		return FALSE;
	}
	if (data == NULL)
	{
		lock_.unlock();
		return FALSE;
	}

	bytes = nlen_;
	size_t bytes_to_read = min(bytes, size_);
	if(!bytes_to_read)
	{
		//LOG_MSG("circular buffer is empty");
		lock_.unlock();
		return FALSE;
	}

	//desc beg_index,end_index,size before pop

	// Read in a single step
	if (bytes_to_read <= capacity_ - beg_index_)
	{
		memcpy(data, data_ + beg_index_, bytes_to_read);
		beg_index_ += bytes_to_read;
		if (beg_index_ == capacity_) beg_index_ = 0;
	}

	size_ -= bytes_to_read;
	//desc beg_index,end_index,size poped

	lock_.unlock();
	return TRUE;
}

BOOL CircularBuffer::pushEx(const char *data,size_t bytes)
{
	lock_.lock();
	if(NULL == data_){
		lock_.unlock();
		return FALSE;
	}
	if(NULL== data){
		lock_.unlock();
		return FALSE;
	}
	if(NULL == pushLen_)
	{
		lock_.unlock();
		return FALSE;
	}
	if (bytes == 0){
		lock_.unlock();
		return 0;
	}
 //ASSERT (bytes <= nlen_ - 4) 

	size_t bytes_to_write = min(bytes, capacity_ - size_);
	if(!bytes_to_write) bytes_to_write = bytes;

	// Write in a single step
	//desc beg_index,end_index,size before push

	if (bytes_to_write <= capacity_ - end_index_ && NULL != data)
	{
		memset(data_ + end_index_,0,nlen_);
		*pushLen_ = bytes_to_write;
		memcpy(data_ + end_index_,pushLen_,sizeof(size_t));
		int bytes = *(size_t*)(data_ + beg_index_);
		memcpy(data_ + end_index_ + sizeof(size_t), data, bytes_to_write);
		end_index_ +=/*bytes_to_write*/ nlen_;
		if (end_index_ == capacity_) end_index_ = 0;
	}

	size_ += /*bytes_to_write*/nlen_;
	if(isFull()){
		//LOG_MSG("circular buffer is full");
	}

	if(capacity_ < size_){//assert(size_ == capacity + nlen_)
		beg_index_ += (size_ - capacity_);
		if(capacity_ == beg_index_){
			beg_index_ = 0;
		}
		size_ = capacity_;
	}
	//desc beg_index,end_index,size pushed

	lock_.unlock();
	return TRUE;
}

BOOL CircularBuffer::popEx(char *data,int &bytes)
{
	lock_.lock();
	if(NULL == data_){
	lock_.unlock();
	return FALSE;
	}
	if (data == NULL)
	{
		lock_.unlock();
		return FALSE;
	}
	if(pushLen_ == NULL)
	{
		lock_.unlock();
		//LOG_MSG("pushLen mem is empty...");
		return FALSE;
	}
	if(size_ && data_ + beg_index_){
		bytes = /*nlen_*/*(size_t*)(data_ + beg_index_);
	}
	size_t bytes_to_read = min(bytes, size_);
	if(!bytes_to_read)
	{
		//LOG_MSG("circular buffer is empty");
		lock_.unlock();
		return FALSE;
	}

	//desc beg_index,end_index,size before pop

	// Read in a single step
	if (bytes_to_read <= capacity_ - beg_index_)
	{
		memcpy(data, data_ + beg_index_ + sizeof(size_t), bytes_to_read);
		beg_index_ += /*bytes_to_read*/nlen_;
		if (beg_index_ == capacity_) beg_index_ = 0;
	}

	size_ -= /*bytes_to_read*/nlen_;
	//desc beg_index,end_index,size poped

	lock_.unlock();
	return TRUE;
}

BOOL CircularBuffer::empty()
{
	return size_ == 0?true:false;
}

BOOL CircularBuffer::isFull()
{
	return size_ == capacity_?true:false;
}

BOOL CircularBuffer::isInit()
{
	return isInit_;
}

BOOL CircularBuffer::reInit(size_t capacity)
{
	lock_.lock();
	if(!isInit_){
		lock_.unlock();
		return FALSE;
	}
	
	//reinit cmp

	if(capacity_ != capacity){
		char *szbuf = new char [nlen_];
		memcpy(szbuf,data_,nlen_);
		delete [] data_;
		data_ = new char[capacity];
		size_t bytes_to_cpy = min(nlen_,capacity/ncount_);
		memcpy(data_,szbuf,bytes_to_cpy);
		delete [] szbuf;
		capacity_ = capacity;
		nlen_ = capacity_/ncount_;

		lock_.unlock();
		return TRUE;
	}
	else{
		lock_.unlock();
		//printf("already malloc mem....");
		return FALSE;
	}
}