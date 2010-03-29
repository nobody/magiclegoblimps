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
        Object(int id, std::string name, char* color, int colorsize)
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

        long getColorsize()
        {
            return colorsize_;
        }
        char* getColor()
        {
            return color_;
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
            memcpy(color_,  c, size);
        }
        inline bool operator<(Object& obj)const{
            return objID_ < obj.getOID();
        }

    private:
        int objID_;
        std::string name_;
        char* color_;
        int colorsize_;

};

#endif /* OBJECT_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
