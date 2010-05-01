/*
 * Object.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include "Point.h"
#include "Robot.h"
#include <boost/thread/mutex.hpp>

class Object {
    public:
        //Robot that is viewing this object
        Robot* viewedFrom;

        Object(int id, std::string name, char* color, int colorsize, char* box, int boxsize)
            : objID_(id), name_(name), colorsize_(colorsize), boxsize_(boxsize)
        {
            pos = Point(-1,-1);
            color_ = new char[colorsize];
            viewedFrom = NULL;
            while(colorsize){
                colorsize--;
                color_[colorsize] = color[colorsize];
            }
            box_= new char[boxsize];
            while(boxsize){
                boxsize--;
                box_[boxsize] = box[boxsize];
            }
        }
    Point pos;
        ~Object()
        {
            delete[] color_;
        }

        int getOID() {
            return objID_;
        }

        std::string getName()
        {
            return name_;
        }
        void setName(std::string name)
        {
            name_ = name;
        }

        int getColorsize()
        {
            return colorsize_;
        }
        char* getColor()
        {
            return color_;
        }

        int getBoxsize()
        {
            return boxsize_;
        }
        char* getBox()
        {
            return box_;
        }

        void setColor(char* c, long size) {
            if (size < 1) {
                delete color_;
                color_ = NULL;
                return;
            }
            if (color_)
                delete[] color_;
            color_ = new char[size];
            memcpy(color_, c, size);
        }

        Robot* getViewedFrom() {
            return viewedFrom;
        }

        void setViewedFrom(Robot* r) {
            viewedFrom = r;
        }
        void setBox(char* b, long size) {
            if (size < 1) {
                delete box_;
                box_ = NULL;
                return;
            }
            if (box_)
                delete[] box_;
            box_ = new char[size];
            memcpy(box_, b, size);
        }
        inline bool operator<(Object& obj)const{
            return objID_ < obj.getOID();
        }

        void lock(){
            mutex.lock();
        }

        void unlock(){
            mutex.unlock();
        }

    private:
        int objID_;
        std::string name_;
        char* color_;
        int colorsize_;
        char* box_;
        int boxsize_;
        boost::mutex mutex;

};

#endif /* OBJECT_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
