/*******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2023, Jean-David Gadina - www.xs-labs.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the Software), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#ifndef CV_LOCK_HPP
#define CV_LOCK_HPP

#include <mutex>
#include <condition_variable>

class CVLock
{
    public:

        CVLock(): _locked( false )
        {}

        CVLock( const CVLock & )              = delete;
        CVLock & operator =( const CVLock & ) = delete;

        void lock()
        {
            wait:

            {
                std::unique_lock l( this->_mtx );

                this->_cv.wait
                (
                    l,
                    [ this ]
                    {
                        return this->_locked == false;
                    }
                );
            }

            if( this->_mtx.try_lock() )
            {
                std::lock_guard l( this->_mtx, std::adopt_lock );

                if( this->_locked )
                {
                    goto wait;
                }
                
                this->_locked = true;
            }
            else
            {
                goto wait;
            }
        }

        void unlock()
        {
            {
                std::lock_guard l( this->_mtx );

                this->_locked = false;
            }

            this->_cv.notify_all();
        }

    private:

        bool                    _locked;
        std::mutex              _mtx;
        std::condition_variable _cv;
};

#endif /* CV_LOCK_HPP */
