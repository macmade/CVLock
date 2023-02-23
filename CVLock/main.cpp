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

#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include "CVLock.hpp"
#include "CVRecursiveLock.hpp"

void testLock();
void testRecursiveLock();

int main()
{
    testLock();

    std::cout << "--------------------------------------------------" << std::endl;

    testRecursiveLock();

    return 0;
}

void testLock()
{
    CVLock cvl;

    {
        cvl.lock();

        std::thread
        (
            [ & ]
            {
                std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

                std::cout << "Unlocking" << std::endl;

                cvl.unlock();
            }
        )
        .detach();

        std::cout << "Waiting" << std::endl;

        {
            std::lock_guard< CVLock > l( cvl );
        }

        std::cout << "Done" << std::endl;
    }

    {
        std::vector< std::shared_ptr< std::thread > > threads;

        for( int i = 0; i < 10; i++ )
        {
            threads.push_back
            (
                std::make_shared< std::thread >
                (
                    [ & ]
                    {
                        std::lock_guard< CVLock > l( cvl );

                        std::cout << "Locked: " << std::this_thread::get_id() << std::endl;

                        std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
                    }
                )
            );
        }

        for( const auto & t: threads )
        {
            t->join();
        }
    }
}

void testRecursiveLock()
{
    CVRecursiveLock cvl;

    {
        std::lock_guard< CVRecursiveLock > l1( cvl );
        std::lock_guard< CVRecursiveLock > l2( cvl );
    }

    {
        cvl.lock();

        std::thread
        (
            [ & ]
            {
                std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

                std::cout << "Unlocking" << std::endl;

                cvl.unlock();
            }
        )
        .detach();

        std::cout << "Waiting" << std::endl;

        std::thread
        (
            [ & ]
            {
                {
                    std::lock_guard< CVRecursiveLock > l1( cvl );
                    std::lock_guard< CVRecursiveLock > l2( cvl );
                }

                std::cout << "Done" << std::endl;
            }
        )
        .join();
    }

    {
        std::vector< std::shared_ptr< std::thread > > threads;

        for( int i = 0; i < 10; i++ )
        {
            threads.push_back
            (
                std::make_shared< std::thread >
                (
                    [ & ]
                    {
                        std::lock_guard< CVRecursiveLock > l( cvl );

                        std::cout << "Locked: " << std::this_thread::get_id() << std::endl;

                        std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
                    }
                )
            );
        }

        for( const auto & t: threads )
        {
            t->join();
        }
    }
}
