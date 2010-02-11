/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package magiclegoblimps;

import java.awt.geom.Point2D.Double;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.util.*;
/**
 *
 * @author Kevin Hulin
 */
class MLBFrame extends JFrame{
    public final static int GRID_X = 10;
    public final static int GRID_Y = 8;
    public final static int SPACING = 60;
    public final static int TRIM = 20;
    public RobotMapPanel mp;
    public MLBFrame(){
        
        setSize(700,700);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLayout(new BorderLayout());
        mp = new RobotMapPanel(GRID_X, GRID_Y, SPACING);
        add(mp, BorderLayout.CENTER);
        JPanel buttons = new JPanel(new GridLayout(0,2));

        JButton bLeft = new JButton("Left");
        JButton bRight = new JButton("Right");
        bLeft.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                for(MovableEntity o : mp.me){
                    if(o instanceof Robot)
                        ((Robot)o).nextLeft();
                }

            }
        });
        bRight.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                for(MovableEntity o : mp.me){
                    if(o instanceof Robot)
                        ((Robot)o).nextRight();
                }
            }
        });
        buttons.add(bLeft,0);
        buttons.add(bRight,1);
        add(buttons,BorderLayout.SOUTH);

        setVisible(true);

    }
}

class RobotMapPanel extends JPanel{
    Color backgroundColor = Color.GRAY;
    BufferedImage bi = null;
    int x_count;
    int y_count;
    int spacing;
    int trim = MLBFrame.TRIM;
    ArrayList<MovableEntity> me = null;
    HashMap<ObjectOfInterest, Point2D.Double> ls;
    HashMap<magiclegoblimps.Robot, ObjectOfInterest> as;


    public RobotMapPanel(int x, int y, int s){
        setSize(500,500);
        x_count = x;
        y_count = y;
        spacing = s;

    }


    @Override
    public void paintComponent(Graphics g){

           bi = new BufferedImage(getWidth(), getHeight(), BufferedImage.TYPE_INT_ARGB);
           paintMe(bi.getGraphics());
           g.drawImage(bi,0,0,null);

        
    }
    public void paintMe(Graphics g){
        //Clear Screen
        g.setColor(backgroundColor);
        ((Graphics2D)g).setStroke(new BasicStroke(4));
        g.fillRect(0,0,getWidth(),getHeight());
        //Draw Grid
        g.setColor(Color.BLACK);
        for(int i = 0; i < x_count; i++){
            for(int j = 0; j < y_count; j++){
                g.drawRect(trim+i*spacing, trim+j*spacing, spacing, spacing);
            }
        }
        if(me!=null){
            for(MovableEntity rr : me){
                if(rr instanceof Robot)
                    g.setColor(new Color(255,0,0));
                else if(rr instanceof ObjectOfInterest)
                    g.setColor(new Color(220,150,25));
                else
                    g.setColor(Color.BLACK);
                rr.draw(g);
            }
        if(as!=null){
            int i = 0;
            g.setColor(Color.BLUE);
            g.drawString("Assignments:",MLBFrame.TRIM, MLBFrame.GRID_Y*MLBFrame.SPACING+MLBFrame.TRIM*2 + i*14);
            i++;
            for(Robot r : as.keySet()){
                
                g.drawString(r.name + " : " + as.get(r).name, MLBFrame.TRIM, MLBFrame.GRID_Y*MLBFrame.SPACING+MLBFrame.TRIM*2 + i*14);
                
                i++;
            }
        }

        }
        g.setColor(Color.WHITE);
        if(ls!=null){
            for(ObjectOfInterest o : ls.keySet()){
                if(ls.get(o)!=null){
                    g.drawString(o.name,(int)(ls.get(o).x-20),(int)(ls.get(o).y-10));
                    g.drawString("X",(int)ls.get(o).x,(int)ls.get(o).y);
                }
            }
        }
    }
}
abstract class MovableEntity{

