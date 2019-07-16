//
//  Video_Frame_buffer.hpp
//  OpenLive
//
//  Created by zhanxiaochao on 2019/7/5.
//  Copyright © 2019年 Agora. All rights reserved.
//

#ifndef Video_Frame_buffer_h
#define Video_Frame_buffer_h
#include <iostream>
#include <stdio.h>
#include "ref_count.h"
#include "scoped_ref_ptr.h"

class I420BufferInterface;


class VideoFrameBuffer: public rtc::RefCountInterface{
public:
    // New frame buffer types will be added conservatively when there is an
    // opportunity to optimize the path between some pair of video source and
    // video sink.
    enum class Type {
        kNative,
        kI420,
        kI420A,
        kI444,
        kI010,
    };
    
    // This function specifies in what pixel format the data is stored in.
    virtual Type type() const = 0;
    
    virtual rtc::scoped_refptr<I420BufferInterface> ToI420() = 0 ;
    
    virtual const I420BufferInterface* GetI420() const;
    // The resolution of the frame in pixels. For formats where some planes are
    // subsampled, this is the highest-resolution plane.
    virtual int width() const = 0;
    virtual int height() const = 0;
    
protected:
    ~VideoFrameBuffer(){}
};
class PlanarYuvBuffer : public VideoFrameBuffer {
public:
    virtual int ChromaWidth() const = 0;
    virtual int ChromaHeight() const = 0;
    
    // Returns the number of steps(in terms of Data*() return type) between
    // successive rows for a given plane.
    virtual int StrideY() const = 0;
    virtual int StrideU() const = 0;
    virtual int StrideV() const = 0;
    
protected:
    ~PlanarYuvBuffer()  {}
};
// This interface represents 8-bit color depth formats: Type::kI420,
// Type::kI420A and Type::kI444.
class PlanarYuv8Buffer : public PlanarYuvBuffer {
public:
    // Returns pointer to the pixel data for a given plane. The memory is owned by
    // the VideoFrameBuffer object and must not be freed by the caller.
    virtual const uint8_t* DataY() const = 0;
    virtual const uint8_t* DataU() const = 0;
    virtual const uint8_t* DataV() const = 0;
    
protected:
    ~PlanarYuv8Buffer()  {}
};

class I420BufferInterface : public PlanarYuv8Buffer {
public:
    Type type() const override;
    
    int ChromaWidth() const final;
    int ChromaHeight() const final;
    
    rtc::scoped_refptr<I420BufferInterface> ToI420() final;
    virtual const I420BufferInterface* GetI420() final;
    
protected:
    ~I420BufferInterface() {}
};

#endif /* Video_Frame_buffer_h */
