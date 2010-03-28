/*
 * Object.cpp
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef OBJECT_H_
#define OBJECT_H_

class Object {
    public:
        Object(int id, std::string name, char* color, size_t colorsize)
            : objID_(id), name_(name), colorsize_(colorsize)
        {
            color_ = new char[colorsize];
            while(colorsize){
                colorsize--;
                color_[colorsize] = color[colorsize];
            }
        }

        ~Object()
        {
            delete color_;
        }

        std::string name()
        {
            return name_;
        }
        void set_name(std::string name)
        {
            name_ = name;
        }

        size_t colorsize()
        {
            return colorsize_;
        }
        char* color()
        {
            return color_;
        }
        int objID(){
            return objID_;
        }

    private:
        int objID_;
        std::string name_;
        char* color_;
        size_t colorsize_;

};

#endif /* OBJECT_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