    Point2D.Double pos;
    double dir = 0;
    double speed = 0;
    String name = "DEFAULT_NAME";
    public abstract void move();
    abstract void draw(Graphics g);
}
class ObjectOfInterest extends MovableEntity{
    public static final Point2D.Double DEFAULT_POI_POS = new Point2D.Double(100,100);
    public static final int POI_RAIDUS = 4;
    private static Random rand = new Random();
    public ObjectOfInterest(String s){
        super();
        name = s;
        speed = 2;
        pos = (Point2D.Double)DEFAULT_POI_POS.clone();
        dir = 0;
        //System.out.println(s.length());
    }
    public void move(){
        System.out.println(name + " " + (int)(dir/(Math.PI/2)));
        double d = rand.nextDouble();
        if(d>.5){
            dir += .1;
        }else{
            dir += -.1;
        }
        //System.out.println(dir);
        if(pos.x < MLBFrame.TRIM){
            dir -= Math.PI;
            pos.x = MLBFrame.TRIM;
        }else if(pos.x > MLBFrame.TRIM + MLBFrame.SPACING * MLBFrame.GRID_X){
            dir -= Math.PI;
            pos.x = MLBFrame.TRIM + MLBFrame.SPACING * MLBFrame.GRID_X;
        }else{
            pos.x = pos.x + speed*Math.cos(dir);
        }
        
        if(pos.y < MLBFrame.TRIM){
            dir -= Math.PI;
            pos.y = MLBFrame.TRIM;
        }else if(pos.y > MLBFrame.TRIM + MLBFrame.SPACING * MLBFrame.GRID_Y){
            dir -= Math.PI;
            pos.y = MLBFrame.TRIM + MLBFrame.SPACING * MLBFrame.GRID_Y;
        }else{
            pos.y = pos.y + speed*Math.sin(dir);
        }
        

    }
    void draw(Graphics g){
        g.drawString(name,(int)(pos.x-POI_RAIDUS-3*(name.length()/2)),(int)(pos.y-POI_RAIDUS-3));

        g.fillRect((int)(pos.x-POI_RAIDUS),(int)(pos.y-POI_RAIDUS),(int)(2*POI_RAIDUS), (int)(2*POI_RAIDUS));

    }

}
class Robot extends MovableEntity{

    public static final int ROBOT_RADIUS = 6; //in pixles for drawing
    public static final double ROBOT_ACCEL = 1; //pixles per tick^2 Not really used...
    public static final double ROBOT_SPEED = 1; //pixles per tick
    public static final double FRICTION = -.3; //stopping accel... again not really used
    public static final Point2D.Double START_POS = new Point2D.Double(20,20); //Robot starting position
    public static final int GRID_WIDTH = MLBFrame.SPACING;//grid width in pixles
    public static final int TRIM = MLBFrame.TRIM;//border trim in pixles
    public static final double CAMERA_ANGLE = Math.PI/3;//viewing angle of camera
    public static final double CAMERA_DIST = 450;//how far the camera can see
    public static final double SWIVEL_SPEED = .02;//how fast the camera rotates

    
    public double accel = 0;
    public double cameraDir = 0;
    public double cameraDirTheta = 0;
    
    public boolean left = false;
    public boolean right = false;

    private boolean[] currMoves = null;
    private Polygon viewingArea = null;

