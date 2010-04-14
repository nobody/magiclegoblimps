
#include "../DataFile.h"

int main() {
    DataFile df("robot.dat", DataFile::ROBOT);

    Vector_ts<Robot*>* robots = new Vector_ts<Robot*>;

    boost::asio::ip::tcp::endpoint e;
    Robot* rob = new Robot(e, 65535);
    rob->setXCord(7);
    rob->setYCord(15);
    rob->setVideoURL("10.10.1.100");

    Robot* rob2 = new Robot(e, 32767);
    rob2->setXCord(31);
    rob2->setYCord(3);
    rob2->setVideoURL("10.10.1.101");

    robots->push_back(rob);
    robots->push_back(rob2);

    df.write(robots);

    delete rob;
    delete rob2;

    delete robots;


    robots = (Vector_ts<Robot*>*)df.read();


    Vector_ts<Robot*>::iterator it;
    Vector_ts<Robot*>::iterator it_end = robots->end();
    
    for (it = robots->begin(); it < it_end; ++it) {
        std::cout << "Got Robot:\n";
        std::cout << "  RID: " << (*it)->getRID() << "\n";
        std::cout << "  X:   " << (*it)->getXCord() << "\n";
        std::cout << "  Y:   " << (*it)->getYCord() << "\n";
        std::cout << "  URL: " << (*it)->getVideoURL() << "\n";
        std::cout << "\n";

        delete (*it);
    }

    delete robots;


    DataFile dfo("object.dat", DataFile::OBJECT);

    Vector_ts<Object*>* objs = new Vector_ts<Object*>;

    char tmp1[] = "qwertyuiopasdf";
    char tmp2[] = "qwertyuiopasdf";
    char tmp3[] = "qwertyuiopasdf";
    char tmp4[] = "qwertyuiopasdf";
    char tmp5[] = "qwertyuiopasdf";
    char tmp6[] = "qwertyuiopasdf";
    char tmp7[] = "qwertyuiopasdf";
    Object* obj1 = new Object(1, "Object 1", tmp1, 13);
    Object* obj2 = new Object(2, "Object 2", tmp2, 13);
    Object* obj3 = new Object(3, "Object 3", tmp3, 13);
    Object* obj4 = new Object(4, "Object 4", tmp4, 13);
    Object* obj5 = new Object(5, "Object 5", tmp5, 13);
    Object* obj6 = new Object(6, "Object 6", tmp6, 13);
    Object* obj7 = new Object(15, "Object 15", tmp7, 13);

    objs->push_back(obj1);
    objs->push_back(obj2);
    objs->push_back(obj3);
    objs->push_back(obj4);
    objs->push_back(obj5);
    objs->push_back(obj6);
    objs->push_back(obj7);

    dfo.write(objs);

    delete obj1;

    delete objs;

    objs = (Vector_ts<Object*>*)dfo.read();

    Vector_ts<Object*>::iterator ito; 
    Vector_ts<Object*>::iterator ito_end = objs->end(); 

    for (ito = objs->begin(); ito < ito_end; ++ito) {
        std::cout << "Got Object:\n"
                  << "  OID:   " << (*ito)->getOID() << "\n"
                  << "  name:  " << (*ito)->getName() << "\n";

        long size = (*ito)->getColorsize();
        std::cout << "  color_s: " << size << "\n";

        char* color = (*ito)->getColor();

        std::cout << "  color: " << std::hex;

        for (long i = 0; i < size; ++i)
            std::cout << (short)color[i];

        std::cout << std::dec << "\n";

        delete (*ito);

    }

    delete objs;

    return 0;
}
