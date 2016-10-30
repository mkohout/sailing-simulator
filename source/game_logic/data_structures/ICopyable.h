#pragma once

#include <memory>

using std::shared_ptr;

namespace Game_Logic
{
    /**
     * Interface to ensure that the class
     * that is inheriting implements a consistent
     * function to copy its main values
     */
    template <class T>
    class ICopyable
    {
    public:
        virtual shared_ptr<T> VCopy() = 0;
    };
}