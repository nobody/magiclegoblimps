package magiclegoblimps;


import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.util.*;

class MLBController{
    MLBFrame f = null;
    
    // items is the union of robots and objects
    ArrayList<MovableEntity> items = new ArrayList<MovableEntity>();
    ArrayList<Robot> robots = new ArrayList<Robot>();
    ArrayList<ObjectOfInterest> objects = new ArrayList<ObjectOfInterest>();
    
    javax.swing.Timer timer = null;
    HashMap<ObjectOfInterest,Point2D.Double> lastSeen = new HashMap<ObjectOfInterest,Point2D.Double>();
    HashMap<Robot,ObjectOfInterest> assignments = new HashMap<Robot,ObjectOfInterest>();
    HashMap<ObjectOfInterest,Double> demand = new HashMap<ObjectOfInterest,Double>();
    HashMap<ObjectOfInterest,Robot> viewedFrom = new HashMap<ObjectOfInterest,Robot>();
    HashMap<ObjectOfInterest,Double> QoS = new HashMap<ObjectOfInterest,Double>();
    ObjectOfInterest monkey;
    ObjectOfInterest giraffe;
    ObjectOfInterest penguin;
    ObjectOfInterest squirrel;
    
    public void initSimulation() {
        //Robots
        Robot r1 = addRobot("Robot 1");
        Robot r2 = addRobot("Robot 2");
        Robot r3 = addRobot("Robot 3");
        //PoI
        monkey = addObject("Monkey", new Color(255, 167, 50));
        giraffe = addObject("Giraffe", new Color(255, 255, 50));
        penguin = addObject("Penguin",new Color(255,0,255));
        squirrel = addObject("Squirrel",new Color(255,100,200));

        //Assign cameras to objects
        //startTracking(r1, monkey);
       // startTracking(r2, giraffe);
        //startTracking(r3, penguin);

        //demand.put(monkey,.2);
       // demand.put(giraffe,.3);
       // demand.put(penguin,.4);
        //demand.put(squirrel,.1);
    }
    
