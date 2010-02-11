package magiclegoblimps;

import java.awt.geom.Point2D.Double;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.util.*;

class MLBController{
    MLBFrame f = null;
    ArrayList<MovableEntity> items = new ArrayList<MovableEntity>();
    javax.swing.Timer t = null;
    HashMap<ObjectOfInterest,Point2D.Double> lastSeen = new HashMap<ObjectOfInterest,Point2D.Double>();
    HashMap<Robot,ObjectOfInterest> assignments = new HashMap<Robot,ObjectOfInterest>();
    ObjectOfInterest monkey;
    ObjectOfInterest giraffe;
    public MLBController(MLBFrame mlbf){
        f = mlbf;

        //Give the entities list to the map panel for drawing
        mlbf.mp.me = items;
        mlbf.mp.ls = lastSeen;
        mlbf.mp.as = assignments;

        //Robots
        Robot r1 = new Robot("Robot_1");
        Robot r2 = new Robot("Robot_2");

        //PoI
        monkey = new ObjectOfInterest("Monkey");
        giraffe = new ObjectOfInterest("Giraffe");

        //Add to entity list
        items.add(monkey);
        items.add(giraffe);
        items.add(r1);
        items.add(r2);

        //Assign cameras to objects
        assignments.put(r1, monkey);
        assignments.put(r2,giraffe);

        t = new javax.swing.Timer(25,new ActionListener(){

            public void actionPerformed(ActionEvent e) {
                tick();
            }
        });
        t.start();
        //simulate(r1);
    }
    public void simulate(Robot r){
        r.go();
        r.move();
        r.swivelLeft();
    }
    public void tick(){

        for(MovableEntity o : items){ //Move everything
            o.move();
        }
        for(Robot r : assignments.keySet()){//For each robot, if r can see an object, update it's position
            for(MovableEntity me : items){
                if(me instanceof ObjectOfInterest){
                    ObjectOfInterest o = (ObjectOfInterest)me;
                    if(r.canSee(o.pos)){
                        lastSeen.put(o,(Point2D.Double)o.pos.clone());
                    }
                }
            }
        }
        for(Robot r : assignments.keySet()){
            r.goTo(lastSeen.get(assignments.get(r)));
            r.centerOnObject(lastSeen.get(assignments.get(r)),assignments.get(r));

            for(ObjectOfInterest o : lastSeen.keySet()){
                if(r.canSee(lastSeen.get(o))&&!r.canSee(o.pos)){
                    lastSeen.put(o, null);
                }
            }
        }



        f.mp.repaint();
        
    }
    public int avg(int a, int b, int c){
        return (a + b + c) / 3;
    }
}
