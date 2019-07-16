//
//  Video_Frame_buffer.cpp
//  OpenLive
//
//  Created by zhanxiaochao on 2019/7/5.
//  Copyright © 2019年 Agora. All rights reserved.
//

#include "Video_Frame_buffer.h"
    
    const I420BufferInterface* VideoFrameBuffer::GetI420() const {
        // Overridden by subclasses that can return an I420 buffer without any
        // conversion, in particular, I420BufferInterface.
        return nullptr;
    }
    VideoFrameBuffer::Type I420BufferInterface::type() const {
        return Type::kI420;
    }
    
    int I420BufferInterface::ChromaWidth() const {
        return (width() + 1) / 2;
    }
    
    int I420BufferInterface::ChromaHeight() const {
        return (height() + 1) / 2;
    }
    
    rtc::scoped_refptr<I420BufferInterface> I420BufferInterface::ToI420() {
        return this;
    }

    const I420BufferInterface* I420BufferInterface::GetI420() {
       return this;
    }

