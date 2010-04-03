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
    //locks the vector
    boost::mutex vectorMutex;
    //locks the number of readers
    boost::mutex readMutex;
    //prevents more readers from starting while writer is waiting
    boost::mutex writerMutex;
    std::vector<T>* theVector;
    int readers;
	    
    public:
    //type defs
    typedef typename std::vector<T>::iterator iterator;

    Vector_ts(){
        theVector = new std::vector<T>();
        readers = 0;
    }

    ~Vector_ts(){
        delete theVector;
    }
    
    int size(){
        return theVector->size();
    }

    T &operator[](int index){
        return theVector[index];
    }

    T& at(size_t i) {
        return theVector->at(i);
    }

    iterator begin(){
        typename std::vector<T>::iterator it;
        it = theVector->begin();
        return it;
    }
    iterator end(){
	    typename std::vector<T>::iterator it;
	    it = theVector->end();
	    return it;
    }

    iterator erase(iterator position){
        return theVector->erase(position);
    }

    void push_back(const T& x){
	    theVector->push_back(x);
    }

    void lock(){
        
        writerMutex.lock();
        vectorMutex.lock();
        writerMutex.unlock();
    }

    void unlock(){
        vectorMutex.unlock();
    }
    void readLock(){
        writerMutex.lock();
        readMutex.lock();
        ++readers;
        if(readers == 1)
            vectorMutex.lock();
        readMutex.unlock();
        writerMutex.unlock();
    }
    void readUnlock(){
        readMutex.lock();
        --readers;
        if(readers == 0)
            vectorMutex.unlock();
        readMutex.unlock();

    }

};
#endif

/*vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4:*/