    public void runSimulation() {
        timer = new javax.swing.Timer(25, new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                tick();
            }
        });
        timer.start();
    }

    public void simulate(Robot r){
        r.go();
        r.move();
        r.swivelLeft();
    }
    
    
    public int tickCount = 0;
    public void tick() {
        //Move everything
        for(MovableEntity o : items) {
            o.move();
        }
        
        //For each robot, if r can see an object, update it's position
        for(Robot r : assignments.keySet()){
            for(MovableEntity me : items){
                if(me instanceof ObjectOfInterest){
                    ObjectOfInterest o = (ObjectOfInterest)me;
                    if(r.canSee(o.pos)){
                        lastSeen.put(o,(Point2D.Double)o.pos.clone());
                    }
                }
            }
        }
        //For each robot, tell the robot to track its assigned object
        //If a robot can see where an object is supposed to be but it is not there,
        //update the lastSeen map with null.

        for(Robot r : assignments.keySet()){
            r.goTo(lastSeen.get(assignments.get(r)));
            r.centerOnObject(lastSeen.get(assignments.get(r)),assignments.get(r));

            for(ObjectOfInterest o : lastSeen.keySet()){
                if(r.canSee(lastSeen.get(o))&&!r.canSee(o.pos)){
                    lastSeen.put(o, null);
                }
            }
        }
        tickCount++;
        
        if(tickCount%100==0){
            tickCount=0;
            optimizeQoS();
        }

        for(ObjectOfInterest ooi : objects){
            if(viewedFrom.containsKey(ooi)){
                QoS.put(ooi,getQoS(ooi,viewedFrom.get(ooi)));
            }else{
                viewedFrom.put(ooi,null);
                QoS.put(ooi,0.0);
            }
        }
        //System.out.println(getSystemQoS());

        f.graphPanel.update();
        f.graphPanel.repaint();
        f.mp.repaint();
    }
    
    public int avg(int a, int b, int c){
        return (a + b + c) / 3;
    }
    
    public void setFrame(MLBFrame frame) {
        f = frame;
        
        //Give the entities list to the map panel for drawing
        f.mp.me = items;
        f.mp.ls = lastSeen;
        f.mp.as = assignments;
        f.mp.vf = viewedFrom;
        f.mp.qos = QoS;
        f.graphPanel.qos=QoS;
        initSimulation();
    }
    
    /*
    Controller protocol for user interfaces (in progress)
    */
    
    // Add a new robot/camera to the simulation
    public Robot addRobot(String name) {
        Robot r = new Robot(name);
        robots.add(r);
        items.add(r);
        simulate(r);
        f.update();
        return r;
    }
    
    // Removes a robot from the simulation
    public void removeRobot(Robot r) {
        robots.remove(r);
        items.remove(r);
        f.update();
    }
    
    // Adds a new object of interest to the simulation
    public ObjectOfInterest addObject(String name, Color c) {
        ObjectOfInterest ooi = new ObjectOfInterest(name, c);
        objects.add(ooi);
        items.add(ooi);
        f.update();
        return ooi;
    }
    
    // Removes an object of interest from the simulation
    public void removeObject(ObjectOfInterest ooi) {
        objects.remove(ooi);
        items.remove(ooi);
        f.update();
    }
    
    // Tells a robot to track a particular object.
    // TODO: allow robots to track multiple objects???
    public void startTracking(Robot r, ObjectOfInterest ooi) {
        assignments.put(r, ooi);
    }
    
    // Tells a robot to stop tracking any objects it is currently tracking.
    public void stopTracking(Robot r) {
        assignments.remove(r);
    }
    
    // Returns true if the robot is currently tracking an object.
    public boolean isTracking(Robot r) {
        return (assignments.containsKey(r) && assignments.get(r) != null);
    }
    
    // Returns the object that the robot is currently tracking.
    public ObjectOfInterest getTrackingObject(Robot r) {
        return assignments.get(r);   
    }
    public double getSystemQoS(){
        double ret = 0;
        for(ObjectOfInterest ooi : objects){
            ret += getQoS(ooi,viewedFrom.get(ooi))*demand.get(ooi);
        }
        return ret;
    }
    private void optimizeQoS() {
        
        for(ObjectOfInterest ooi : objects){
            if(!demand.containsKey(ooi)){
                demand.put(ooi,0.0);
            }
        }
        //Find max demand
        assignments.clear();
        while(true){
            double maxDemand = 0;
            ObjectOfInterest o = null;
            for(ObjectOfInterest ooi : demand.keySet()){
                if(!assignments.containsValue(ooi)&&(maxDemand==0||demand.get(ooi)>=maxDemand)){
                    maxDemand = demand.get(ooi);
                    o = ooi;
                }
            }
            if(o==null)
                break;
            System.out.print(o.name);
            double maxQoS = 0;
            Robot r = null;
            for(Robot rr : robots){
                if(!assignments.containsKey(rr)&&(maxDemand==0||getAssQoS(o,rr)>=maxQoS)){
                    maxQoS = getAssQoS(o,rr);
                    r = rr;
                }
            }
            if(r==null)
                break;
            System.out.print(r.name);
            assignments.put(r,o);
            viewedFrom.put(o,r);
        }
        System.out.println();


        /*
        for(ObjectOfInterest ooi : objects){
            Robot maxQoS = null;
            double d = 0;
            for(Robot r : robots){
                if(getQoS(ooi,r)>0&&(maxQoS==null||getQoS(ooi,r)>d)){
                    maxQoS = r;
                    d = getQoS(ooi,r);
                }
            }
            viewedFrom.put(ooi,maxQoS);
            QoS.put(ooi,d);
        }
        for(ObjectOfInterest ooi : viewedFrom.keySet()){
            if(viewedFrom.get(ooi)!=null){
                if(assignments.containsValue(ooi)){
                    Robot trackedBy = null;
                    for(Robot r : assignments.keySet()){
                        if(assignments.get(r)==ooi){
                            trackedBy = r;
                        }
                    }
                    if(trackedBy!=null){
                        if(trackedBy!=viewedFrom.get(ooi)){
                            ObjectOfInterest tmp = assignments.get(viewedFrom.get(ooi));
                            assignments.put(viewedFrom.get(ooi),ooi);
                            assignments.put(trackedBy, tmp);
                        }
                    }
                }
            }
         
        }*/
    }
    public double getQoS(ObjectOfInterest ooi, Robot r){
        if(ooi==null || r==null){
            return 0;
        }
        double distMetric = 0;
        double visibleMetric = 0;
        if(!lastSeen.containsKey(ooi)||lastSeen.get(ooi)==null){
            return 0;
        }
        if(!r.canSee(lastSeen.get(ooi))){
            visibleMetric = 0;
        }else{
            visibleMetric = 1;
        }

        if(lastSeen.get(ooi).distance(r.pos)<ooi.OPTIMAL_VIEWING_DISTANCE){
            distMetric =  lastSeen.get(ooi).distance(r.pos)/(ooi.OPTIMAL_VIEWING_DISTANCE);
        }else{
            distMetric =  ooi.OPTIMAL_VIEWING_DISTANCE/(lastSeen.get(ooi).distance(r.pos));
        }
        return distMetric * visibleMetric;
        //return Math.exp(-Math.abs(lastSeen.get(ooi).distance(r.pos))/100)*(r.canSee(ooi.pos)?1:0);
    }
    public double getAssQoS(ObjectOfInterest ooi, Robot r){
        if(ooi==null || r==null){
            return 0;
        }
        double distMetric = 0;
      
        if(!lastSeen.containsKey(ooi)||lastSeen.get(ooi)==null){
            return 0;
        }
       

        if(lastSeen.get(ooi).distance(r.pos)<ooi.OPTIMAL_VIEWING_DISTANCE){
            distMetric =  lastSeen.get(ooi).distance(r.pos)/(ooi.OPTIMAL_VIEWING_DISTANCE);
        }else{
            distMetric =  ooi.OPTIMAL_VIEWING_DISTANCE/(lastSeen.get(ooi).distance(r.pos));
        }
        return distMetric ;
        //return Math.exp(-Math.abs(lastSeen.get(ooi).distance(r.pos))/100)*(r.canSee(ooi.pos)?1:0);
    }
}