    public Robot(String s){
        name = s;
        pos = (Point2D.Double)START_POS.clone();
    }
    public void swivelLeft(){
        cameraDirTheta = -SWIVEL_SPEED;
    }
    public void swivelRight(){
        cameraDirTheta = SWIVEL_SPEED;
    }
    public void stopSwivel(){
        cameraDirTheta = 0;
    }
    public void nextLeft(){
        left = true;
        right = false;
    }
    public void nextRight(){
        right = true;
        left = false;
    }
    public void turnLeft(){
        dir += Math.PI/2;
    }
    public void turnRight(){
        dir -= Math.PI/2;
    }
    public void go(){
        accel = ROBOT_ACCEL;
    }
    public void stop(){
        accel = FRICTION;
    }
    public void goTo(Point2D.Double p){
        if(p==null){
            p = new Point2D.Double(MLBFrame.GRID_X*MLBFrame.SPACING/2+MLBFrame.TRIM, MLBFrame.GRID_Y*MLBFrame.SPACING/2+MLBFrame.TRIM);

        }
       // System.out.println(p.toString());
        if(Math.abs(Math.sin(dir))<.00001&&Math.abs(Math.cos(dir)-1)<.00001){//Moving East
           // System.out.println("Moving East"+ "(" + pos.x + ","+pos.y +")");
            if(p.x<pos.x-MLBFrame.SPACING/2||Math.abs(p.x-pos.x)<MLBFrame.SPACING/2){

                if(Math.abs(p.x-pos.x)<MLBFrame.SPACING/2&&Math.abs(p.y-pos.y)<MLBFrame.SPACING/2){
                    stop();
                }
                else if(p.y < pos.y){
                    go();
                    nextLeft();
                }else{
                    go();
                    nextRight();
                }
            }else{
                go();
            }
        }
        else if (Math.abs(Math.sin(dir)-1)<.00001 && Math.abs(Math.cos(dir))<.00001){//Moving North
           // System.out.println("Moving North"+ "(" + pos.x + ","+pos.y +")");

            if(p.y<pos.y-MLBFrame.SPACING/2||Math.abs(p.y-pos.y)<MLBFrame.SPACING/2){

                if(Math.abs(p.y-pos.y)<MLBFrame.SPACING/2&&Math.abs(p.x-pos.x)<MLBFrame.SPACING/2){
                    stop();
                }
                else if(p.x < pos.x){
                    go();
                    nextLeft();
                }else{
                    go();
                    nextRight();
                }
            }else{
                go();
            }
        }
        else if(Math.abs(Math.sin(dir))<.00001 && Math.abs(Math.cos(dir)+1)<.00001){//Moving West
            //System.out.println("Moving West"+ "(" + pos.x + ","+pos.y +")");
            if(p.x>pos.x-MLBFrame.SPACING/2||Math.abs(p.x-pos.x)<MLBFrame.SPACING/2){

                if(Math.abs(p.x-pos.x)<MLBFrame.SPACING/2&&Math.abs(p.y-pos.y)<MLBFrame.SPACING/2){
                    stop();
                }
                else if(p.y < pos.y){
                    go();
                    nextRight();
                }else{
                    go();
                    nextLeft();
                }
            }else{
                go();
            }
        }
        else if(Math.abs(Math.sin(dir)+1)<.00001 && Math.abs(Math.cos(dir))<.00001){//Moving South
           // System.out.println("Moving South"+ "(" + pos.x + ","+pos.y +")");
            if(p.y>pos.y-MLBFrame.SPACING/2||Math.abs(p.y-pos.y)<MLBFrame.SPACING/2){

                if(Math.abs(p.y-pos.y)<MLBFrame.SPACING/2&&Math.abs(p.x-pos.x)<MLBFrame.SPACING/2){
                    stop();
                }
                else if(p.x < pos.x){
                    go();
                    nextRight();
                }else{
                    go();
                    nextLeft();
                }
            }else{
                go();
            }



        }else{
                System.out.println("FAIL");
            }
    }
    public boolean[] validMoves(){
        double delta = .01;
        boolean[] ret = new boolean[]{false,false,false};//Can Move forward, left, right

        if(Math.abs(Math.sin(dir))<.00001&&Math.abs(Math.cos(dir)-1)<.00001){//Moving East
           // System.out.println("Moving East"+ "(" + pos.x + ","+pos.y +")");
            //Can continue in +X Direction?
            if( pos.x < TRIM + GRID_WIDTH * MLBFrame.GRID_X)
                ret[0] = true;
            //Can turn?
            if(!ret[0] ||fpart((pos.x-TRIM)/GRID_WIDTH)<delta){//Is at an intersection?
                //Turn left?
                if(pos.y > TRIM){
                    ret[1] = true;
                }
                //Turn right?
                if(pos.y <  TRIM + GRID_WIDTH * MLBFrame.GRID_Y){
                    ret[2] = true;
                }
            }
        }
        else if (Math.abs(Math.sin(dir)-1)<.00001 && Math.abs(Math.cos(dir))<.00001){//Moving North
           // System.out.println("Moving North"+ "(" + pos.x + ","+pos.y +")");
            //Can continue in -Y Direction?
            if( pos.y > TRIM )
                ret[0] = true;
            //Can turn?
            if(!ret[0] ||fpart((pos.y-TRIM)/GRID_WIDTH)<delta){//Is at an intersection?
                //Turn left?
                if(pos.x > TRIM){
                    ret[1] = true;
                }
                //Turn right?
                if(pos.x <  GRID_WIDTH * MLBFrame.GRID_X){
                    ret[2] = true;
                }
            }
        }
        else if(Math.abs(Math.sin(dir))<.00001 && Math.abs(Math.cos(dir)+1)<.00001){//Moving West
           // System.out.println("Moving West"+ "(" + pos.x + ","+pos.y +")");
            //Can continue in -X Direction?
            if( pos.x > TRIM )
                ret[0] = true;
            //Can turn?
            if(!ret[0] || fpart((pos.x-TRIM)/GRID_WIDTH)<delta){//Is at an intersection?
                //Turn right?
                if(pos.y > TRIM){
                    ret[2] = true;
                }
                //Turn left?
                if(pos.y <  TRIM + GRID_WIDTH * MLBFrame.GRID_Y){
                    ret[1] = true;
                }
            }
        }
        else if(Math.abs(Math.sin(dir)+1)<.00001 && Math.abs(Math.cos(dir))<.00001){//Moving South
          //  System.out.println("Moving South"+ "(" + pos.x + ","+pos.y +")");
            //Can continue in +Y Direction?
           
            if( pos.y < TRIM + GRID_WIDTH * MLBFrame.GRID_Y )
                ret[0] = true;
            //Can turn?
            if(!ret[0] ||fpart((pos.y-TRIM)/GRID_WIDTH)<delta){//Is at an intersection?
                //Turn right?
                if(pos.x > TRIM){
                    ret[2] = true;
                }
                //Turn left?
                if(pos.x <  GRID_WIDTH * MLBFrame.GRID_X){
                    ret[1] = true;
                }
            }
            
        
       
        }else{
                System.out.println("FAIL");
            }



        return ret;
    }
    public double fpart(double d){
        return d - Math.floor(d);
    }
    public void move(){
        currMoves = validMoves();
        //System.out.println(dir);
        if(currMoves[0]){
            pos.x = pos.x + speed*Math.cos(dir);
            pos.y = pos.y - speed*Math.sin(dir);  
        }

        if(left && currMoves[1]){
            turnLeft();
            left = false;
        }else if(right && currMoves[2]){
            turnRight();
            right = false;
        }

        speed = accel<0 ? Math.max(0,speed+accel) : Math.min(ROBOT_SPEED,speed+accel);
        cameraDir += cameraDirTheta;

        viewingArea = new Polygon(new int[]{(int)pos.x,(int)(pos.x+CAMERA_DIST*Math.cos(dir+cameraDir-CAMERA_ANGLE/2)),(int)(pos.x+CAMERA_DIST*Math.cos(dir+cameraDir+CAMERA_ANGLE/2))},
                new int[]{(int)pos.y,(int)(pos.y-CAMERA_DIST*Math.sin(dir+cameraDir-CAMERA_ANGLE/2)),(int)(pos.y-CAMERA_DIST*Math.sin(dir+cameraDir+CAMERA_ANGLE/2))},3);

    }
    public boolean canSee(Point2D p){
        if(p==null) return false;
        return viewingArea.contains(p);

    }
    public boolean centerOnObject(Point2D.Double p, MovableEntity me){//Returns false if object is not in view
        if(p==null){
            return false;
        }
        if(!canSee(p)){  //If the camera can't see the object, dont try to center on it
            if(p.x > pos.x && p.y > pos.y){
                if(Math.atan((p.x-pos.x)/(p.y-pos.y))<0){
                    swivelLeft();
                }else{
                    swivelRight();
                }
            }
            return false;
        }
        else{
            if(canSee(p)&&!canSee(me.pos)){
                return false;
            }
            Polygon lView = new Polygon(new int[]{viewingArea.xpoints[0],viewingArea.xpoints[1],(viewingArea.xpoints[1]+viewingArea.xpoints[2])/2},
                    new int[]{viewingArea.ypoints[0],viewingArea.ypoints[1],(viewingArea.ypoints[1]+viewingArea.ypoints[2])/2},3);
            if(lView.contains(p)){
                swivelLeft();
            }else{
                swivelRight();
            }
            return true;
        }

    }
    void draw(Graphics g){
        g.drawString(name,(int)(pos.x-ROBOT_RADIUS-3*(name.length()/2)),(int)(pos.y-ROBOT_RADIUS-3));
        Color tmp = g.getColor();
        if(viewingArea!=null){
            g.setColor(Color.GREEN);
            ((Graphics2D)g).setStroke(new BasicStroke(1));

            g.drawPolygon(viewingArea);
        }
        g.setColor(tmp);
        g.fillOval((int)(pos.x-ROBOT_RADIUS),(int)(pos.y-ROBOT_RADIUS),(int)(2*ROBOT_RADIUS), (int)(2*ROBOT_RADIUS));
        

    }
    
    
}

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

class RobotSimProgram{
    MLBFrame mlbf;
    MLBController mlbc;
    public RobotSimProgram(){
    }

    public void start(){
         mlbf = new MLBFrame();
         mlbc = new MLBController(mlbf);
    }

}