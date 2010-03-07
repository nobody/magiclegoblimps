 /* Vector_ts.h 
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
*/
#ifndef VECTOR_TS_H_
#define VECTOR_TS_H_
#include <vector>
#include <boost/thread/mutex.hpp>

template <class T> class Vector_ts{
    private:
    boost::mutex vectorMutex;
    std::vector<T>* theVector;

    
    public:
    //type defs
    typedef typename std::vector<T>::iterator iterator;

    Vector_ts(){
        theVector = new std::vector<T>();
    }

    ~Vector_ts(){
        delete theVector;
    }
    
    int size(){
        return theVector.size();
    }

    T &operator[](int index){
        return theVector[index];
    }

    iterator begin(){
        typename std::vector<T>::iterator it;
        it = theVector.begin();
        return it;
    }

    iterator erase(iterator position){
        return theVector.erase(position);
    }

    void lock(){
        vectorMutex.lock();
    }

    void unlock(){
        vectorMutex.unlock();
    }

};
#endif

/*vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4:*/

