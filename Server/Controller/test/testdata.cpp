
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

    char tmp[] = "qwertyuiopasdf";
    Object* obj1 = new Object(15, "Object 1", tmp, 13);

    objs->push_back(obj1);

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
